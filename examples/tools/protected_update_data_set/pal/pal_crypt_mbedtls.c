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
* \file pal_crypt_mbedtls.c
*
* \brief   This file implements APIs, types and data structures used for protected update pal crypt.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls\sha256.h"
#include "mbedtls\rsa.h"
#include "mbedtls\pk.h"
#include "mbedtls\entropy.h"
#include "mbedtls\ctr_drbg.h"
#include "pal\pal_logger.h"
#include "mbedtls\ccm.h"
#include "mbedtls\ssl.h"
#include "pal\pal_crypt.h"
#include "protected_update_data_set.h"
#include "pal\pal_os_memory.h"
#include "pal\pal_logger.h"

static uint16_t pal_crypt_calculate_sha256_hash(    const uint8_t * message,
                                            uint16_t message_len,
                                            uint8_t * digest)
{
    uint16_t status = 1;
    mbedtls_sha256_context sha256_ctx;
    do
    {
        mbedtls_sha256_init(&sha256_ctx);

        if (0 != mbedtls_sha256_starts_ret(&sha256_ctx, 0))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_starts_ret\n");
            break;
        }
        if (0 != mbedtls_sha256_update_ret(&sha256_ctx, message, message_len))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_update_ret\n");
            break;
        }

        if (0 != mbedtls_sha256_finish_ret(&sha256_ctx, digest))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_sha256_finish_ret\n");
            break;
        }
        mbedtls_sha256_free(&sha256_ctx);
        status = 0;
    } while (0);
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_hash( pal_crypt_t* p_pal_crypt,
                    uint8_t hash_algorithm,
                    const uint8_t * p_message,
                    uint32_t message_length,
                    uint8_t * p_digest)
{
    pal_status_t status = 1;

    if((uint8_t)eSHA_256 == hash_algorithm) // SHA-256
    {
        status = pal_crypt_calculate_sha256_hash(p_message, (uint16_t)message_length, p_digest);
    }

    return status;
}

