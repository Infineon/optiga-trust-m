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
* \file example_optiga_crypt_rsa_verify.c
*
* \brief   This file provides the example for RSA verify operation using #optiga_crypt_rsa_verify.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_RSA_VERIFY_ENABLED

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
static uint8_t public_key [] = {
    //BIT String
    0x03,
        //BIT String Length
        0x81, 0x8E,
        //UnusedBits
        0x00,
        //SEQUENCE
        0x30,
            //Length
            0x81, 0x8A,
            //INTEGER (pub key)
            0x02,
            //Pub key modulus length
            0x81, 0x81,
            //Public key modulus
            0x00,
            0xA1, 0xD4, 0x6F, 0xBA, 0x23, 0x18, 0xF8, 0xDC, 0xEF, 0x16, 0xC2, 0x80, 0x94, 0x8B, 0x1C, 0xF2,
            0x79, 0x66, 0xB9, 0xB4, 0x72, 0x25, 0xED, 0x29, 0x89, 0xF8, 0xD7, 0x4B, 0x45, 0xBD, 0x36, 0x04,
            0x9C, 0x0A, 0xAB, 0x5A, 0xD0, 0xFF, 0x00, 0x35, 0x53, 0xBA, 0x84, 0x3C, 0x8E, 0x12, 0x78, 0x2F,
            0xC5, 0x87, 0x3B, 0xB8, 0x9A, 0x3D, 0xC8, 0x4B, 0x88, 0x3D, 0x25, 0x66, 0x6C, 0xD2, 0x2B, 0xF3,
            0xAC, 0xD5, 0xB6, 0x75, 0x96, 0x9F, 0x8B, 0xEB, 0xFB, 0xCA, 0xC9, 0x3F, 0xDD, 0x92, 0x7C, 0x74,
            0x42, 0xB1, 0x78, 0xB1, 0x0D, 0x1D, 0xFF, 0x93, 0x98, 0xE5, 0x23, 0x16, 0xAA, 0xE0, 0xAF, 0x74,
            0xE5, 0x94, 0x65, 0x0B, 0xDC, 0x3C, 0x67, 0x02, 0x41, 0xD4, 0x18, 0x68, 0x45, 0x93, 0xCD, 0xA1,
            0xA7, 0xB9, 0xDC, 0x4F, 0x20, 0xD2, 0xFD, 0xC6, 0xF6, 0x63, 0x44, 0x07, 0x40, 0x03, 0xE2, 0x11,
            //INTEGER (publicExponent)
            0x02,
            //Pub key exponent length
            0x04,
            //Public Exponent
            0x00, 0x01, 0x00, 0x01
};

//SHA-256 Digest
static uint8_t digest [] =
{
    0x91, 0x70, 0x02, 0x48, 0x3F, 0xBD, 0x5F, 0xDD, 0xD5, 0x38, 0xEB, 0xDA, 0x9A, 0x5E, 0x1F, 0x46,
    0xFC, 0xAD, 0x8F, 0x1E, 0x2C, 0x75, 0xB0, 0x83, 0xD0, 0x71, 0x2B, 0x80, 0xD4, 0xAA, 0xC6, 0x9B
};

// RSA 1024 Signature ( no additional encoding needed )
static uint8_t signature [] =
{
    0x5B, 0xDE, 0x46, 0xE4, 0x35, 0x48, 0xF4, 0x81, 0x45, 0x7C, 0x72, 0x31, 0x54, 0x55, 0xE8, 0x9F,
    0x1D, 0xD0, 0x5D, 0x9D, 0xEC, 0x40, 0xE6, 0x6B, 0x89, 0xF3, 0xBC, 0x52, 0x68, 0xB1, 0xD8, 0x70,
    0x35, 0x05, 0xFC, 0x98, 0xF6, 0x36, 0x99, 0x24, 0x53, 0xF0, 0x17, 0xB8, 0x9B, 0xD4, 0xA0, 0x5F,
    0x12, 0x04, 0x8A, 0xA1, 0xA7, 0x96, 0xE6, 0x33, 0xCA, 0x48, 0x84, 0xD9, 0x00, 0xE4, 0xA3, 0x8E,
    0x2F, 0x6F, 0x3F, 0x6D, 0xE0, 0x1D, 0xF8, 0xEA, 0xE0, 0x95, 0xBA, 0x63, 0x15, 0xED, 0x7B, 0x6A,
    0xB6, 0x6E, 0x20, 0x17, 0xB5, 0x64, 0xDE, 0x49, 0x64, 0x97, 0xCA, 0x5E, 0x4D, 0x84, 0x63, 0xA0,
    0xF1, 0x00, 0x6C, 0xEE, 0x70, 0x89, 0xD5, 0x6E, 0xC5, 0x05, 0x31, 0x0D, 0xAA, 0xB7, 0xBA, 0xA0,
    0xAA, 0xBF, 0x98, 0xE8, 0x39, 0x93, 0x70, 0x07, 0x2D, 0xFF, 0x42, 0xF9, 0xA4, 0x6F, 0x1B, 0x00
};

/**
 * The below example demonstrates the verification of signature using
 * the public key provided by host.
 *
 * Example for #optiga_crypt_rsa_verify
 *
 */
void example_optiga_crypt_rsa_verify(void)
{
    public_key_from_host_t public_key_details =
    {
         public_key,
         sizeof(public_key),
         (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL
    };

    optiga_lib_status_t return_status = 0;
    optiga_crypt_t * me = NULL;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
    do
    {
        /**
         * 1. Create OPTIGA Crypt Instance
         */
        me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. Verify RSA signature using public key from host
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_verify (me,
                                                 OPTIGA_RSASSA_PKCS1_V15_SHA256,
                                                 digest,
                                                 sizeof(digest),
                                                 signature,
                                                 sizeof(signature),
                                                 OPTIGA_CRYPT_HOST_DATA,
                                                 &public_key_details,
                                                 0x0000);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_verify operation is completed
        }

        if ((OPTIGA_LIB_SUCCESS != optiga_lib_status))
        {
            //RSA Signature verification failed.
            return_status = optiga_lib_status;
            break;
        }
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

#endif  //OPTIGA_CRYPT_RSA_VERIFY_ENABLED
/**
* @}
*/
