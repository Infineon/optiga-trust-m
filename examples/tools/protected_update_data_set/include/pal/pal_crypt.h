/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file pal_crypt.h
*
* \brief   This file defines APIs, types and data structures used for protected update pal crypt.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/


#ifndef _PROTECTED_UPDATE_PAL_CRYPTO_
#define _PROTECTED_UPDATE_PAL_CRYPTO_

#include <stdint.h>
#include "protected_update_data_set.h"

typedef struct pal_crypt
{
    /// callback
    void * callback_ctx;
}pal_crypt_t;

/**
 * @brief PAL return status.
 */
typedef uint16_t pal_status_t;

// Generates hash
pal_status_t pal_crypt_hash(pal_crypt_t* p_pal_crypt,
                            uint8_t hash_algorithm,
                            const uint8_t * p_message,
                            uint32_t message_length,
                            uint8_t * p_digest);

// Calculates signature
pal_status_t pal_crypt_sign(pal_crypt_t* p_pal_crypt,
                            uint8_t * p_digest, 
                            uint16_t digest_length,
                            uint8_t * p_signature,
                            uint16_t * signature_length,
                            const uint8_t * p_private_key,
                            uint16_t private_key_length);

// Gets the signature length based on private key
pal_status_t pal_crypt_get_signature_length( uint8_t * p_private_key,
                                            uint16_t * sign_len,
                                            signature_algo_t sign_algo);

// Generate encrypted data
pal_status_t pal_crypt_encrypt_aes128_ccm(  pal_crypt_t* p_pal_crypt,
                                            const uint8_t * p_plain_text,
                                            uint16_t plain_text_length,
                                            const uint8_t * p_encrypt_key,
                                            const uint8_t * p_nonce,
                                            uint16_t nonce_length,
                                            const uint8_t * p_associated_data,
                                            uint16_t associated_data_length,
                                            uint8_t mac_size,
                                            uint8_t * p_cipher_text);
// Derive key
pal_status_t pal_crypt_tls_prf_sha256(  pal_crypt_t* p_pal_crypt,
                                        const uint8_t * p_secret,
                                        uint16_t secret_length,
                                        const uint8_t * p_label,
                                        uint16_t label_length,
                                        const uint8_t * p_seed,
                                        uint16_t seed_length,
                                        uint8_t * p_derived_key,
                                        uint16_t derived_key_length);


// Generate random
pal_status_t pal_crypt_generate_random(pal_crypt_t* p_pal_crypt,
                                       uint8_t * p_random_data ,
                                       uint16_t random_data_length );

// Set seed
pal_status_t pal_crypt_set_seed(pal_crypt_t* p_pal_crypt,
                                uint8_t * p_seed,
                                uint16_t seed_length);

pal_status_t pal_crypt_get_key_type(int8_t * key_file);

pal_status_t pal_crypt_parse_ecc_key(void * key_file,
                                    uint8_t ** D, uint16_t * D_length,
                                    uint8_t ** X, uint16_t * X_length,
                                    uint8_t ** Y, uint16_t * Y_length);

pal_status_t pal_crypt_parse_rsa_key(void * key_file,
                                    uint8_t ** N, uint16_t * N_length,
                                    uint8_t ** E, uint16_t * E_length,
                                    uint8_t ** D, uint16_t * D_length);

#endif //_PROTECTED_UPDATE_PAL_CRYPTO_
/**
* @}
*/