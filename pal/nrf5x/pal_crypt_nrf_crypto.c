/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
*
* \file pal_crypt_nrf_crypto.c
*
* \brief   This file implements the platform abstraction layer APIs for cryptographic functions using nrf_crypto.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_crypt.h"
#include "optiga/pal/pal_memory_mgmt.h"
#include "mbedtls/ccm.h"
#include "mbedtls/md.h"
#include "mbedtls/ssl.h"
#include "mbedtls/version.h"
#include "nrf_crypto.h"

#define PAL_CRYPT_MAX_LABEL_SEED_LENGTH     (96U)

pal_status_t pal_crypt_tls_prf_sha256(pal_crypt_t* p_pal_crypt,
                                      const uint8_t * p_secret,
                                      uint16_t secret_length,
                                      const uint8_t * p_label,
                                      uint16_t label_length,
                                      const uint8_t * p_seed,
                                      uint16_t seed_length,
                                      uint8_t * p_derived_key,
                                      uint16_t derived_key_length)
{
    #define PAL_CRYPT_DIGEST_MAX_SIZE    (32U)

    pal_status_t return_value = PAL_STATUS_FAILURE;
    uint8_t message_digest_length = PAL_CRYPT_DIGEST_MAX_SIZE;
    uint16_t derive_key_len_index;
    uint16_t hmac_checksum_result_index;
    uint16_t hmac_result_length;
    uint8_t md_hmac_temp_array[PAL_CRYPT_MAX_LABEL_SEED_LENGTH + PAL_CRYPT_DIGEST_MAX_SIZE];
    uint8_t hmac_checksum_result[PAL_CRYPT_DIGEST_MAX_SIZE];
    //const mbedtls_md_info_t *message_digest_info;
    //mbedtls_md_context_t message_digest_context;
    uint16_t final_seed_length = 0;
    nrf_crypto_hmac_context_t hmac_ctx;
    
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_secret) || (NULL == p_label) || (NULL == p_seed) || (NULL == p_derived_key))
        {
            break;
        }
#endif  //OPTIGA_LIB_DEBUG_NULL_CHECK

        if (sizeof(md_hmac_temp_array ) < (uint32_t)(message_digest_length + label_length + seed_length))
        {
            return_value = PAL_STATUS_INVALID_INPUT;
            break;
        }

        /*
        message_digest_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

        mbedtls_md_init(&message_digest_context);
        */

        memcpy(md_hmac_temp_array + message_digest_length, p_label, label_length);
        memcpy(md_hmac_temp_array + message_digest_length + label_length, p_seed, seed_length);
        final_seed_length = label_length + seed_length;

        /*
        if (0 != (mbedtls_md_setup(&message_digest_context,message_digest_info,1)))
        {
            return_value = PAL_STATUS_INVALID_INPUT;
            break;
        }
        */

        // SHA256-HMAC
        // feed secret key
        // mbedtls_md_hmac_starts(&message_digest_context, p_secret, secret_length);
        nrf_crypto_hmac_init(&hmac_ctx, &g_nrf_crypto_hmac_sha256_info, p_secret, secret_length);

        // feed message digest
        // mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array + message_digest_length, final_seed_length);
        nrf_crypto_hmac_update(&hmac_ctx, md_hmac_temp_array + message_digest_length, final_seed_length);

        // finish HMAC
        // mbedtls_md_hmac_finish(&message_digest_context, md_hmac_temp_array);
        size_t digest_len = PAL_CRYPT_DIGEST_MAX_SIZE;
        nrf_crypto_hmac_finalize(&hmac_ctx, md_hmac_temp_array, &digest_len);

        for (derive_key_len_index = 0; derive_key_len_index < derived_key_length; 
             derive_key_len_index += message_digest_length)
        {
            // reset HMAC context, re-use key
            // mbedtls_md_hmac_reset(&message_digest_context);
            nrf_crypto_hmac_init(&hmac_ctx, &g_nrf_crypto_hmac_sha256_info, p_secret, secret_length);

            // feed message
            // mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array, 
            //                         message_digest_length + final_seed_length);
            nrf_crypto_hmac_update(&hmac_ctx, md_hmac_temp_array, message_digest_length + final_seed_length);

            // finish
            // mbedtls_md_hmac_finish(&message_digest_context, hmac_checksum_result);
            digest_len = PAL_CRYPT_DIGEST_MAX_SIZE;
            nrf_crypto_hmac_finalize(&hmac_ctx, md_hmac_temp_array, &digest_len);

            // reset HMAC context, re-use key
            // mbedtls_md_hmac_reset(&message_digest_context);
            nrf_crypto_hmac_init(&hmac_ctx, &g_nrf_crypto_hmac_sha256_info, p_secret, secret_length);

            // feed message
            // mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array, message_digest_length);
            nrf_crypto_hmac_update(&hmac_ctx, md_hmac_temp_array, message_digest_length);

            // finish
            // mbedtls_md_hmac_finish(&message_digest_context, md_hmac_temp_array);
            digest_len = PAL_CRYPT_DIGEST_MAX_SIZE;
            nrf_crypto_hmac_finalize(&hmac_ctx, md_hmac_temp_array, &digest_len);

            hmac_result_length = ((derive_key_len_index + message_digest_length) > derived_key_length) ? 
                                  (derived_key_length % message_digest_length) : (message_digest_length);

            for (hmac_checksum_result_index = 0; hmac_checksum_result_index < hmac_result_length; 
                 hmac_checksum_result_index++)
            {
                p_derived_key[derive_key_len_index + hmac_checksum_result_index] = 
                                                                    hmac_checksum_result[hmac_checksum_result_index];
            }
        }

        //mbedtls_md_free(&message_digest_context);

        memset(md_hmac_temp_array, 0x00, sizeof(md_hmac_temp_array));
        memset(hmac_checksum_result, 0x00, sizeof(hmac_checksum_result));

        return_value = PAL_STATUS_SUCCESS;
    } while (FALSE);
    #undef PAL_CRYPT_DIGEST_MAX_SIZE
    return return_value;
}

