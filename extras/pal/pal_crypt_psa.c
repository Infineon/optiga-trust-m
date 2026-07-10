/**
 * SPDX-FileCopyrightText: 2021-2026 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_crypt_psa.c
 *
 * \brief   This file implements the platform abstraction layer APIs for
 *          cryptographic functions using the PSA Crypto API (Mbed TLS 4.x /
 *          TF-PSA-Crypto 1.x).
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include <mbedtls/version.h>
#include <psa/crypto.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "optiga_lib_common.h"
#include "pal_crypt.h"
#include "pal_os_memory.h"

#define PAL_CRYPT_MAX_LABEL_SEED_LENGTH (96U)
#define PAL_CRYPT_AES128_KEY_BYTES (16U)

/* PSA crypto initialization is performed exactly once across all threads. */
static pthread_once_t pal_psa_init_once_ctl = PTHREAD_ONCE_INIT;
static psa_status_t pal_psa_init_status = PSA_ERROR_BAD_STATE;

static void pal_psa_do_init(void) {
    pal_psa_init_status = psa_crypto_init();
}

static psa_status_t pal_psa_init_once(void) {
    (void)pthread_once(&pal_psa_init_once_ctl, pal_psa_do_init);
    return pal_psa_init_status;
}

// lint --e{818, 715, 830} suppress "argument \"p_pal_crypt\" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_tls_prf_sha256(
    pal_crypt_t *p_pal_crypt,
    const uint8_t *p_secret,
    uint16_t secret_length,
    const uint8_t *p_label,
    uint16_t label_length,
    const uint8_t *p_seed,
    uint16_t seed_length,
    uint8_t *p_derived_key,
    uint16_t derived_key_length
) {
    (void)p_pal_crypt;

    pal_status_t return_value = PAL_STATUS_FAILURE;
    psa_status_t st;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256);
    psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;
    do {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (p_secret == NULL || p_label == NULL || p_seed == NULL || p_derived_key == NULL) {
            break;
        }
#endif  // OPTIGA_LIB_DEBUG_NULL_CHECK

        if (pal_psa_init_once() != PSA_SUCCESS) {
            break;
        }

        if ((uint32_t)label_length + (uint32_t)seed_length > PAL_CRYPT_MAX_LABEL_SEED_LENGTH) {
            break;
        }
        /* Import the key */
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&attr, alg);
        psa_set_key_type(&attr, PSA_KEY_TYPE_DERIVE);
        st = psa_import_key(&attr, p_secret, (size_t)secret_length, &key_id);
        psa_reset_key_attributes(&attr);
        if (st != PSA_SUCCESS) {
            break;
        }

        /* Derive the key */
        st = psa_key_derivation_setup(&operation, alg);
        if (st != PSA_SUCCESS) {
            break;
        }
        st = psa_key_derivation_set_capacity(&operation, derived_key_length);
        if (st != PSA_SUCCESS) {
            break;
        }
        st = psa_key_derivation_input_bytes(
            &operation,
            PSA_KEY_DERIVATION_INPUT_SEED,
            p_seed,
            seed_length
        );
        if (st != PSA_SUCCESS) {
            break;
        }
        st = psa_key_derivation_input_key(&operation, PSA_KEY_DERIVATION_INPUT_SECRET, key_id);
        if (st != PSA_SUCCESS) {
            break;
        }
        st = psa_key_derivation_input_bytes(
            &operation,
            PSA_KEY_DERIVATION_INPUT_LABEL,
            p_label,
            label_length
        );
        if (st != PSA_SUCCESS) {
            break;
        }
        st = psa_key_derivation_output_bytes(&operation, p_derived_key, derived_key_length);
        if (st != PSA_SUCCESS) {
            break;
        }
        return_value = PAL_STATUS_SUCCESS;

    } while (FALSE);

    /* Clean up */
    psa_key_derivation_abort(&operation);
    if (key_id != 0) {
        (void)psa_destroy_key(key_id);
    }

    return return_value;
}

