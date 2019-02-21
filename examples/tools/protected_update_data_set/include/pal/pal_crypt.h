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

// Generates SHA256 hash
int pal_crypt_calculate_sha256_hash(unsigned char * message,
	unsigned short message_len,
	unsigned char * digest);

// Calculates RSA/ ECDSA signature
int pal_crypt_calculate_signature(unsigned char * message,
	unsigned short message_length,
	char * private_key,
	unsigned char * signature,
	unsigned short * signature_len,
	unsigned char sign_alg);

#endif //_PAL_CRYPTO_

/**
* @}
*/