pal_status_t pal_crypt_encrypt_aes128_ccm(pal_crypt_t* p_pal_crypt,
                                          const uint8_t * p_plain_text,
                                          uint16_t plain_text_length,
                                          const uint8_t * p_encrypt_key,
                                          const uint8_t * p_nonce,
                                          uint16_t nonce_length,
                                          const uint8_t * p_associated_data,
                                          uint16_t associated_data_length,
                                          uint8_t mac_size,
                                          uint8_t * p_cipher_text)
{
    #define AES128_KEY_BITS_SIZE    (16U)
    #define MAC_TAG_BUFFER_SIZE     (16U)
    
    pal_status_t return_status = PAL_STATUS_FAILURE;
    uint8_t mac_output[MAC_TAG_BUFFER_SIZE];
    //mbedtls_ccm_context sEncrypt;

    //mbedtls_ccm_init(&sEncrypt);

    nrf_crypto_aead_context_t aead_ctx;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_cipher_text) || (NULL == p_plain_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_encrypt_key))
        {
            break;
        }
#endif

        // set key
        /*
        if (0 != mbedtls_ccm_setkey(&sEncrypt, MBEDTLS_CIPHER_ID_AES, p_encrypt_key, 8 * AES128_KEY_BITS_SIZE))
        {
            break;
        }
        */

        nrf_crypto_aead_init(&aead_ctx, &g_nrf_crypto_aes_ccm_256_info, p_encrypt_key);
        
        nrf_crypto_aead_crypt(&aead_ctx, NRF_CRYPTO_ENCRYPT,
                              p_nonce,
                              nonce_length,
                              p_associated_data,
                              associated_data_length,
                              p_plain_text,
                              plain_text_length,
                              p_cipher_text,
                              mac_output,
                              mac_size);

        /*
        if (0 != mbedtls_ccm_encrypt_and_tag(&sEncrypt,
                                              plain_text_length,
                                              p_nonce,
                                              nonce_length,
                                              p_associated_data,
                                              associated_data_length,
                                              p_plain_text,
                                              p_cipher_text,
                                              mac_output,
                                              mac_size))
        
        {
            break;
        }
        */

        memcpy((p_cipher_text + plain_text_length), mac_output, mac_size);
        return_status = PAL_STATUS_SUCCESS;
    } while (FALSE);
    //mbedtls_ccm_free(&sEncrypt);
    #undef AES128_KEY_BITS_SIZE
    #undef MAC_TAG_BUFFER_SIZE    
    return return_status;
}

pal_status_t pal_crypt_decrypt_aes128_ccm(pal_crypt_t* p_pal_crypt,
                                          const uint8_t * p_cipher_text,
                                          uint16_t cipher_text_length,
                                          const uint8_t * p_decrypt_key,
                                          const uint8_t * p_nonce,
                                          uint16_t nonce_length,
                                          const uint8_t * p_associated_data,
                                          uint16_t associated_data_length,
                                          uint8_t mac_size,
                                          uint8_t * p_plain_text)
{
    #define AES128_KEY_BITS_SIZE    (16U)
    pal_status_t return_status = PAL_STATUS_FAILURE;
    nrf_crypto_aead_context_t aead_ctx;
    /*
    mbedtls_ccm_context sDecrypt;
    mbedtls_ccm_init(&sDecrypt);
*/
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_plain_text) || (NULL == p_cipher_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_decrypt_key))
        {
            break;
        }
#endif

/*
        if (0 != mbedtls_ccm_setkey(&sDecrypt, MBEDTLS_CIPHER_ID_AES, p_decrypt_key, 8 * AES128_KEY_BITS_SIZE))
        {
            break;
        }
        */

        nrf_crypto_aead_init(&aead_ctx, &g_nrf_crypto_aes_ccm_256_info, p_decrypt_key);
        
       
/*
        if (0 != mbedtls_ccm_auth_decrypt(&sDecrypt,
                                          (cipher_text_length - mac_size),
                                          p_nonce,
                                          nonce_length,
                                          p_associated_data,
                                          associated_data_length,
                                          p_cipher_text,
                                          p_plain_text, 
                                          &p_cipher_text[cipher_text_length - mac_size],
                                          mac_size))
        {
            break;
        }
        */

        nrf_crypto_aead_crypt(&aead_ctx, NRF_CRYPTO_DECRYPT,
                              p_nonce,
                              nonce_length,
                              p_associated_data,
                              associated_data_length,
                              p_cipher_text,
                              (cipher_text_length - mac_size),
                              p_plain_text,
                              &p_cipher_text[cipher_text_length - mac_size],
                              mac_size);
        return_status = PAL_STATUS_SUCCESS;
    } while (FALSE);
    //mbedtls_ccm_free(&sDecrypt);
    #undef AES128_KEY_BITS_SIZE
    return return_status;
}

pal_status_t pal_crypt_version(uint8_t * p_crypt_lib_version_info, uint16_t * length)
{
    pal_status_t return_value  = PAL_STATUS_FAILURE;    
    uint8_t sizeof_version_number  = (uint8_t)strlen(MBEDTLS_VERSION_STRING);

    do
    {
        if (sizeof_version_number > *length)
        {
            break;
        }

        pal_os_memcpy(p_crypt_lib_version_info, MBEDTLS_VERSION_STRING, sizeof_version_number);
        *length = sizeof_version_number;

        return_value = PAL_STATUS_SUCCESS;

    } while (0);
    return return_value;
}

/**
* @}
*/
