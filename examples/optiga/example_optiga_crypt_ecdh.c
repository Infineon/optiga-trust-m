/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file example_optiga_crypt_ecdh.c
 *
 * \brief    This file provides the example for ECDH operation using #optiga_crypt_ecdh.
 *
 * \ingroup grOptigaExamples
 *
 * @{
 */

#include "optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_ECDH_ENABLED

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

// Peer public key details for the ECDH operation
static uint8_t peer_public_key[] = {
    // Bit string format
    0x03,
    // Remaining length
    0x42,
    // Unused bits
    0x00,
    // Compression format
    0x04,
    // Public Key
    0x94,
    0x89,
    0x2F,
    0x09,
    0xEA,
    0x4E,
    0xCA,
    0xBC,
    0x6A,
    0x4E,
    0xF2,
    0x06,
    0x36,
    0x26,
    0xE0,
    0x5D,
    0xE0,
    0xD5,
    0xF9,
    0x77,
    0xEA,
    0xC3,
    0xB2,
    0x70,
    0xAC,
    0xE2,
    0x19,
    0x00,
    0xF5,
    0xDB,
    0x56,
    0xE7,
    0x37,
    0xBB,
    0xBE,
    0x46,
    0xE4,
    0x49,
    0x76,
    0x38,
    0x25,
    0xB5,
    0xF8,
    0x94,
    0x74,
    0x9E,
    0x1A,
    0xB6,
    0x5A,
    0xF1,
    0x29,
    0xD7,
    0x3A,
    0xB6,
    0x9B,
    0x80,
    0xAC,
    0xC5,
    0xE1,
    0xC3,
    0x10,
    0xF2,
    0x16,
    0xC6,
};
static public_key_from_host_t peer_public_key_details = {
    (uint8_t *)peer_public_key,
    sizeof(peer_public_key),
    (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256,
};

/**
 * The below example demonstrates the generation of
 * shared secret using #optiga_crypt_ecdh with a session based approach.
 *
 */
void example_optiga_crypt_ecdh(void) {
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    uint32_t time_taken = 0;
    // To store the generated public key as part of Generate key pair
    uint8_t public_key[100];
    uint16_t public_key_length = sizeof(public_key);

    // To store the generated shared secret as part of ECDH
    uint8_t shared_secret[32];

    optiga_crypt_t *me = NULL;

    do {
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        example_optiga_init();
#endif  // OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

        OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
        /**
         * 1. Create OPTIGA Crypt Instance
         */
        me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == me) {
            break;
        }

        /**
         * 2. Generate ECC Key pair - To use the private key with ECDH in the next step
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage as Key Agreement
         *       - Store the Private key with in OPTIGA Session
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;

        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_COMMAND_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
            me,
            OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
        );
        return_status = optiga_crypt_ecc_generate_keypair(
            me,
            OPTIGA_ECC_CURVE_NIST_P_256,
            (uint8_t)OPTIGA_KEY_USAGE_KEY_AGREEMENT,
            FALSE,
            &optiga_key_id,
            public_key,
            &public_key_length
        );
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        START_PERFORMANCE_MEASUREMENT(time_taken);

        /**
         * 3. Perform ECDH using the Peer Public key
         *       - Use ECC NIST P 256 Curve
         *       - Provide the peer public key details
         *       - Export the generated shared secret with protected I2C communication
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_COMMAND_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
            me,
            OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
        );
        return_status =
            optiga_crypt_ecdh(me, optiga_key_id, &peer_public_key_details, TRUE, shared_secret);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        READ_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif  // OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);

    if (me) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}

#endif  // OPTIGA_CRYPT_ECDH_ENABLED
/**
 * @}
 */