pal_status_t pal_crypt_get_signature_length(uint8_t * p_private_key, uint16_t * sign_len, signature_algo_t sign_algo)
{
    pal_status_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_ecp_keypair * ecc_ctx;
    mbedtls_rsa_context * rsa_ctx;
    mbedtls_pk_type_t key_type;
    
    mbedtls_pk_init(&ctx);
    do
    {
        if (0 != mbedtls_pk_parse_keyfile(&ctx, (const int8_t*)p_private_key, ""))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_pk_parse_keyfile\n");
            break;
        }

        key_type = mbedtls_pk_get_type(&ctx);
        if(eES_SHA == sign_algo)
        {
            if(MBEDTLS_PK_ECKEY == key_type)
            {
                ecc_ctx = (mbedtls_ecp_keypair *)ctx.pk_ctx;
                // Signature length is (n/8)*2 hence to get the length of R & S component, 8/2 = 4
                // Mod operation is performed to get non divisible key size in case of secp521r1.
                *sign_len = (uint16_t)((ecc_ctx->grp.nbits)/4 + ((ecc_ctx->grp.nbits)%4?2:0)); 
            }
            else
            {
                pal_logger_print_message(" Error : Key type not mismatch\n");
                break;
            }
        }
        else if(eRSA_SSA_PKCS1_V1_5_SHA_256 == sign_algo)
        {
            if(MBEDTLS_PK_RSA == key_type)
            {
                rsa_ctx = (mbedtls_rsa_context *)ctx.pk_ctx;
                // Signature length is (n/8)*2 hence to get the length of R & S component, 8/2 = 4
                // Mod operation is performed to get non divisible key size in case of secp521r1.
                *sign_len = (uint16_t)rsa_ctx->len; 
            }
            else
            {
                pal_logger_print_message(" Error : Key type not mismatch\n");
                break;
            }
        }
        else
        {
            pal_logger_print_message(" Error : Invalid sign algo\n");
            break;
        }
        status = 0;
    }while(0);

    mbedtls_pk_free(&ctx);
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt", "private_key_length" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_sign( pal_crypt_t* p_pal_crypt,
                    uint8_t * p_digest, 
                    uint16_t digest_length,
                    uint8_t * p_signature,
                    uint16_t * signature_length,
                    const uint8_t * p_private_key,
                    uint16_t private_key_length)
{
    pal_status_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    uint8_t hash[32];
    const int8_t *pers = (const int8_t *)"mbedtls_pk_sign";
    size_t signature_buff_len;

    do
    {
            mbedtls_entropy_init(&entropy);
            mbedtls_ctr_drbg_init(&ctr_drbg);
            mbedtls_pk_init(&ctx);

            if(0 != mbedtls_ctr_drbg_seed(  &ctr_drbg, mbedtls_entropy_func, 
                                            &entropy, 
                                            (const uint8_t *)pers, 
                                            strlen(pers)))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_ctr_drbg_seed\n");
                break;
            }
            //Read key into an array
            if (0 != mbedtls_pk_parse_keyfile(&ctx, (const int8_t*)p_private_key, ""))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_pk_parse_keyfile\n");
                break;
            }

            // generate hash
            if (0 != pal_crypt_hash(NULL, (uint8_t)eSHA_256, p_digest, digest_length, hash))
            {
                pal_logger_print_message(" Error : Failed in pal_crypt_hash\n");
                break;
            }

            if (0 != mbedtls_pk_sign(&ctx, MBEDTLS_MD_SHA256, hash, 0, p_signature, &signature_buff_len,
                mbedtls_ctr_drbg_random, &ctr_drbg))
            {
                pal_logger_print_message(" Error : Failed in mbedtls_pk_sign\n");
                break;
            }
            *signature_length = (uint16_t)signature_buff_len;
            mbedtls_pk_free(&ctx);
            mbedtls_ctr_drbg_free(&ctr_drbg);
            mbedtls_entropy_free(&entropy);
            status = 0;
    } while (0);
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
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
    
    pal_status_t status = 1;
    uint8_t mac_output[MAC_TAG_BUFFER_SIZE];
    mbedtls_ccm_context encrypt;

    mbedtls_ccm_init(&encrypt);

    do
    {

        if (0 != mbedtls_ccm_setkey(&encrypt, MBEDTLS_CIPHER_ID_AES, p_encrypt_key, 8 * AES128_KEY_BITS_SIZE))
        {
            break;
        }
        
        if (0 != mbedtls_ccm_encrypt_and_tag(&encrypt,
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

        memcpy((p_cipher_text + plain_text_length), mac_output, mac_size);
        status = 0;
    } while (0);
    mbedtls_ccm_free(&encrypt);
    #undef AES128_KEY_BITS_SIZE
    #undef MAC_TAG_BUFFER_SIZE    
    return status;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
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
    #define PAL_CRYPT_MAX_LABEL_SEED_LENGTH     (96U)

    pal_status_t return_value = 1;
    uint8_t message_digest_length = PAL_CRYPT_DIGEST_MAX_SIZE;
    uint16_t derive_key_len_index, hmac_checksum_result_index;
    uint16_t hmac_result_length;
    uint8_t md_hmac_temp_array[PAL_CRYPT_MAX_LABEL_SEED_LENGTH + PAL_CRYPT_DIGEST_MAX_SIZE];
    uint8_t hmac_checksum_result[PAL_CRYPT_DIGEST_MAX_SIZE];
    const mbedtls_md_info_t *message_digest_info;
    mbedtls_md_context_t message_digest_context;
    uint16_t final_seed_length = 0;
       
    mbedtls_md_init(&message_digest_context);
    
    do
    {
        if (sizeof(md_hmac_temp_array ) < (uint32_t)(message_digest_length + label_length + seed_length))
        {
            //return_value = PAL_STATUS_INVALID_INPUT;
            break;
        }

        message_digest_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

        memcpy(md_hmac_temp_array + message_digest_length, p_label, label_length);
        memcpy(md_hmac_temp_array + message_digest_length + label_length, p_seed, seed_length);
        final_seed_length = label_length + seed_length;

        if (0 != (mbedtls_md_setup(&message_digest_context,message_digest_info,1)))
        {
            //return_value = PAL_STATUS_INVALID_INPUT;
            break;
        }

        if (0 != mbedtls_md_hmac_starts(&message_digest_context, p_secret, secret_length))
        {
            break;
        }
       
        if (0 != mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array + message_digest_length, final_seed_length))
        {
            break;
        }
        
        if (0 != mbedtls_md_hmac_finish(&message_digest_context, md_hmac_temp_array))
        {
            break;
        }

        for (derive_key_len_index = 0; derive_key_len_index < derived_key_length; 
             derive_key_len_index += message_digest_length)
        {
            if (0 != mbedtls_md_hmac_reset(&message_digest_context))
            {
                break;
            }
            if (0 != mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array, 
                            message_digest_length + final_seed_length))
            {
                break;                
            }
            if (0 != mbedtls_md_hmac_finish(&message_digest_context, hmac_checksum_result))
            {
                break;                                
            }

            if (0 != mbedtls_md_hmac_reset(&message_digest_context))
            {
                break;                
            }
            if (0 != mbedtls_md_hmac_update(&message_digest_context, md_hmac_temp_array, message_digest_length))
            {
                break;                                
            }
            if (0 != mbedtls_md_hmac_finish(&message_digest_context, md_hmac_temp_array))
            {
                break;                                
            }

            hmac_result_length = ((derive_key_len_index + message_digest_length) > derived_key_length) ? 
                                  (derived_key_length % message_digest_length) : (message_digest_length);

            for (hmac_checksum_result_index = 0; hmac_checksum_result_index < hmac_result_length; 
                 hmac_checksum_result_index++)
            {
                p_derived_key[derive_key_len_index + hmac_checksum_result_index] = hmac_checksum_result[hmac_checksum_result_index];
            }
        }
        if (derive_key_len_index >= derived_key_length)
        {
            return_value = 0;
        }
    } while (FALSE);
    
    mbedtls_md_free(&message_digest_context);

    memset(md_hmac_temp_array, 0x00, sizeof(md_hmac_temp_array));
    memset(hmac_checksum_result, 0x00, sizeof(hmac_checksum_result));    
    #undef PAL_CRYPT_DIGEST_MAX_SIZE
    #undef PAL_CRYPT_MAX_LABEL_SEED_LENGTH
    return return_value;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_generate_random( pal_crypt_t* p_pal_crypt,
                               uint8_t * p_random_data ,
                               uint16_t random_data_length )
{
    pal_status_t return_value = 1;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    const int8_t *pers = (const int8_t *)"mbedtls_pk_sign";


    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    do
    {
        if(0 != mbedtls_ctr_drbg_seed(  &ctr_drbg, mbedtls_entropy_func, 
                                        &entropy, 
                                        (const uint8_t *)pers, 
                                        strlen(pers)))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_ctr_drbg_seed\n");
            break;
        }
        if (0 != mbedtls_ctr_drbg_random(&ctr_drbg, p_random_data, random_data_length))
        {
            pal_logger_print_message(" Error : Failed in mbedtls_pk_parse_keyfile\n");
            break;
        }        
        return_value = 0;
    } while (FALSE);

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    return return_value;
}

