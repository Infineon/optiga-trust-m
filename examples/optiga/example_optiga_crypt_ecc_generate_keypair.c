/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file example_optiga_crypt_ecc_generate_keypair.c
 *
 * \brief   This file provides the example for generation of ecc keypair using
 *          optiga_crypt_ecc_generate_keypair.
 *
 * \ingroup grOptigaExamples
 *
 * @{
 */
#include "optiga_crypt.h"
#include "optiga_example.h"
#include "optiga_util.h"

#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

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
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}
/**
 * Sample metadata of 0xE0F1
 */
const uint8_t E0F1_metadata[] = {0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00};

/**
 * The below example demonstrates the generation of
 * ECC Key pair using #optiga_crypt_ecc_generate_keypair.
 *
 */
void example_optiga_crypt_ecc_generate_keypair(void) {
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    uint32_t time_taken = 0;
    // To store the generated public key as part of Generate key pair
    uint8_t public_key[100];
    uint16_t public_key_length = sizeof(public_key);
    uint16_t optiga_oid;

    optiga_crypt_t *crypt_me = NULL;
    optiga_util_t *util_me = NULL;

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
        crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == crypt_me) {
            break;
        }

        util_me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == util_me) {
            break;
        }

        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_oid = 0xE0F1;
        return_status =
            optiga_util_write_metadata(util_me, optiga_oid, E0F1_metadata, sizeof(E0F1_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        START_PERFORMANCE_MEASUREMENT(time_taken);

        /**
         * 2. Generate ECC Key pair
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage (Key Agreement or Sign based on requirement)
         *       - Store the Private key in OPTIGA Key store
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_E0F1;
        // for Session based, use OPTIGA_KEY_ID_SESSION_BASED as key id as shown below.
        // optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;
        return_status = optiga_crypt_ecc_generate_keypair(
            crypt_me,
            OPTIGA_ECC_CURVE_NIST_P_256,
            (uint8_t)OPTIGA_KEY_USAGE_SIGN,
            FALSE,
            &optiga_key_id,
            public_key,
            &public_key_length
        );
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

    if (crypt_me) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(crypt_me);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    if (util_me) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(util_me);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}

#endif  // OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED
/**
 * @}
 */