// lint --e{818, 715, 830} suppress "argument \"p_pal_crypt\" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_encrypt_aes128_ccm(
    pal_crypt_t *p_pal_crypt,
    const uint8_t *p_plain_text,
    uint16_t plain_text_length,
    const uint8_t *p_encrypt_key,
    const uint8_t *p_nonce,
    uint16_t nonce_length,
    const uint8_t *p_associated_data,
    uint16_t associated_data_length,
    uint8_t mac_size,
    uint8_t *p_cipher_text
) {
    (void)p_pal_crypt;

    pal_status_t return_value = PAL_STATUS_FAILURE;
    psa_status_t st;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    size_t out_len = 0;

    do {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (p_plain_text == NULL || p_encrypt_key == NULL || p_nonce == NULL
            || p_associated_data == NULL || p_cipher_text == NULL) {
            break;
        }
#endif  // OPTIGA_LIB_DEBUG_NULL_CHECK

        if (pal_psa_init_once() != PSA_SUCCESS) {
            break;
        }

        psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attr, PAL_CRYPT_AES128_KEY_BYTES * 8U);
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT);
        psa_set_key_algorithm(&attr, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mac_size));

        st = psa_import_key(&attr, p_encrypt_key, PAL_CRYPT_AES128_KEY_BYTES, &key_id);
        psa_reset_key_attributes(&attr);
        if (st != PSA_SUCCESS) {
            break;
        }

        /* Output layout expected by the caller: ciphertext || tag */
        st = psa_aead_encrypt(
            key_id,
            PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mac_size),
            p_nonce,
            nonce_length,
            p_associated_data,
            associated_data_length,
            p_plain_text,
            plain_text_length,
            p_cipher_text,
            (size_t)plain_text_length + mac_size,
            &out_len
        );
        if (st == PSA_SUCCESS && out_len == (size_t)plain_text_length + mac_size) {
            return_value = PAL_STATUS_SUCCESS;
        }
    } while (FALSE);
    if (key_id != 0) {
        (void)psa_destroy_key(key_id);
    }

    return return_value;
}

// lint --e{818, 715, 830} suppress "argument \"p_pal_crypt\" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_decrypt_aes128_ccm(
    pal_crypt_t *p_pal_crypt,
    const uint8_t *p_cipher_text,
    uint16_t cipher_text_length,
    const uint8_t *p_decrypt_key,
    const uint8_t *p_nonce,
    uint16_t nonce_length,
    const uint8_t *p_associated_data,
    uint16_t associated_data_length,
    uint8_t mac_size,
    uint8_t *p_plain_text
) {
    (void)p_pal_crypt;

    pal_status_t return_value = PAL_STATUS_FAILURE;
    psa_status_t st;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    size_t out_len = 0;

    do {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if (p_cipher_text == NULL || p_decrypt_key == NULL || p_nonce == NULL
            || p_associated_data == NULL || p_plain_text == NULL) {
            break;
        }
#endif  // OPTIGA_LIB_DEBUG_NULL_CHECK

        if (cipher_text_length < mac_size) {
            break;
        }

        if (pal_psa_init_once() != PSA_SUCCESS) {
            break;
        }

        psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attr, PAL_CRYPT_AES128_KEY_BYTES * 8U);
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DECRYPT);
        psa_set_key_algorithm(&attr, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mac_size));

        st = psa_import_key(&attr, p_decrypt_key, PAL_CRYPT_AES128_KEY_BYTES, &key_id);
        psa_reset_key_attributes(&attr);
        if (st != PSA_SUCCESS) {
            break;
        }

        st = psa_aead_decrypt(
            key_id,
            PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mac_size),
            p_nonce,
            nonce_length,
            p_associated_data,
            associated_data_length,
            p_cipher_text,
            cipher_text_length,
            p_plain_text,
            (size_t)cipher_text_length - mac_size,
            &out_len
        );
        if (st == PSA_SUCCESS && out_len == (size_t)cipher_text_length - mac_size) {
            return_value = PAL_STATUS_SUCCESS;
        }
    } while (FALSE);

    if (key_id != 0) {
        (void)psa_destroy_key(key_id);
    }

    return return_value;
}

pal_status_t pal_crypt_version(uint8_t *p_crypt_lib_version_info, uint16_t *length) {
    const char *v = MBEDTLS_VERSION_STRING;
    size_t vlen;

#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
    if (p_crypt_lib_version_info == NULL || length == NULL) {
        return PAL_STATUS_INVALID_INPUT;
    }
#endif  // OPTIGA_LIB_DEBUG_NULL_CHECK

    vlen = strlen(v);
    if (vlen > *length) {
        return PAL_STATUS_FAILURE;
    }

    pal_os_memcpy(p_crypt_lib_version_info, v, vlen);
    *length = (uint16_t)vlen;

    return PAL_STATUS_SUCCESS;
}

/**
 * @}
 */
