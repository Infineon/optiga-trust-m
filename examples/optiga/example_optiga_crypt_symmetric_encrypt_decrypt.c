/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 * \brief   This file provides the example for symmetric encryption and decryption for CBC mode using
 *          optiga_crypt_symmetric_encrypt_start, optiga_crypt_symmetric_encrypt_continue,
 *          optiga_crypt_symmetric_encrypt_final, optiga_crypt_symmetric_decrypt_start,
 *          optiga_crypt_symmetric_decrypt_continue and optiga_crypt_symmetric_decrypt_final APIs.
 *          It also provides example for symmetric encryption for CBC MAC mode using optiga_crypt_symmetric_encrypt API.
 *
 * \ingroup grOptigaExamples
 *
 * @{
 */

#include "optiga_crypt.h"
#include "optiga_example.h"

#if defined(OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) && defined(OPTIGA_CRYPT_SYM_DECRYPT_ENABLED)

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

extern optiga_lib_status_t generate_symmetric_key(void);
/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_crypt_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

/**
 * The below example demonstrates the symmetric encryption and decryption for CBC mode using OPTIGA.
 *
 * Example for
 *  #optiga_crypt_symmetric_encrypt_start,
 *  #optiga_crypt_symmetric_encrypt_continue and
 *  #optiga_crypt_symmetric_encrypt_final for symmetric encryption.
 *  #optiga_crypt_symmetric_decrypt_start,
 *  #optiga_crypt_symmetric_decrypt_continue and
 *  #optiga_crypt_symmetric_decrypt_final for symmetric decryption.
 */
void example_optiga_crypt_symmetric_encrypt_decrypt_cbc(void) {
    const uint8_t plain_data_buffer_start[] = {
        0x6b,
        0xc1,
        0xbe,
        0xe2,
        0x2e,
        0x40,
        0x9f,
        0x96,
        0xe9,
        0x3d,
        0x7e,
        0x11,
        0x73,
        0x93,
        0x17,
        0x2a};
    const uint8_t plain_data_buffer_continue[] = {
        0x7c,
        0xd2,
        0xcf,
        0xf3,
        0x3f,
        0x51,
        0xa0,
        0xa7,
        0xf0,
        0x4e,
        0x8f,
        0x22,
        0x84,
        0xa4,
        0x28,
        0x3b};
    const uint8_t plain_data_buffer_final[] = {
        0x5a,
        0xb0,
        0xad,
        0xd1,
        0x1d,
        0x3f,
        0x8e,
        0x85,
        0xd8,
        0x2c,
        0x6d,
        0xf1,
        0x62,
        0x82,
        0x06,
        0x19};

    uint8_t encrypted_data_buffer_start[32] = {0};
    uint32_t encrypted_data_length_start = sizeof(encrypted_data_buffer_start);
    uint8_t encrypted_data_buffer_continue[32] = {0};
    uint32_t encrypted_data_length_continue = sizeof(encrypted_data_buffer_continue);
    uint8_t encrypted_data_buffer_final[32] = {0};
    uint32_t encrypted_data_length_final = sizeof(encrypted_data_buffer_final);
    uint8_t decrypted_data_buffer_start[32] = {0};
    uint32_t decrypted_data_length_start = sizeof(decrypted_data_buffer_start);
    uint8_t decrypted_data_buffer_continue[32] = {0};
    uint32_t decrypted_data_length_continue = sizeof(decrypted_data_buffer_continue);
    uint8_t decrypted_data_buffer_final[32] = {0};
    uint32_t decrypted_data_length_final = sizeof(decrypted_data_buffer_final);
    optiga_crypt_t *me = NULL;
    uint32_t time_taken = 0;
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;

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
         *
         */
        me = optiga_crypt_create(0, optiga_util_crypt_callback, NULL);
        if (NULL == me) {
            break;
        }

        /**
         * 2. Update AES 128 symmetric key using secure key update
         *
         */
        OPTIGA_EXAMPLE_LOG_MESSAGE("Symmetric key generation");
        return_status = generate_symmetric_key();
        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        /**
         * 3. Start encryption sequence and encrypt the plain data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;

        START_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = optiga_crypt_symmetric_encrypt_start(
            me,
            OPTIGA_SYMMETRIC_CBC,
            OPTIGA_KEY_ID_SECRET_BASED,
            plain_data_buffer_start,
            sizeof(plain_data_buffer_start),
            NULL,
            0,
            NULL,
            0,
            0,
            encrypted_data_buffer_start,
            &encrypted_data_length_start
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        if (encrypted_data_length_start != sizeof(plain_data_buffer_start)) {
            // Encrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        /**
         * 4. Continue encrypting the plain data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_encrypt_continue(
            me,
            plain_data_buffer_continue,
            sizeof(plain_data_buffer_continue),
            encrypted_data_buffer_continue,
            &encrypted_data_length_continue
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        if (encrypted_data_length_continue != sizeof(plain_data_buffer_continue)) {
            // Encrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        /**
         * 5. End encryption sequence and encrypt plain data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_encrypt_final(
            me,
            plain_data_buffer_final,
            sizeof(plain_data_buffer_final),
            encrypted_data_buffer_final,
            &encrypted_data_length_final
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        if (encrypted_data_length_final != sizeof(plain_data_buffer_final)) {
            // Encrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        /**
         * 6. Start decryption sequence and decrypt the encrypted data from step 3
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_decrypt_start(
            me,
            OPTIGA_SYMMETRIC_CBC,
            OPTIGA_KEY_ID_SECRET_BASED,
            encrypted_data_buffer_start,
            encrypted_data_length_start,
            NULL,
            0,
            NULL,
            0,
            0,
            decrypted_data_buffer_start,
            &decrypted_data_length_start
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        // Compare the decrypted data with plain data
        if (OPTIGA_LIB_SUCCESS
            != memcmp(
                plain_data_buffer_start,
                decrypted_data_buffer_start,
                decrypted_data_length_start
            )) {
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        if (decrypted_data_length_start != sizeof(plain_data_buffer_start)) {
            // Decrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        /**
         * 7. Continue to decrypt the encrypted data from step 4
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_decrypt_continue(
            me,
            encrypted_data_buffer_continue,
            encrypted_data_length_continue,
            decrypted_data_buffer_continue,
            &decrypted_data_length_continue
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        // Compare the decrypted data with plain data
        if (OPTIGA_LIB_SUCCESS
            != memcmp(
                plain_data_buffer_continue,
                decrypted_data_buffer_continue,
                decrypted_data_length_continue
            )) {
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        if (sizeof(plain_data_buffer_continue) != decrypted_data_length_continue) {
            // Decrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        /**
         * 8. End decryption sequence and decrypt encrypted data from step 5
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_decrypt_final(
            me,
            encrypted_data_buffer_final,
            encrypted_data_length_final,
            decrypted_data_buffer_final,
            &decrypted_data_length_final
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        READ_PERFORMANCE_MEASUREMENT(time_taken);

        // Compare the decrypted data with plain data
        if (OPTIGA_LIB_SUCCESS
            != memcmp(
                plain_data_buffer_final,
                decrypted_data_buffer_final,
                decrypted_data_length_final
            )) {
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        if (sizeof(plain_data_buffer_final) != decrypted_data_length_final) {
            // Decrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
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

/**
 * The below example demonstrates the symmetric encryption for CBC MAC mode using OPTIGA.
 *
 * Example for #optiga_crypt_symmetric_encrypt.
 *
 */
void example_optiga_crypt_symmetric_encrypt_cbcmac(void) {
    const uint8_t plain_data_buffer[] = {
        0x6b,
        0xc1,
        0xbe,
        0xe2,
        0x2e,
        0x40,
        0x9f,
        0x96,
        0xe9,
        0x3d,
        0x7e,
        0x11,
        0x73,
        0x93,
        0x17,
        0x2a};
    uint8_t encrypted_data_buffer[32] = {0};
    uint32_t encrypted_data_length = sizeof(encrypted_data_buffer);
    uint32_t expected_data_length = 0x10;
    uint32_t time_taken = 0;
    optiga_crypt_t *me = NULL;
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;

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
         *
         */
        me = optiga_crypt_create(0, optiga_util_crypt_callback, NULL);
        if (NULL == me) {
            break;
        }

        /**
         * 2. Update symmetric key using secure key update
         *
         */
        OPTIGA_EXAMPLE_LOG_MESSAGE("Symmetric key generation");
        return_status = generate_symmetric_key();
        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        /**
         * 3. Encrypt the plain data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;

        START_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = optiga_crypt_symmetric_encrypt(
            me,
            OPTIGA_SYMMETRIC_CBC,
            OPTIGA_KEY_ID_SECRET_BASED,
            plain_data_buffer,
            sizeof(plain_data_buffer),
            NULL,
            0,
            NULL,
            0,
            encrypted_data_buffer,
            &encrypted_data_length
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        READ_PERFORMANCE_MEASUREMENT(time_taken);

        if (encrypted_data_length != expected_data_length) {
            // Encrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }

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

#endif  // OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED && OPTIGA_CRYPT_SYM_DECRYPT_ENABLED
/**
 * @}
 */
