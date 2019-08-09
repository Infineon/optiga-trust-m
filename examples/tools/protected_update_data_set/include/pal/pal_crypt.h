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
* \file pal_crypt.h
*
* \brief   This file defines APIs, types and data structures used for protected update pal crypt.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include <stdio.h>
#ifndef _PAL_CRYPTO_
#define _PAL_CRYPTO_

typedef struct pal_crypt_t
{
    /// callback
    void * callback_ctx;
}pal_crypt_t;

// Generates hash
int pal_crypt_hash(	pal_crypt_t* p_pal_crypt,
					unsigned char hash_algorithm,
					const unsigned char * p_message,
					unsigned int message_length,
					unsigned char * p_digest);

// Calculates signature
int pal_crypt_sign(	pal_crypt_t* p_pal_crypt,
					unsigned char * p_digest, 
					unsigned short digest_length,
					unsigned char * p_signature,
					unsigned short * signature_length,
					const unsigned char * p_private_key,
					unsigned short private_key_length);

// Gets the signature length based on private key
int pal_crypt_get_signature_length(	unsigned char * p_private_key, 
									unsigned short * sign_len);
#endif //_PAL_CRYPTO_

/**
* @}
*/