//lint --e{715} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
pal_status_t pal_crypt_set_seed(pal_crypt_t* p_pal_crypt,
                       uint8_t * p_seed,
                       uint16_t seed_length)
{
    return(pal_crypt_generate_random(p_pal_crypt, p_seed, seed_length));
}

pal_status_t pal_crypt_get_key_type(int8_t * key_file)
{
    pal_status_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_pk_type_t key_type;
    int8_t buffer[200];

    mbedtls_pk_init(&ctx);

    do
    {
        if (0 != mbedtls_pk_parse_keyfile(&ctx, key_file, ""))
        {
            sprintf(buffer, "Error : Parsing of the key file");
            pal_logger_print_message(buffer);
            break;
        }
        key_type = mbedtls_pk_get_type(&ctx);
        switch (key_type)
        {
        case MBEDTLS_PK_RSA:
            status = (uint16_t)eRSA;
            break;
        case MBEDTLS_PK_ECKEY:
            status = (uint16_t)eECC;
            break;
        case MBEDTLS_PK_NONE:
        case MBEDTLS_PK_ECKEY_DH:
        case MBEDTLS_PK_ECDSA:
        case MBEDTLS_PK_RSA_ALT:
        case MBEDTLS_PK_RSASSA_PSS:
        default :
            break;
        }
    } while (0);

    mbedtls_pk_free(&ctx);
    return status;
}

