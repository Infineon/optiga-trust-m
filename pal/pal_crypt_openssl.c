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
* \file pal_crypt_mbedtls.c
*
* \brief   This file implements the platform abstraction layer APIs for cryptographic functions using mbedTLS SW Crypto.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_crypt.h"
#include "optiga/pal/pal_os_memory.h"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/opensslv.h>
#include <openssl/kdf.h>

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
	
    pal_status_t return_value = PAL_STATUS_FAILURE;
    uint8_t md_hmac_temp_array[PAL_CRYPT_MAX_LABEL_SEED_LENGTH];
    uint16_t final_seed_length = 0;
	EVP_PKEY_CTX *ctx;
	
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_secret) || (NULL == p_label) || (NULL == p_seed) || (NULL == p_derived_key))
        {
            break;
        }
#endif  //OPTIGA_LIB_DEBUG_NULL_CHECK
		
		ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_TLS1_PRF, NULL);
		if (!(EVP_PKEY_derive_init(ctx)))
		{
			break;
		}

		memcpy(md_hmac_temp_array, p_label, label_length);
		memcpy(md_hmac_temp_array + label_length, p_seed, seed_length);
		final_seed_length = label_length + seed_length;
		
		if (!(EVP_PKEY_CTX_set_tls1_prf_md(ctx, EVP_sha256())))
		{
			break;
		}
		if (!(EVP_PKEY_CTX_set1_tls1_prf_secret(ctx, p_secret, secret_length)))
		{
			break;
		}
		if (!(EVP_PKEY_CTX_add1_tls1_prf_seed(ctx, md_hmac_temp_array, final_seed_length)))
		{
			break;
		}
		if (!(EVP_PKEY_derive(ctx, p_derived_key, (size_t *)&derived_key_length)))
		{
			break;
		}

        return_value = PAL_STATUS_SUCCESS;
    } while (FALSE);

	EVP_PKEY_CTX_free(ctx);	
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
    pal_status_t return_status = PAL_STATUS_FAILURE;
    uint8_t mac_output[16];

	EVP_CIPHER_CTX *ctx;
	int outlen;
	int ciphertextlen;
	
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_cipher_text) || (NULL == p_plain_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_encrypt_key))
        {
            break;
        }
#endif
		
		if (!(ctx = EVP_CIPHER_CTX_new()))
        {
            break;
        }
			
		// Set cipher type and mode
		if (!(EVP_EncryptInit_ex(ctx, EVP_aes_128_ccm(), NULL, NULL, NULL)))
        {
            break;
        }
		
		// Setting IV length (nonce length)	
		if(!(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, nonce_length, NULL)))
        {
            break;
        }
			
		// Set tag length
		EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, mac_size, NULL);

		// Initialise key and IV 
		if(!(EVP_EncryptInit_ex(ctx, NULL, NULL, p_encrypt_key, p_nonce)))
        {
            break;
        }

		//Set plaintext length
		if(!(EVP_EncryptUpdate(ctx, NULL, &outlen, NULL, plain_text_length)))
        {
            break;
        }

		// Zero or one call to specify any AAD 
		if(!(EVP_EncryptUpdate(ctx, NULL, &outlen, p_associated_data, associated_data_length)))
        {
            break;
        }

		// Encrypt plaintext: can only be called once
		if(!(EVP_EncryptUpdate(ctx, p_cipher_text, &outlen, p_plain_text, plain_text_length)))
        {
            break;
        }
		ciphertextlen = outlen;
		// Finalise: note get no output for CCM
		if(!(EVP_EncryptFinal_ex(ctx, (p_cipher_text + outlen), &outlen)))
        {
            break;
        }
		ciphertextlen += outlen;

		// Get MAC tag
		if(!(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_GET_TAG, mac_size, mac_output)))
        {
            break;
        }

		memcpy((p_cipher_text+ciphertextlen) , mac_output, mac_size);
	
        return_status = PAL_STATUS_SUCCESS;
    } while (FALSE);
	EVP_CIPHER_CTX_free(ctx);
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
    pal_status_t return_status = PAL_STATUS_FAILURE;
	EVP_CIPHER_CTX *ctx;
	uint8_t outbuf[1560];
    int outlen;

    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == p_plain_text) || (NULL == p_cipher_text) ||
            (NULL == p_nonce) || (NULL == p_associated_data) || (NULL == p_decrypt_key))
        {
            break;
        }
#endif
		
		if (!(ctx = EVP_CIPHER_CTX_new()))
		{
			break;
		}
			
		// Set cipher type and mode
		if (!(EVP_DecryptInit_ex(ctx, EVP_aes_128_ccm(), NULL, NULL, NULL)))
		{
			break;
		}
		
		// Setting IV length (nonce length)	
		if(!(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_IVLEN, nonce_length, NULL)))
		{
			break;
		}
			
		// Set expected tag value 
		EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_CCM_SET_TAG, mac_size, (uint8_t *) &p_cipher_text[cipher_text_length - mac_size]);

		// Initialise key and IV 
		if(!(EVP_DecryptInit_ex(ctx, NULL, NULL, p_decrypt_key, p_nonce)))
		{
			break;
		}
			
		//Set cipher text length
		if(!(EVP_DecryptUpdate(ctx, NULL, &outlen, NULL, cipher_text_length - mac_size)))
		{
			break;
		}		

		// Zero or one call to specify any AAD 
		if(!(EVP_DecryptUpdate(ctx, NULL, &outlen, p_associated_data, associated_data_length)))
		{
			break;
		}
		// Decrypt cipher text: can only be called once
		if(!(EVP_DecryptUpdate(ctx, outbuf, &outlen, p_cipher_text, cipher_text_length - mac_size)))
		{
			break;
		}
		
		memcpy(p_plain_text,outbuf, outlen);
			
        return_status = PAL_STATUS_SUCCESS;
    } while (FALSE);
	EVP_CIPHER_CTX_free(ctx);    
    return return_status;
}

pal_status_t pal_crypt_version(uint8_t * p_crypt_lib_version_info, uint16_t * length)
{
    pal_status_t return_value  = PAL_STATUS_FAILURE;    
	char *version;

	version = (char *)OpenSSL_version(OPENSSL_VERSION);
	
    do
    {
        if (strlen(version) > *length)
        {
            break;
        }

        pal_os_memcpy(p_crypt_lib_version_info, version, strlen(version));
        *length = strlen(version);

        return_value = PAL_STATUS_SUCCESS;

    } while (0);
    return return_value;
}

/**
* @}
*/
