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
* \file example_optiga_crypt_rsa_sign.c
*
* \brief   This file provides the example for RSA Sign operation using #optiga_crypt_rsa_sign.
*
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"

#ifdef OPTIGA_CRYPT_RSA_SIGN_ENABLED

extern void example_log_execution_status(const char_t* function, uint8_t status);
extern void example_log_function_name(const char_t* function);

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

// SHA-256 digest to be signed
static uint8_t digest [] = 
{
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A,0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F,0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};
    
/**
 * The below example demonstrates the signing of digest using
 * the Private key present in OPTIGA Key store.
 *
 * Example for #optiga_crypt_rsa_sign
 *
 */
void example_optiga_crypt_rsa_sign(void)
{

    uint8_t logging_status = 0;

    //To store the signture generated
    uint8_t signature [200];
    uint16_t signature_length = sizeof(signature);

    //Crypt Instance
    optiga_crypt_t * me = NULL;
    optiga_lib_status_t return_status;
    example_log_function_name(__FUNCTION__);

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
         * 2. Sign the digest -
         *       - Use Private key from Key Store ID E0FC
         *       - Signature scheme is SHA256,
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_sign(me,
                                              OPTIGA_RSASSA_PKCS1_V15_SHA256,
                                              digest,
                                              sizeof(digest),
                                              OPTIGA_KEY_ID_E0FC,
                                              signature,
                                              &signature_length,
                                              0x0000);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) 
        {
            //Wait until the optiga_crypt_rsa_sign operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //RSA Signature generation failed.
            break;
        }
        logging_status = 1;

    } while (FALSE);

    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
    }
    example_log_execution_status(__FUNCTION__,logging_status);
}
#endif  //OPTIGA_CRYPT_RSA_SIGN_ENABLED
/**
* @}
*/