static inline void pal_crypt_set_buffer(uint8_t ** dest, uint16_t * dest_len, uint16_t len)
{
    void * x = pal_os_malloc(len);
    *dest = x;
    *dest_len = len;
}

pal_status_t pal_crypt_parse_ecc_key(void * key_file,
    uint8_t ** D, uint16_t * D_length,
    uint8_t ** X, uint16_t * X_length,
    uint8_t ** Y, uint16_t * Y_length)
{
    int32_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_ecp_keypair * ecc_ctx;
    int8_t buffer[200];

    mbedtls_pk_init(&ctx);
    
    do
    {
        if (0 != mbedtls_pk_parse_keyfile(&ctx, key_file, ""))
        {
            sprintf(buffer, "Error : Parsing of the ecc key file");
            pal_logger_print_message(buffer);
        }
        ecc_ctx = (mbedtls_ecp_keypair *)ctx.pk_ctx;

        // Check for curve which is not divisible by 8
        pal_crypt_set_buffer(D, D_length, (uint16_t)(((ecc_ctx->grp.nbits)/8) + ((ecc_ctx->grp.nbits)%8?1:0)));
        pal_crypt_set_buffer(X, X_length, (uint16_t)(((ecc_ctx->grp.nbits)/8) + ((ecc_ctx->grp.nbits)%8?1:0)));
        pal_crypt_set_buffer(Y, Y_length, (uint16_t)(((ecc_ctx->grp.nbits)/8) + ((ecc_ctx->grp.nbits)%8?1:0)));

        status = mbedtls_mpi_write_binary(&(ecc_ctx->d), *D, *D_length);
        status |= mbedtls_mpi_write_binary(&(ecc_ctx->Q.X), *X, *X_length);
        status |= mbedtls_mpi_write_binary(&(ecc_ctx->Q.Y), *Y, *Y_length);

    } while (0);
    mbedtls_pk_free(&ctx);
    return (pal_status_t)status;
}


pal_status_t pal_crypt_parse_rsa_key(void * key_file,
    uint8_t ** N, uint16_t * N_length,
    uint8_t ** E, uint16_t * E_length,
    uint8_t ** D, uint16_t * D_length)
{
    int32_t status = 1;
    mbedtls_pk_context ctx;
    mbedtls_rsa_context * rsa_ctx;
    int8_t buffer[200];

    mbedtls_pk_init(&ctx);

    do
    {
        if (0 != mbedtls_pk_parse_keyfile(&ctx, key_file, ""))
        {
            sprintf(buffer,"Error : Parsing of the rsa key file");
            pal_logger_print_message(buffer);
            break;
        }
        rsa_ctx = (mbedtls_rsa_context *)ctx.pk_ctx;
        pal_crypt_set_buffer(N, N_length, (uint16_t)rsa_ctx->len);
        pal_crypt_set_buffer(E, E_length, 4);
        pal_crypt_set_buffer(D, D_length, (uint16_t)rsa_ctx->len);

        status = mbedtls_mpi_write_binary(&(rsa_ctx->N), *N, *N_length);
        status |= mbedtls_mpi_write_binary(&(rsa_ctx->E), *E, *E_length);
        status |= mbedtls_mpi_write_binary(&(rsa_ctx->D), *D, *D_length);

    } while (0);

    mbedtls_pk_free(&ctx);
    return (pal_status_t)status;
}

/** 
* @}
*/

