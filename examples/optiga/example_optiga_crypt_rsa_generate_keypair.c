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
* \file example_optiga_crypt_rsa_generate_keypair.c
*
* \brief   This file provides the example for generation of RSA keypair using
*          optiga_crypt_rsa_generate_keypair.
*
* \ingroup grOptigaExamples
*
* @{
*/
#include "optiga/optiga_util.h"
#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED

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

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

/**
 * Sample metadata of 0xE0F1 
 */
const uint8_t E0FC_metadata[] = { 0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00 };

/**
 * The below example demonstrates the generation of
 * RSA Key pair using #optiga_crypt_rsa_generate_keypair.
 *
 */
void example_optiga_crypt_rsa_generate_keypair(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    uint16_t optiga_oid;
    
    //To store the generated public key as part of Generate key pair
    /*
    *
    * Give a buffer of minimum size required to store the exported public key.
    * E.g
    * For 1024 bit key : modulus(128 bytes) + public expoenent(3 bytes) + Encoding bytes length(approximately 15 bytes)
    * For 2048 bit key : modulus(256 bytes) + public expoenent(3 bytes) + Encoding bytes length(approximately 15 bytes)
    */
    uint8_t public_key [150];
    uint16_t public_key_length = sizeof(public_key);

    optiga_crypt_t * crypt_me = NULL;
    optiga_util_t * util_me = NULL;    
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do
    {
        /**
         * 1. Create OPTIGA Crypt Instance
         */
        crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == crypt_me)
        {
            break;
        }

        util_me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == util_me)
        {
            break;
        }

        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_oid = 0xE0FC;
        return_status = optiga_util_write_metadata(util_me,
                                                   optiga_oid,
                                                   E0FC_metadata,
                                                   sizeof(E0FC_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * 2. Generate RSA Key pair
         *       - Use 1024 or 2048 bit RSA key
         *       - Specify the Key Usage
         *       - Store the Private key in OPTIGA Key store
         *              (When exporting the private key, provide buffer of sufficient length (key size in bytes +
         *               encoding length))
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_E0FC;
        return_status = optiga_crypt_rsa_generate_keypair(crypt_me,
                                                          OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                                          (uint8_t)OPTIGA_KEY_USAGE_SIGN,
                                                          FALSE,
                                                          &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);

    if (crypt_me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(crypt_me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    if (util_me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(util_me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }      
}

#endif  //OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED
/**
* @}
*/
