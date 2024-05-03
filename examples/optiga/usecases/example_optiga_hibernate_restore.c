/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file example_optiga_hibernate_restore.c
 *
 * \brief   This file provides an example for hibernate and restore functionalities
 *
 * \ingroup grUseCases
 *
 * @{
 */

#include "optiga_crypt.h"
#include "optiga_example.h"
#include "optiga_util.h"
#include "pal_os_timer.h"

#if defined(OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) && defined(OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) \
    && defined(OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED)

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
// lint --e{526} suppress "the function is defined in example_pair_host_and_optiga_using_pre_shared_secret source file"
extern optiga_lib_status_t pair_host_and_optiga_using_pre_shared_secret(void);
#endif
/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_lib_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

// SHA-256 Digest to be signed
static const uint8_t digest[] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

/**
 * The below example demonstrates hibernate and restore functionalities
 *
 * Example for #optiga_util_open_application and #optiga_util_close_application
 *
 */
void example_optiga_util_hibernate_restore(void) {
    optiga_util_t *me_util = NULL;
    optiga_crypt_t *me_crypt = NULL;
    // To store the public key generated
    uint8_t public_key[100];
    // To store the signature generated
    uint8_t signature[80];
    uint16_t signature_length = sizeof(signature);
    uint16_t bytes_to_read = 1;
    optiga_key_id_t optiga_key_id;
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    uint8_t security_event_counter = 0;
    public_key_from_host_t public_key_details;
    // To store the generated public key as part of Generate key pair
    uint16_t public_key_length = sizeof(public_key);
    uint32_t time_taken = 0;

    OPTIGA_EXAMPLE_LOG_MESSAGE("Begin demonstrating hibernate feature...\n");
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do {
        // Create an instance of optiga_util and optiga_crypt
        me_util = optiga_util_create(0, optiga_lib_callback, NULL);
        if (NULL == me_util) {
            break;
        }

        me_crypt = optiga_crypt_create(0, optiga_lib_callback, NULL);
        if (NULL == me_crypt) {
            break;
        }

        /**
         * 1. Open the application on OPTIGA which is a pre-condition to perform any other operations
         *    using optiga_util_open_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;

        START_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = optiga_util_open_application(me_util, 0);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        /**
         * 2. Pairing the Host and OPTIGA using a pre-shared secret
         */
        return_status = pair_host_and_optiga_using_pre_shared_secret();
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // pairing of host and optiga failed
            break;
        }

#endif

        /**
         * 3. Generate ECC Key pair
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage (Key Agreement or Sign based on requirement)
         *       - Store the Private key generated in a Session OID
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;
        // OPTIGA Comms Shielded connection settings to enable the protection
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me_crypt, OPTIGA_COMMS_RESPONSE_PROTECTION);

        return_status = optiga_crypt_ecc_generate_keypair(
            me_crypt,
            OPTIGA_ECC_CURVE_NIST_P_256,
            (uint8_t)OPTIGA_KEY_USAGE_SIGN,
            FALSE,
            &optiga_key_id,
            public_key,
            &public_key_length
        );
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * - The subsequent steps will hibernate OPTIGA and save the session and optiga comms related information in OPTIGA .
         * - The session and optiga comms related information are then restored back and crypto operation are performed using these information.
         */

        /**
         * 4. To perform the hibernate, Security Event Counter(SEC) must be 0.
         *    Read SEC data object (0xE0C5) and wait until SEC = 0
         */
        do {
            optiga_lib_status = OPTIGA_LIB_BUSY;
            return_status = optiga_util_read_data(
                me_util,
                0xE0C5,
                0x0000,
                &security_event_counter,
                &bytes_to_read
            );

            WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
            pal_os_timer_delay_in_milliseconds(1000);
        } while (0 != security_event_counter);

        /**
         * 5. Hibernate the application on OPTIGA
         *    using optiga_util_close_application with perform_hibernate parameter as true
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me_util, 1);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 6. Restore the application on OPTIGA
         *    using optiga_util_open_application with perform_restore parameter as true
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_open_application(me_util, 1);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 7. Sign the digest using the session key from Step 3
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me_crypt, OPTIGA_COMMS_RESPONSE_PROTECTION);
        return_status = optiga_crypt_ecdsa_sign(
            me_crypt,
            digest,
            sizeof(digest),
            OPTIGA_KEY_ID_SESSION_BASED,
            signature,
            &signature_length
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 8. Verify ECDSA signature using public key from step 2
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        public_key_details.public_key = public_key;
        public_key_details.length = public_key_length;
        public_key_details.key_type = (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256;
        return_status = optiga_crypt_ecdsa_verify(
            me_crypt,
            digest,
            sizeof(digest),
            signature,
            signature_length,
            OPTIGA_CRYPT_HOST_DATA,
            &public_key_details
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 9. Close the application on OPTIGA without hibernating
         *    using optiga_util_close_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me_util, 0);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        READ_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = OPTIGA_LIB_SUCCESS;
        OPTIGA_EXAMPLE_LOG_MESSAGE("Hibernate feature demonstration completed...\n");
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);

    if (me_util) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me_util);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }

    if (me_crypt) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me_crypt);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}
#endif  // OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED && OPTIGA_CRYPT_ECDSA_SIGN_ENABLED && OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
/**
 * @}
 */
