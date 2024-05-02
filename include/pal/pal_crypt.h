/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_crypt.h
 *
 * \brief   This file provides the prototype declarations of PAL crypt.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_CRYPT_H_
#define _PAL_CRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_lib_types.h"
#include "pal.h"

/** \brief PAL crypt context structure */
typedef struct pal_crypt_t {
    /// callback
    void *callback_ctx;
} pal_crypt_t;

/**
 * \brief Derives the key using the TLS PRF SHA256 for a given secret.
 *
 * \details
 * Derives the key using the TLS PRF SHA256 for a given secret.
 * - Derives a key using the user provided secret.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in]           p_pal_crypt             Crypt context
 * \param[in]           p_secret                Valid pointer to input shared secret.
 * \param[in]           secret_length           Length of the shared secret.
 * \param[in]           p_label                 Valid pointer to Label for PRF calculation.
 * \param[in]           label_length            Label length.
 * \param[in]           p_seed                  Valid pointer to the buffer which contains seed.
 * \param[in]           seed_length             seed length.
 * \param[in,out]       p_derived_key           Valid pointer to the buffer where PRF output to be saved.
 * \param[in]           derived_key_length      Length of the key to be derived from secret.
 *
 * \retval              PAL_STATUS_SUCCESS      In case of success
 * \retval              PAL_STATUS_FAILURE      In case of failure
 */
LIBRARY_EXPORTS pal_status_t pal_crypt_tls_prf_sha256(
    pal_crypt_t *p_pal_crypt,
    const uint8_t *p_secret,
    uint16_t secret_length,
    const uint8_t *p_label,
    uint16_t label_length,
    const uint8_t *p_seed,
    uint16_t seed_length,
    uint8_t *p_derived_key,
    uint16_t derived_key_length
);

/**
 * \brief Encrypts the input plain text using AES CCM algorithm and provides the cipher text as well as MAC into output buffer.
 *
 * \details
 * Encrypts the input plain text using AES CCM algorithm and provides the cipher text as well as MAC into output buffer.
 * - Encrypts the given plain text using the provided encryption key, nonce and associated data.
 *
 * \pre
 * - None
 *
 * \note
 * - If <b>mac_size</b> is set to 8 AES128_CCM_8 algorithm will be used for encryption.
 * - If <b>mac_size</b> is set to 16 AES128_CCM algorithm will be used for encryption.
 *
 * \param[in]           p_pal_crypt                 Crypt context
 * \param[in]           p_plain_text                Valid pointer to plain text data.
 * \param[in]           plain_text_length           Plain text data size.
 * \param[in]           p_encrypt_key               Valid pointer to Encrypt key .
 * \param[in]           p_nonce                     Valid pointer to Nonce data.
 * \param[in]           nonce_length                Nonce data size.
 * \param[in]           p_associated_data           Valid pointer to Associated data.
 * \param[in]           associated_data_length      Associated data size.
 * \param[in]           mac_size                    Length of expected MAC data.
 * \param[in,out]       p_cipher_text               Valid pointer to store cipher text and MAC output. Buffer length must be at-least <b>plain_text_length</b> + <b>MAC mac_size</b>.
 *
 * \retval              PAL_STATUS_SUCCESS          In case of success
 * \retval              PAL_STATUS_FAILURE          In case of failure
 */
LIBRARY_EXPORTS pal_status_t pal_crypt_encrypt_aes128_ccm(
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
);

/**
 * \brief Decrypts the cipher text using AES CCM algorithm and provides the plain text.
 *
 * \details
 * Decrypts the cipher text using AES CCM algorithm and provides the plain text.
 * - Decrypts the cipher text using the provided decryption key, nonce and associated data
 * - This operation validates the MAC internally and provided the plain text if the MAC is successfully validated.
 *
 * \pre
 * - None
 *
 * \note
 * - If <b>mac_size</b> is set to 8 AES128_CCM_8 algorithm will be used for decryption.
 * - If <b>mac_size</b> is set to 16 AES128_CCM algorithm will be used for decryption.
 *
 * \param[in]           p_pal_crypt                 Crypt context
 * \param[in]           p_cipher_text               Valid pointer to the Cipher text + MAC data.
 * \param[in]           cipher_text_length          Cipher text data size.
 * \param[in]           p_decrypt_key               Valid pointer to decrypt key.
 * \param[in]           p_nonce                     Valid pointer to Nonce data.
 * \param[in]           nonce_length                Nonce size.
 * \param[in]           p_associated_data           Valid pointer to Associated data.
 * \param[in]           associated_data_length      Associated data size.
 * \param[in]           mac_size                    Length of MAC data.
 * \param[in,out]       p_plain_text                Valid pointer to store plain text. Buffer length must be at-least <b>Cipher_text_length</b> - <b>mac_size</b>.
 *
 * \retval              PAL_STATUS_SUCCESS          In case of success
 * \retval              PAL_STATUS_FAILURE          In case of failure
 */
LIBRARY_EXPORTS pal_status_t pal_crypt_decrypt_aes128_ccm(
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
);

/**
 * \brief Gets the external crypto library version number.
 *
 * \details
 * Gets the external crypto library version number. <br>
 *
 * \pre
 * - None
 *
 * \note
 * - Format of version information : vX.Y.Z
 * - mbedTLS Version: vX.Y.Z
 *
 * \param[inout]     p_crypt_lib_version_info               Valid pointer to store the version number.
 * \param[inout]     length                                 version number size.
 *
 * \retval           PAL_STATUS_SUCCESS                     In case of success
 * \retval           PAL_STATUS_FAILURE                     In case of failure
 */
pal_status_t pal_crypt_version(uint8_t *p_crypt_lib_version_info, uint16_t *length);

#ifdef __cplusplus
}
#endif

#endif /*_PAL_CRYPT_H_ */

/**
 * @}
 */
