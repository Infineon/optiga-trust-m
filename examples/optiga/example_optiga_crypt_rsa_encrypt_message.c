/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
* \file example_optiga_crypt_rsa_encrypt_message.c
*
* \brief    This file provides the example for RSA Encryption using #optiga_crypt_rsa_encrypt_message.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

void example_util_encode_rsa_public_key_in_bit_string_format(const uint8_t * n_buffer,
                                                        uint16_t n_length,
                                                        const uint8_t * e_buffer,
                                                        uint16_t e_length,
                                                        uint8_t * pub_key_buffer,
                                                        uint16_t * pub_key_length);
                                                        
/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

// RSA 1024 public key
static const uint8_t rsa_public_key_modulus [] = 
{
    //Public key modulus
    0xA1, 0xD4, 0x6F, 0xBA, 0x23, 0x18, 0xF8, 0xDC, 0xEF, 0x16, 0xC2, 0x80, 0x94, 0x8B, 0x1C, 0xF2,
    0x79, 0x66, 0xB9, 0xB4, 0x72, 0x25, 0xED, 0x29, 0x89, 0xF8, 0xD7, 0x4B, 0x45, 0xBD, 0x36, 0x04,
    0x9C, 0x0A, 0xAB, 0x5A, 0xD0, 0xFF, 0x00, 0x35, 0x53, 0xBA, 0x84, 0x3C, 0x8E, 0x12, 0x78, 0x2F,
    0xC5, 0x87, 0x3B, 0xB8, 0x9A, 0x3D, 0xC8, 0x4B, 0x88, 0x3D, 0x25, 0x66, 0x6C, 0xD2, 0x2B, 0xF3,
    0xAC, 0xD5, 0xB6, 0x75, 0x96, 0x9F, 0x8B, 0xEB, 0xFB, 0xCA, 0xC9, 0x3F, 0xDD, 0x92, 0x7C, 0x74,
    0x42, 0xB1, 0x78, 0xB1, 0x0D, 0x1D, 0xFF, 0x93, 0x98, 0xE5, 0x23, 0x16, 0xAA, 0xE0, 0xAF, 0x74,
    0xE5, 0x94, 0x65, 0x0B, 0xDC, 0x3C, 0x67, 0x02, 0x41, 0xD4, 0x18, 0x68, 0x45, 0x93, 0xCD, 0xA1,
    0xA7, 0xB9, 0xDC, 0x4F, 0x20, 0xD2, 0xFD, 0xC6, 0xF6, 0x63, 0x44, 0x07, 0x40, 0x03, 0xE2, 0x11,
};

// RSA 1024 public key
static const uint8_t rsa_public_key_exponent [] = 
{
    //Public Exponent
    0x00, 0x01, 0x00, 0x01
};

const uint8_t message[] = {"RSA PKCS1_v1.5 Encryption of user message"};

static uint8_t rsa_public_key[150] = {0x00};
static uint16_t rsa_public_key_length = 0;

/**
 * The below example demonstrates RSA encryption
 * #optiga_crypt_rsa_encrypt_message where message is provided by user
 *
 */
void example_optiga_crypt_rsa_encrypt_message(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_rsa_encryption_scheme_t encryption_scheme;
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    public_key_from_host_t public_key_from_host;

    optiga_crypt_t * me = NULL;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do
    {
        /**
         * 1. Create OPTIGA Crypt Instance
         *
         */
        me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. RSA encryption
         */
        // Form rsa public key in bit string format
        example_util_encode_rsa_public_key_in_bit_string_format(rsa_public_key_modulus,
                                                           sizeof(rsa_public_key_modulus),
                                                           rsa_public_key_exponent,
                                                           sizeof(rsa_public_key_exponent),
                                                           rsa_public_key,
                                                           &rsa_public_key_length);
                                                           
        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        public_key_from_host.public_key = rsa_public_key;
        public_key_from_host.length = rsa_public_key_length;
        public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_encrypt_message(me,
                                                            encryption_scheme,
                                                            message,
                                                            sizeof(message),
                                                            NULL,
                                                            0,
                                                            OPTIGA_CRYPT_HOST_DATA,
                                                            &public_key_from_host,
                                                            encrypted_message,
                                                            &encrypted_message_length);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}
#endif  //OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

/**
* @}
*/
