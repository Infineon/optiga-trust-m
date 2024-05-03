/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_crypt_mbedtls_unit_test.c
 *
 * \brief   This file implements the OPTIGA PAL Crypt mbedtls Unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "pal_crypt_mbedtls_unit_test.h"

void print_hex(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void pal_crypt_version_unit_test() {
    pal_status_t ut_pal_status;
    uint8_t ut_crypt_lib_version_info[20];
    uint16_t ut_crypt_lib_version_info_len = 20;

    /* pal_crypt_version check
     * check that correct version is read and check content + length
     */
    ut_pal_status = pal_crypt_version(ut_crypt_lib_version_info, &ut_crypt_lib_version_info_len);
    assert(ut_pal_status == PAL_STATUS_SUCCESS);
    assert(ut_crypt_lib_version_info_len == (uint8_t)strlen(MBEDTLS_VERSION_STRING));
    assert(strcmp((char *)ut_crypt_lib_version_info, (char *)MBEDTLS_VERSION_STRING) == 0);

    /* pal_crypt_version check
     * if length is wrong status should be failure
     */
    ut_crypt_lib_version_info_len = 2;
    ut_pal_status = pal_crypt_version(ut_crypt_lib_version_info, &ut_crypt_lib_version_info_len);
    assert(ut_pal_status == PAL_STATUS_FAILURE);
}

void pal_crypt_tls_prf_sha256_unit_test() {
    pal_status_t ut_pal_status;
    uint8_t label_input[] = "Unit test";
    uint8_t secret_input[OPTIGA_SHARED_SECRET_MAX_LENGTH];
    uint16_t shared_secret_length;
    uint8_t random_key[RANDOM_KEY_LENGTH];
    uint8_t session_key[SESSION_KEY_LENGTH];

    /* Read binding secret from datastore */
    ut_pal_status = pal_os_datastore_read(
        OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID,
        secret_input,
        &shared_secret_length
    );
    assert(ut_pal_status == PAL_STATUS_SUCCESS);

    /* Seed the random number generator with the current time */
    srand(time(NULL));

    /* Generate random key and session_key */
    for (int i = 0; i < RANDOM_KEY_LENGTH; i++) {
        random_key[i] = rand() % 256;
    }
    for (int i = 0; i < SESSION_KEY_LENGTH; i++) {
        session_key[i] = rand() % 256;
    }

    printf("keys before computation : \n");

    /* printing the original keys */
    print_hex(session_key, SESSION_KEY_LENGTH);

    /* pal_crypt_tls_prf_sha256 check */
    ut_pal_status = pal_crypt_tls_prf_sha256(
        NULL,
        secret_input,
        shared_secret_length,
        label_input,
        sizeof(label_input) - 1,
        random_key,
        RANDOM_KEY_LENGTH,
        session_key,
        SESSION_KEY_LENGTH
    );

    assert(ut_pal_status == PAL_STATUS_SUCCESS);

    printf("keys after computation : \n");

    /* printing the derived keys */
    print_hex(session_key, SESSION_KEY_LENGTH);
}

void pal_crypt_encrypt_decrypt_aes128_ccm_unit_test() {
    pal_status_t ut_pal_status;

    char *ut_plaintext = "AES128 CCM Enc-Dec";
    uint16_t ut_plaintext_len = strlen(ut_plaintext);
    unsigned char ut_plaintext_decrypted[ut_plaintext_len];
    char *ut_associated_data = "Unit Test";
    uint16_t ut_associated_data_len = strlen(ut_associated_data);
    unsigned char ut_encrypt_key[AES_BLOCK_SIZE] = {
        0x2b,
        0x7e,
        0x15,
        0x16,
        0x28,
        0xae,
        0xd2,
        0xa6,
        0xab,
        0xf7,
        0x15,
        0x88,
        0x09,
        0xcf,
        0x4f,
        0x3c};
    unsigned char ut_nonce[NONCE_SIZE];
    unsigned char ut_ciphertext[ut_plaintext_len + IFX_PRL_MAC_SIZE];
    memset(ut_ciphertext, 0, sizeof(ut_ciphertext));

    /* Seed the random number generator with the current time */
    srand(time(NULL));

    /* Generate random nonce */
    for (int i = 0; i < NONCE_SIZE; i++) {
        ut_nonce[i] = rand() % 256;
    }

    /* print the actual plaintext message */
    printf("plain text : \n");
    printf("%s \n", ut_plaintext);

    /* pal_crypt_encrypt_aes128_ccm check */
    ut_pal_status = pal_crypt_encrypt_aes128_ccm(
        NULL,
        (const unsigned char *)ut_plaintext,
        ut_plaintext_len,
        ut_encrypt_key,
        ut_nonce,
        NONCE_SIZE,
        (const unsigned char *)ut_associated_data,
        ut_associated_data_len,
        IFX_PRL_MAC_SIZE,
        ut_ciphertext
    );

    assert(ut_pal_status == PAL_STATUS_SUCCESS);

    /* print the encrypted message */
    printf("cipher text : \n");
    print_hex(ut_ciphertext, sizeof(ut_ciphertext));

    /* pal_crypt_decrypt_aes128_ccm check */
    ut_pal_status = pal_crypt_decrypt_aes128_ccm(
        NULL,
        ut_ciphertext,
        sizeof(ut_ciphertext),
        ut_encrypt_key,
        ut_nonce,
        NONCE_SIZE,
        (const unsigned char *)ut_associated_data,
        ut_associated_data_len,
        IFX_PRL_MAC_SIZE,
        ut_plaintext_decrypted
    );

    assert(ut_pal_status == PAL_STATUS_SUCCESS);
    /* Check that the decrypted message is the same as the encrypted one */
    assert(strcmp((char *)ut_plaintext_decrypted, (char *)ut_plaintext) == 0);

    /* print the decrypted message */
    printf("plain text decrypted : \n");
    printf("%s \n", ut_plaintext_decrypted);
}

int main(int argc, char **argv) {
    // to remove warning for unused parameter
    (void)(argc);
    (void)(argv);

    /* pal_crypt_version unit tests function */
    pal_crypt_version_unit_test();

    /* pal_crypt_tls_prf_sha256 unit tests function */
    pal_crypt_tls_prf_sha256_unit_test();

    /* pal_crypt_encrypt_aes128_ccm unit tests function */
    pal_crypt_encrypt_decrypt_aes128_ccm_unit_test();

    return 0;
}