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
* \file pal_crypt_wolfssl.c
*
* \brief   This file implements the platform abstraction layer(pal) APIs for cryptographic functions.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_crypt.h"
#include "optiga/pal/pal_memory_mgmt.h"

/// @cond hidden
//lint --e{123,617,537} suppress "Suppress ctype.h in Keil + Warning mpi_class.h is both a module and an include file + Repeated include"
#include <wolfssl\wolfcrypt\aes.h>
#include <wolfssl\optiga_wolfssl_tls.h>
/// @endcond

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
    pal_status_t return_value = PAL_STATUS_FAILURE;
    
    (void )p_pal_crypt;
    
    #define SHA256_ALGORITHM            (4U)
    do
    {
    #ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_secret) || (NULL == p_label) || (NULL == p_seed) || (NULL == p_derived_key))
        {
            break;
        }
    #endif
        //Calling PRF function
        if (0 != (int32_t)PRF(p_derived_key,
                              derived_key_length,
                              p_secret,
                              secret_length,
                              p_label,
                              label_length,
                              p_seed,
                              seed_length,
                              TRUE,
                              SHA256_ALGORITHM))

        {
            break;
        }

        return_value = PAL_STATUS_SUCCESS;
    } while (0);
    #undef SHA256_ALGORITHM
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
    pal_status_t return_value = PAL_STATUS_FAILURE;
    Aes encrypt;
    uint8_t mac_output[16];
        
    (void )p_pal_crypt;
    
    #define AES128_KEY_SIZE         (16U)
        
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_cipher_text) || (NULL == p_plain_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_encrypt_key))
        {
            break;
        }
#endif

        if (0 != wc_AesCcmSetKey(&encrypt, p_encrypt_key, AES128_KEY_SIZE))
        {
            break;
        }
        if (0 != wc_AesCcmEncrypt(&encrypt,
                                  p_cipher_text,
                                  p_plain_text,
                                  plain_text_length,
                                  p_nonce,
                                  nonce_length,
                                  mac_output,
                                  mac_size,
                                  p_associated_data,
                                  associated_data_length))

        {
          break;
        }

        pal_os_memcpy((p_cipher_text + plain_text_length), mac_output, mac_size);
        return_value = PAL_STATUS_SUCCESS;
    } while (FALSE);
    #undef AES128_KEY_SIZE
    return return_value;
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
    pal_status_t return_value = PAL_STATUS_FAILURE;
    Aes decrypt;

    (void )p_pal_crypt;
    
    #define AES128_KEY_SIZE     (16U)
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_plain_text) || (NULL == p_cipher_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_decrypt_key))
        {
            break;
        }
#endif
        if (0 != wc_AesCcmSetKey(&decrypt, p_decrypt_key, AES128_KEY_SIZE))
        {
            break;
        }


        if (0 != wc_AesCcmDecrypt(&decrypt,
                                  p_plain_text,
                                  p_cipher_text,
                                  (cipher_text_length - mac_size),
                                  p_nonce,
                                  nonce_length,
                                  &p_cipher_text[cipher_text_length - mac_size],
                                  mac_size,
                                  p_associated_data,
                                  associated_data_length))
        {
            break;
        }

        return_value = PAL_STATUS_SUCCESS;
    } while (FALSE);
    #undef AES128_KEY_SIZE
    return return_value;
}

pal_status_t pal_crypt_version(uint8_t * p_crypt_lib_version_info, uint16_t * length)
{
    pal_status_t return_value  = PAL_STATUS_FAILURE;    
    uint8_t sizeof_version_number  = (uint8_t)strlen(LIBWOLFSSL_VERSION_STRING);
    do
    {        
        if (sizeof_version_number > *length)
        {
            break;
        }

        pal_os_memcpy(p_crypt_lib_version_info, LIBWOLFSSL_VERSION_STRING, sizeof_version_number);
        *length = sizeof_version_number;

        return_value = PAL_STATUS_SUCCESS;

    }while (0);
    return return_value;
}

/// @cond hidden
//lint --e{715,830,818} suppress "As this is reference api to be implemented by the user to generate random number from wolfssl"
int32_t CryptoLib_GenerateSeed(uint8_t * PpbSeed, uint32_t PdwSeedLength)
{
    int32_t i4Status  = 0;
    //"User need to write an Platform specific wc_GenerateSeed() here"
    return i4Status;
}
/// @endcond 
/**
* @}
*/
