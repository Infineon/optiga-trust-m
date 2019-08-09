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
* \brief   This file implements APIs, types and data structures used for protected update pal crypt.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "string.h"
#include "pal/pal_logger.h"
#include "pal/pal_crypt.h"

int pal_crypt_calculate_sha256_hash(	const unsigned char * message,
										unsigned short message_len,
										unsigned char * digest)
{
	int status = -1;
	mbedtls_sha256_context sha256_ctx;
	do
	{
		mbedtls_sha256_init(&sha256_ctx);

		if (0 != mbedtls_sha256_starts_ret(&sha256_ctx, 0))
		{
			pal_logger_print_message(" FAILED : pal_crypt : mbedtls_sha256_starts_ret");
			break;
		}
		if (0 != mbedtls_sha256_update_ret(&sha256_ctx, message, message_len))
		{
			pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_sha256_update_ret");
			break;
		}

		if (0 != mbedtls_sha256_finish_ret(&sha256_ctx, digest))
		{
			pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_sha256_finish_ret");
			break;
		}
		mbedtls_sha256_free(&sha256_ctx);
		status = 0;
	} while (0);
    return status;
}

int pal_crypt_hash(	pal_crypt_t* p_pal_crypt,
					unsigned char hash_algorithm,
					const unsigned char * p_message,
					unsigned int message_length,
					unsigned char * p_digest)
{
	int status = -1;
	// SHA-256 as defined by CDDL
	if(41 == hash_algorithm) 
	{
		status = pal_crypt_calculate_sha256_hash(p_message, (unsigned short)message_length, p_digest);
	}

    return status;
}
 

int pal_crypt_get_signature_length(	unsigned char * p_private_key, unsigned short * sign_len)
{
	int status = -1;
	mbedtls_pk_context ctx;
	mbedtls_ecp_keypair * ecc_ctx;
	uint8_t key_type = 0;
	
	mbedtls_pk_init(&ctx);
	do
	{
		if (0 != mbedtls_pk_parse_keyfile(&ctx, (const char*)p_private_key, ""))
		{
			pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_pk_parse_keyfile");
			break;
		}
		key_type = mbedtls_pk_get_type(&ctx);
		if(MBEDTLS_PK_ECKEY == key_type)
		{
			ecc_ctx = (mbedtls_ecp_keypair *)ctx.pk_ctx;
			// To get the length of R & S component, 8/2 = 4
			*sign_len = (ecc_ctx->grp.nbits)/4; 
		}
		else
		{
			pal_logger_print_message(" FAILED :  pal_crypt : Not supported");
		}
		status = 0;
	}while(0);

	mbedtls_pk_free(&ctx);
	return status;
}

int pal_crypt_sign(	pal_crypt_t* p_pal_crypt,
					unsigned char * p_digest, 
					unsigned short digest_length,
					unsigned char * p_signature,
					unsigned short * signature_length,
					const unsigned char * p_private_key,
					unsigned short private_key_length)
{
	int status = -1;
	mbedtls_pk_context ctx;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	unsigned char hash[32];
    const char *pers = "mbedtls_pk_sign";
	size_t signature_buff_len;

	do
	{
			mbedtls_entropy_init(&entropy);
			mbedtls_ctr_drbg_init(&ctr_drbg);
			mbedtls_pk_init(&ctx);

			if(0 != mbedtls_ctr_drbg_seed(  &ctr_drbg, mbedtls_entropy_func, 
											&entropy, 
											(const unsigned char *)pers, 
											strlen(pers)))
			{
				pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_ctr_drbg_seed");
				break;
			}
			//Read key into an array
			if (0 != mbedtls_pk_parse_keyfile(&ctx, (const char*)p_private_key, ""))
			{
				pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_pk_parse_keyfile");
				break;
			}

			// generate hash
			if (0 != pal_crypt_hash(NULL, 41, p_digest, digest_length, hash))
			{
				pal_logger_print_message(" FAILED :  pal_crypt : pal_crypt_hash");
				break;
			}

			if (0 != mbedtls_pk_sign(&ctx, MBEDTLS_MD_SHA256, hash, 0, p_signature, &signature_buff_len,
				mbedtls_ctr_drbg_random, &ctr_drbg))
			{
				pal_logger_print_message(" FAILED :  pal_crypt : mbedtls_pk_sign");
				break;
			}
			*signature_length = signature_buff_len;
			mbedtls_pk_free(&ctx);
			mbedtls_ctr_drbg_free(&ctr_drbg);
			mbedtls_entropy_free(&entropy);
			status = 0;
	} while (0);
    return status;
}

/**
* @}
*/