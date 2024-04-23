/**
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
* @{
*/

#include "mbedtls/config.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
//#include "../include/trustm_rsa.h"

#include <string.h>
#include "optiga_crypt.h"
#include "optiga_util.h"
#include "optiga_lib_common.h"
#include "pal_os_timer.h"
#include "pal_os_memory.h"

static const uint8_t digest521[] = {
    0x5C, 0xB4, 0x9C, 0x2E, 0x18, 0xAF, 0xC5, 0x8A, 0xF4, 0xBF, 0x36, 0xE4, 0x79, 0x8E, 0xC0, 0xED, 
    0xF6, 0x8E, 0xBB, 0x02, 0x45, 0x65, 0xF4, 0xCA, 0xA1, 0xDF, 0xC1, 0x27, 0x48, 0x47, 0x16, 0x7C, };
/*
static const uint8_t privatekey521[] = {
0x01, 0x74, 0xA0, 0x33, 0xFA, 0x30, 0x18, 0x6A, 0x85, 0x7F, 0x2E, 0x46, 0x8A, 0xBA, 0xBB, 0x3E, 
0x1E, 0x76, 0x3A, 0xAD, 0x8D, 0x16, 0xB5, 0x87, 0x9A, 0xB1, 0xA0, 0xB8, 0x44, 0x94, 0x5A, 0x04, 
0xFD, 0x3A, 0x69, 0xEF, 0xC1, 0x73, 0x71, 0xA1, 0x7F, 0x80, 0x26, 0xBB, 0x8D, 0x99, 0x04, 0x46, 
0xED, 0x48, 0x12, 0x3A, 0xC4, 0x60, 0x99, 0x9C, 0x9D, 0x66, 0x9B, 0xBB, 0xC6, 0xF0, 0x14, 0x21, 
0xC9, 0x1D, };
*/

static const uint8_t publickey521[] = {
	0x04,
    0x00, 0x43, 0xE6, 0xB1, 0xB0, 0xD5, 0x9A, 0xF5, 0xDA, 0xBD, 0x03, 0x0F, 0x72, 0xD5, 0x0D, 0x17, 
    0x70, 0xD7, 0xC5, 0xF6, 0xB7, 0x6A, 0xEB, 0x99, 0xCC, 0xA5, 0x91, 0xB4, 0xBA, 0x42, 0xD4, 0xDA, 
    0x07, 0x56, 0x7F, 0x43, 0x81, 0x9E, 0x73, 0x3C, 0xF2, 0xFE, 0x56, 0xC6, 0xB3, 0x94, 0xCC, 0x10, 
    0x26, 0x6F, 0xBB, 0x5C, 0x21, 0x7A, 0x5E, 0x66, 0x95, 0x1D, 0x26, 0xDB, 0xC1, 0xA6, 0x2E, 0xBE, 
    0x7F, 0x30, 0x00, 0x44, 0xBF, 0xD1, 0xC8, 0x05, 0xDE, 0x3C, 0x6F, 0x2B, 0xDE, 0x3C, 0xF9, 0x1D, 
    0x6D, 0x0A, 0x5D, 0x2B, 0x91, 0xC4, 0x7A, 0x40, 0xDA, 0x14, 0x4F, 0x40, 0x08, 0x23, 0x37, 0x8F, 
    0x64, 0x94, 0x65, 0xD8, 0x79, 0x21, 0x7C, 0xC2, 0xE7, 0xA3, 0xF8, 0x76, 0x08, 0x7C, 0xA3, 0xC8, 
    0x0B, 0x7B, 0xDE, 0xC9, 0xCA, 0xE6, 0x3A, 0x18, 0x79, 0xB1, 0xAA, 0x9F, 0x37, 0xD3, 0x45, 0x83, 
    0x95, 0xD2, 0xFD, 0x9F, 
}; 

static const uint8_t signature521[] = {
	0x02, 0x42,
    0x01, 0x4E, 0xAE, 0xF6, 0x59, 0x19, 0x94, 0x20, 0x97, 0x77, 0xED, 0x24, 0x53, 0x63, 0x18, 0x83, 
    0x6D, 0x43, 0xD1, 0x5A, 0xFA, 0x6D, 0xCD, 0xCA, 0x1A, 0xD4, 0x43, 0x6F, 0x35, 0x4A, 0x54, 0x18, 
    0x24, 0x0A, 0x14, 0xCA, 0x7A, 0xD7, 0x80, 0x0E, 0x57, 0xE2, 0x4B, 0x9F, 0xBA, 0x52, 0x70, 0xBE, 
    0x51, 0xBB, 0x28, 0x7A, 0x0F, 0x5C, 0xA9, 0x93, 0xB9, 0x9E, 0xCD, 0x69, 0xE1, 0x27, 0x28, 0xFA, 
    0x16, 0x62, 
    0x02, 0x42,
	0x00, 0x03, 0x10, 0xAD, 0x0F, 0xF9, 0xD1, 0x12, 0x1F, 0x1B, 0x2E, 0xF3, 0xE8, 0x84, 0xB6, 0xDC, 
	0xE3, 0xF3, 0x2E, 0x44, 0xD5, 0xC3, 0x05, 0xF4, 0x78, 0xA8, 0x85, 0xBC, 0x49, 0x1F, 0x1F, 0x73, 
	0x4A, 0x86, 0x89, 0x29, 0x98, 0xD2, 0x9B, 0x9B, 0x87, 0x1C, 0xB8, 0xE9, 0x88, 0x9E, 0x3C, 0x17, 
	0x7B, 0x63, 0xDD, 0x68, 0xB2, 0xFE, 0x07, 0xAD, 0x3B, 0xA1, 0x8C, 0x5F, 0xB3, 0x3D, 0xCD, 0xFA, 
	0x86, 0xBE, 
};

static const uint8_t digestbp256[] = {
    0x2D, 0x76, 0x15, 0x14, 0x36, 0x09, 0x59, 0xEF, 0x29, 0xD3, 0xFC, 0xF1, 0xDE, 0x73, 0x0C, 0xBF, 
    0xBF, 0x44, 0x8D, 0x61, 0x28, 0xA3, 0x79, 0x40, 0x72, 0xA1, 0x23, 0xA3, 0x91, 0xF2, 0x11, 0x4A,
}; 

static const uint8_t publickeybp256[] = {
	0x04,
    0x51, 0xF1, 0xD1, 0xAA, 0x4D, 0x98, 0xC3, 0x70, 0xF5, 0x1A, 0x07, 0xE6, 0x3B, 0x3E, 0x5C, 0x3F, 
    0x87, 0xC3, 0x56, 0x42, 0xC4, 0x5E, 0x10, 0xC0, 0x2B, 0x85, 0xE5, 0x67, 0xE9, 0x61, 0x4C, 0x39, 
    0x62, 0xFD, 0x96, 0xDE, 0x72, 0xEB, 0x8B, 0xA5, 0x6E, 0x6F, 0x1F, 0xCE, 0x2D, 0x94, 0xCD, 0x41, 
    0x71, 0xF9, 0x17, 0xD8, 0x45, 0x76, 0x44, 0x8A, 0xD0, 0x4C, 0x85, 0xEE, 0xAE, 0xD0, 0x92, 0x97,
}; 

static const uint8_t signaturebp256[] = {
	0x02, 0x20,
    0x2F, 0x77, 0xD4, 0x7E, 0x91, 0xD4, 0x54, 0x90, 0xC0, 0x94, 0xC2, 0x2B, 0x55, 0xF3, 0xDD, 0xF4, 
    0xBC, 0x21, 0x99, 0x51, 0x1F, 0xCD, 0x02, 0xD5, 0x66, 0x2C, 0xCA, 0x25, 0xD2, 0xEC, 0x45, 0x8B,
	0x02, 0x20,
    0x38, 0xF0, 0xD4, 0xBC, 0xFD, 0xCB, 0xEC, 0x66, 0x0B, 0x00, 0xD2, 0xCE, 0xCF, 0x5D, 0x01, 0xCA, 
    0xBC, 0x4B, 0xD2, 0x9C, 0x75, 0xF2, 0x37, 0x90, 0x59, 0x81, 0x9D, 0x09, 0x80, 0x8F, 0xEF, 0xB4, 
};

static const uint8_t digestbp384[] = {
    0xCD, 0x3C, 0x96, 0x7F, 0xB3, 0xEA, 0xA4, 0x20, 0x3A, 0x7D, 0x15, 0x1A, 0x82, 0x8F, 0xC5, 0x47, 
    0xF0, 0xF2, 0x8E, 0x42, 0xAD, 0x26, 0xE6, 0xD7, 0x49, 0x57, 0x5B, 0x9C, 0x5E, 0xA5, 0x42, 0xA3
}; 
    
static const uint8_t publickeybp384[] = {
	0x04,
    0x1A, 0x4B, 0xFC, 0xC2, 0xCF, 0xCA, 0x0D, 0x3C, 0xEC, 0x3D, 0xDB, 0x68, 0x75, 0xA5, 0x32, 0x6B, 
    0x97, 0x56, 0xF8, 0x8D, 0xE0, 0x9E, 0x91, 0xFA, 0x55, 0xC8, 0x48, 0x3D, 0x1F, 0x12, 0x0E, 0x91, 
    0x6C, 0x29, 0x5C, 0x17, 0x76, 0x63, 0x7A, 0x08, 0x79, 0x75, 0x94, 0x35, 0x5F, 0xF4, 0x2E, 0x77, 
    0x7B, 0x53, 0x35, 0xFA, 0x5C, 0xCF, 0x2F, 0x7D, 0x3F, 0xAD, 0x74, 0x7A, 0x68, 0x1D, 0x32, 0x1F, 
    0xD7, 0x0D, 0xD4, 0x08, 0xE5, 0x44, 0x16, 0x31, 0xC7, 0xFE, 0x4C, 0xD4, 0x22, 0x07, 0x33, 0xB5, 
    0x0F, 0x37, 0x60, 0x9F, 0x1D, 0xBA, 0xE0, 0x38, 0x0A, 0x1D, 0x49, 0x48, 0x75, 0x31, 0x37, 0xAE, 
}; 

static const uint8_t signaturebp384[] = {	
	0x02, 0x30,
    0x16, 0x8E, 0x22, 0x10, 0x0C, 0xDB, 0xF4, 0x57, 0xE2, 0xC0, 0xCA, 0x2F, 0x44, 0xBD, 0x62, 0xF9, 
    0x8B, 0xF3, 0x52, 0x1D, 0xB2, 0xC9, 0x1A, 0xF8, 0x0F, 0x87, 0x48, 0xD5, 0x91, 0xF1, 0x4C, 0x94, 
    0xBE, 0xE7, 0x8E, 0x0F, 0x45, 0xCE, 0xA7, 0x2B, 0xFF, 0xE6, 0x41, 0x07, 0xE5, 0x5F, 0xD9, 0xB4,
	0x02, 0x30, 
    0x4E, 0x06, 0x91, 0x47, 0x85, 0xF5, 0xC0, 0x05, 0xDC, 0x51, 0x26, 0x5F, 0x88, 0x6C, 0x6D, 0x12, 
    0x50, 0x3B, 0xAC, 0x27, 0xDF, 0xCF, 0x09, 0x1E, 0xF9, 0xEE, 0xA7, 0xBB, 0xDC, 0x67, 0xED, 0x79, 
    0x8C, 0x7E, 0xA3, 0x92, 0xE2, 0x54, 0xBE, 0x32, 0xB3, 0xC2, 0xF5, 0xC0, 0xDE, 0xC8, 0x56, 0x1F, 
};

static const uint8_t digestbp512[] = {
    0xFF, 0x89, 0xD2, 0x9D, 0x1E, 0x34, 0xB1, 0xF3, 0x93, 0xAF, 0x20, 0x39, 0x17, 0x2D, 0x50, 0xC9, 
    0x4E, 0x58, 0x79, 0x8A, 0x73, 0x4A, 0x83, 0xCD, 0xBD, 0xCA, 0xDB, 0x50, 0xBE, 0xA3, 0x85, 0x8D, 
}; 

static const uint8_t publickeybp512[] = {
	0x04,
    0x28, 0x6C, 0xA7, 0x8E, 0x5D, 0x99, 0x59, 0xD4, 0x10, 0x54, 0xEA, 0x8F, 0xB4, 0x7B, 0x79, 0x67, 
    0x37, 0x2E, 0xB4, 0x6F, 0x23, 0x59, 0x60, 0x9C, 0xD8, 0x1F, 0x5E, 0xA5, 0xE8, 0x89, 0x06, 0x1A, 
    0x3A, 0x6E, 0x0B, 0x96, 0x22, 0x9E, 0x8D, 0xC9, 0x97, 0x63, 0xE5, 0x6A, 0x51, 0xF0, 0xF2, 0xE9, 
    0x68, 0xCA, 0x79, 0xA1, 0xE7, 0x78, 0x6F, 0x84, 0xAB, 0xC8, 0xDE, 0x6D, 0x87, 0x1B, 0x1F, 0xCC, 
    0x47, 0xEF, 0x71, 0xEE, 0xF6, 0xC9, 0x8B, 0x2A, 0x4B, 0xCE, 0x59, 0xC4, 0xA6, 0x38, 0x9C, 0x0F, 
    0xA1, 0x23, 0xE1, 0x8F, 0x21, 0x54, 0xF3, 0xBD, 0x60, 0xAC, 0xE6, 0x46, 0x3C, 0xA7, 0x70, 0x88, 
    0xDF, 0xD0, 0x65, 0xE9, 0x33, 0x76, 0x3F, 0x8F, 0xDC, 0x75, 0x86, 0x70, 0x4D, 0xB3, 0x2D, 0xDE, 
    0xA0, 0x6F, 0x9C, 0xDC, 0x54, 0xEB, 0x4F, 0xD1, 0x11, 0x34, 0xFB, 0xE4, 0xC9, 0x1D, 0x60, 0xC8, 
}; 

static const uint8_t signaturebp512[] = {
	0x02, 0x40,
    0x42, 0xCE, 0x7D, 0x18, 0x0E, 0x21, 0xF9, 0xBF, 0x0A, 0xA6, 0x07, 0x2C, 0x87, 0x08, 0xA2, 0x88, 
    0x85, 0x9C, 0x5C, 0x44, 0x4B, 0xB1, 0xB8, 0xD8, 0x67, 0xF0, 0x79, 0xB5, 0x6F, 0x52, 0x1B, 0x01, 
    0x38, 0xE4, 0x8A, 0x4F, 0xA9, 0xB8, 0xF7, 0x06, 0x58, 0x74, 0x19, 0xF5, 0x50, 0xC0, 0xF8, 0xB3, 
    0x39, 0x35, 0x4A, 0x6C, 0x11, 0x67, 0x72, 0xF3, 0xBF, 0x74, 0xB2, 0x50, 0x3E, 0x3A, 0x7B, 0x26, 
	0x02, 0x40,
    0x1E, 0xBD, 0x8C, 0xB8, 0x35, 0xD2, 0xC0, 0x24, 0xE2, 0x13, 0x95, 0x88, 0x44, 0x2C, 0x18, 0x5E, 
    0x26, 0xF6, 0x0F, 0x9F, 0xA9, 0xA1, 0xFD, 0x05, 0x00, 0xD4, 0x58, 0x10, 0xDE, 0x4B, 0x89, 0x57, 
    0x0D, 0x3C, 0xFC, 0x2D, 0xD2, 0x7A, 0xE2, 0xE3, 0xF0, 0x81, 0x5E, 0x19, 0xD8, 0x12, 0xAB, 0x97, 
    0x43, 0x47, 0xBF, 0xE7, 0xDA, 0xE1, 0xF1, 0x39, 0x92, 0x7F, 0x7C, 0x89, 0xF5, 0x51, 0xA3, 0x5B, 
};
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

static optiga_lib_status_t optiga_ecc_generate_keypair(optiga_ecc_curve_t curve_id,
																  void * private_key,
																  uint8_t * public_key,
																  uint16_t * public_key_length)
{
    optiga_lib_status_t return_status = 0;

    optiga_crypt_t * me = NULL;

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
         * 2. Generate ECC Key pair
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage (Key Agreement or Sign based on requirement)
         *       - Store the Private key in OPTIGA Key store
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
		
        return_status = optiga_crypt_ecc_generate_keypair(me,
                                                          curve_id,
                                                          (uint8_t)OPTIGA_KEY_USAGE_SIGN | OPTIGA_KEY_USAGE_KEY_AGREEMENT,
                                                          FALSE,
                                                          (optiga_key_id_t*)private_key,
                                                          public_key,
                                                          public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_ecc_generate_keypair operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Key pair generation failed
            return_status = optiga_lib_status;
            break;
        }
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);

    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
    }
	return return_status;
}

static optiga_lib_status_t optiga_ecdsa_sign(const uint8_t * digest,
														uint8_t digest_length,
														optiga_key_id_t private_key,
														uint8_t * signature,
														uint16_t * signature_length)
{
    //To store the signture generated

    optiga_crypt_t * me = NULL;
    optiga_lib_status_t return_status = 0;

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
         * 2. Sign the digest using Private key from Key Store ID E0F0
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_ecdsa_sign(me,
                                                digest,
                                                digest_length,
                                                private_key,
                                                signature,
                                                signature_length);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_ecdsa_sign operation is completed
			pal_os_timer_delay_in_milliseconds(10);
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            return_status = optiga_lib_status;
			printf("\n%02X", optiga_lib_status);
            break;
        }
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
    }
	return return_status;
}

static optiga_lib_status_t optiga_ecdh(optiga_key_id_t optiga_key_id,
											  public_key_from_host_t * public_key,
											  uint8_t * shared_secret)
{
    optiga_lib_status_t return_status = 0;

    optiga_crypt_t * me = NULL;

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

        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_ecdh(me,
                                          optiga_key_id,
                                          public_key,
                                          TRUE,
                                          shared_secret);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_ecdh operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //ECDH Operation failed.
            return_status = optiga_lib_status;
            break;
        }
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
    }
	return return_status;
}

int example_mbedtls_optiga_crypt_ecc_extendedcurve_genkeypair()
{
    int return_status ;	
    uint8_t loop_count = 0;
	mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "mbedtls trustm example";
	mbedtls_ecp_group grp;
	mbedtls_ecp_point Q;
	mbedtls_mpi d;
	mbedtls_mpi r;
	mbedtls_mpi s;	
	uint8_t *p;	
	const uint8_t * end = NULL;
	
    uint8_t signature [200];
    uint16_t signature_length = sizeof(signature);
	
	 mbedtls_ecp_group_id group_id[] = {MBEDTLS_ECP_DP_SECP521R1, 
										MBEDTLS_ECP_DP_BP256R1,  
										MBEDTLS_ECP_DP_BP384R1, 
										MBEDTLS_ECP_DP_BP512R1};
    const uint8_t * digest_buffer[] = {&digest521[0], 
                                       &digestbp256[0], 
                                       &digestbp384[0], 
                                       &digestbp512[0]};
									   
	size_t digest_buffer_size[] = {sizeof(digest521), 
                                       sizeof(digestbp256), 
                                       sizeof(digestbp384), 
                                       sizeof(digestbp512)};

	memset(&ctr_drbg, 0x00, sizeof(ctr_drbg));
	memset(&entropy, 0x00, sizeof(entropy));
	mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
	
	mbedtls_ecp_group_init(&grp);
	mbedtls_ecp_point_init(&Q);
	mbedtls_mpi_init(&d);
	mbedtls_mpi_init(&r);
	mbedtls_mpi_init(&s);
		
	do{
		
		if( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
								   (const unsigned char *) pers,
								   strlen( pers ) ) != 0 )
		{
			return_status = 1;
			break;
		}
		
		for (loop_count = 0; loop_count < sizeof(group_id)/sizeof(mbedtls_ecp_group_id); loop_count++)
		{
			return_status = 1;
			signature_length = sizeof(signature);
			if(mbedtls_ecp_group_load(&grp, group_id[loop_count]) != 0)
			{
				return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
				break;
			}
				
			//Generate key pair using mbedtls API integrated with optiga libraries
			return_status = mbedtls_ecdh_gen_public(&grp, &d, &Q, mbedtls_ctr_drbg_random, &ctr_drbg);
			if (0 != return_status)
			{
				break;
			}
			
			//Sign operation using optiga libraries
			return_status = optiga_ecdsa_sign(digest_buffer[loop_count], digest_buffer_size[loop_count], 0xE103, signature,	&signature_length);
			if (0 != return_status)
			{
				break;
			}
			
			p = signature;
			end = signature + signature_length;
			
			return_status = mbedtls_asn1_get_mpi(&p, end, &r);
			if (0 != return_status)
			{
				break;
			}
			
			return_status = mbedtls_asn1_get_mpi(&p, end, &s);
			if (0 != return_status)
			{
				break;
			}
			
			//Verify operation using mbedtls API integrated with optiga libraries
			return_status = mbedtls_ecdsa_verify( &grp, digest_buffer[loop_count], digest_buffer_size[loop_count], &Q, &r, &s);
			if(0 != return_status)
			{
				break;
			}
			
			
			mbedtls_ecp_point_free (&Q);
			mbedtls_ecp_group_free (&grp);
			mbedtls_mpi_free(&r);
			mbedtls_mpi_free(&s);
			mbedtls_mpi_free(&d);
		}
	}while(0);
	
	if(0 != return_status)
	{	
		mbedtls_ecp_group_free (&grp);
		mbedtls_ecp_point_free (&Q);
		mbedtls_mpi_free(&r);
		mbedtls_mpi_free(&s);
		mbedtls_mpi_free(&d);
	}
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );
	return return_status;
}

int example_mbedtls_optiga_crypt_ecc_extendedcurve_calsign()
{
    int return_status ;	
    uint8_t loop_count = 0;
	mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "mbedtls trustm example";
	mbedtls_ecp_group grp;
	mbedtls_ecp_point Q;
	mbedtls_mpi d;
	mbedtls_mpi r;
	mbedtls_mpi s;	
	
    uint8_t public_key [150];
    uint16_t public_key_length = sizeof(public_key);	
    optiga_key_id_t optiga_key_id = OPTIGA_KEY_ID_E0F2;
	
	optiga_ecc_curve_t curve_id[] = {OPTIGA_ECC_CURVE_NIST_P_521,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_256R1,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_384R1,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_512R1};
									
	 mbedtls_ecp_group_id group_id[] = {MBEDTLS_ECP_DP_SECP521R1, 
										MBEDTLS_ECP_DP_BP256R1,  
										MBEDTLS_ECP_DP_BP384R1, 
										MBEDTLS_ECP_DP_BP512R1};
    const uint8_t * digest_buffer[] = {&digest521[0], 
                                       &digestbp256[0], 
                                       &digestbp384[0], 
                                       &digestbp512[0]};
									   
	size_t digest_buffer_size[] = {sizeof(digest521), 
                                       sizeof(digestbp256), 
                                       sizeof(digestbp384), 
                                       sizeof(digestbp512)};
									   
	uint16_t public_key_offset[] = {4,3,3,4};

	memset(&ctr_drbg, 0x00, sizeof(ctr_drbg));
	memset(&entropy, 0x00, sizeof(entropy));

	mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
	
	mbedtls_ecp_group_init(&grp);
	mbedtls_ecp_point_init(&Q);
	mbedtls_mpi_init(&d);
	mbedtls_mpi_init(&r);
	mbedtls_mpi_init(&s);
		
	do{
		printf("%04X", optiga_key_id);
		if( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
								   (const unsigned char *) pers,
								   strlen( pers ) ) != 0 )
		{
			return_status = 1;
			break;
		}
		
		for (loop_count = 0; loop_count < sizeof(group_id)/sizeof(mbedtls_ecp_group_id); loop_count++)
		{
			return_status = 1;
			public_key_length = sizeof(public_key);
			if(mbedtls_ecp_group_load(&grp, group_id[loop_count]) != 0)
			{
				return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
				break;
			}
				
			//Generate key pair using optiga libraries
			return_status = optiga_ecc_generate_keypair(curve_id[loop_count],
																  &optiga_key_id,
																  public_key,
																  &public_key_length);
			if (0 != return_status)
			{
				break;
			}
			
			//store public key generated from optiga into mbedtls structure .
			if (mbedtls_ecp_point_read_binary(&grp, &Q, &public_key[public_key_offset[loop_count]], (public_key_length-public_key_offset[loop_count])) != 0)
			{
				return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
				break;
			}
		
			//Sign operation using optiga libraries
			return_status = mbedtls_ecdsa_sign( &grp, &r, &s, &d, digest_buffer[loop_count], digest_buffer_size[loop_count],
                mbedtls_ctr_drbg_random, &ctr_drbg);
			if (0 != return_status)
			{
				break;
			}
			
			//Verify operation using mbedtls API integrated with optiga libraries
			//Note: Make sure to use same private key oid used for calcsign in trust_ecdsa.c file, for generating the key pair also
			return_status = mbedtls_ecdsa_verify( &grp, digest_buffer[loop_count], digest_buffer_size[loop_count], &Q, &r, &s);
			if(0 != return_status)
			{
				break;
			}
			
			
			mbedtls_ecp_point_free (&Q);
			mbedtls_ecp_group_free (&grp);
			mbedtls_mpi_free(&r);
			mbedtls_mpi_free(&s);
			mbedtls_mpi_free(&d);
		}
	}while(0);
	
	if(0 != return_status)
	{	
		mbedtls_ecp_group_free (&grp);
		mbedtls_ecp_point_free (&Q);
		mbedtls_mpi_free(&r);
		mbedtls_mpi_free(&s);
		mbedtls_mpi_free(&d);
	}
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );
	return return_status;
}

int example_mbedtls_optiga_crypt_ecc_extendedcurve_verify()
{
    int return_status;
    uint8_t loop_count = 0;
    mbedtls_ecp_group grp;
	mbedtls_ecp_point Q;
	uint8_t *p;
	mbedtls_mpi r;
	mbedtls_mpi s;
	const uint8_t * end = NULL;
	
    mbedtls_ecp_group_id group_id[] = {MBEDTLS_ECP_DP_SECP521R1, 
										MBEDTLS_ECP_DP_BP256R1,  
										MBEDTLS_ECP_DP_BP384R1, 
										MBEDTLS_ECP_DP_BP512R1};
    const uint8_t * digest_buffer[] = {&digest521[0], 
                                       &digestbp256[0], 
                                       &digestbp384[0], 
                                       &digestbp512[0]};
									   
	size_t digest_buffer_size[] = {sizeof(digest521), 
                                       sizeof(digestbp256), 
                                       sizeof(digestbp384), 
                                       sizeof(digestbp512)};
									   
	const unsigned char * signature_buffer[] = {&signature521[0],
                                          &signaturebp256[0], 
                                          &signaturebp384[0], 
                                          &signaturebp512[0]};
	
	const unsigned char * public_key[] = {&publickey521[0],
                                           &publickeybp256[0], 
                                           &publickeybp384[0], 
                                           &publickeybp512[0]};
										   
	size_t public_key_size[] = {sizeof(publickey521),
                                           sizeof(publickeybp256), 
                                           sizeof(publickeybp384), 
                                           sizeof(publickeybp512)};
										   
    size_t signature_size[] = {sizeof(signature521), 
                             sizeof(signaturebp256), 
                             sizeof(signaturebp384), 
                             sizeof(signaturebp512)};
		
	for (loop_count = 0; loop_count < sizeof(group_id)/sizeof(mbedtls_ecp_group_id); loop_count++)
	{
		return_status = 1;
		mbedtls_ecp_group_init(&grp);
		mbedtls_ecp_point_init(&Q);
		mbedtls_mpi_init(&r);
		mbedtls_mpi_init(&s);
		
		if(mbedtls_ecp_group_load(&grp, group_id[loop_count]) != 0)
		{
			return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
			break;
		}

		//store public key generated from optiga into mbedtls structure .
		if (mbedtls_ecp_point_read_binary(&grp, &Q, public_key[loop_count], public_key_size[loop_count]) != 0)
		{
			return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
			break;
		}
		
		p = signature_buffer[loop_count];
		end = signature_buffer[loop_count] + signature_size[loop_count];
		
		return_status = mbedtls_asn1_get_mpi(&p, end, &r);
		if (0 != return_status)
		{
			break;
		}
		
		return_status = mbedtls_asn1_get_mpi(&p, end, &s);
		if (0 != return_status)
		{
			break;
		}

		return_status = mbedtls_ecdsa_verify( &grp, digest_buffer[loop_count], digest_buffer_size[loop_count], &Q, &r, &s);

		if(0 != return_status)
		{
			printf("Failure");
			break;
		}
		
		mbedtls_ecp_point_free (&Q);
		mbedtls_ecp_group_free (&grp);
		mbedtls_mpi_free(&r);
		mbedtls_mpi_free(&s);
	}
	
	if(0 != return_status)
	{	
		mbedtls_ecp_group_free (&grp);
		mbedtls_ecp_point_free (&Q);
		mbedtls_mpi_free(&r);
		mbedtls_mpi_free(&s);
	}
	return(return_status);
}


int example_mbedtls_optiga_crypt_ecc_extendedcurve_calcssec()
{
    int return_status ;	
    uint8_t loop_count = 0;
	mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "mbedtls trustm example";
	mbedtls_ecp_group grp;
	mbedtls_ecp_point Q1, Q2;
	mbedtls_mpi d;
	mbedtls_mpi z;
	
    uint8_t public_key [150];
    uint16_t public_key_length = sizeof(public_key);
	optiga_key_id_t optiga_key_id = 0xE102;
	
	uint8_t shared_secret_mbedtls_optiga [150];
	uint8_t shared_secret_host[150];
	public_key_from_host_t peer_public_key_details;
	
	uint16_t public_key_offset ;
	
	optiga_ecc_curve_t curve_id[] = {OPTIGA_ECC_CURVE_NIST_P_521,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_256R1,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_384R1,
									OPTIGA_ECC_CURVE_BRAIN_POOL_P_512R1};
									
	 mbedtls_ecp_group_id group_id[] = {MBEDTLS_ECP_DP_SECP521R1, 
										MBEDTLS_ECP_DP_BP256R1,  
										MBEDTLS_ECP_DP_BP384R1, 
										MBEDTLS_ECP_DP_BP512R1};
	
	uint8_t key_offset[] = {4, 3, 3, 4};
	
	memset(&ctr_drbg, 0x00, sizeof(ctr_drbg));
	memset(&entropy, 0x00, sizeof(entropy));
	mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
	
	mbedtls_ecp_group_init(&grp);
	mbedtls_ecp_point_init(&Q1);
	mbedtls_ecp_point_init(&Q2);
	mbedtls_mpi_init(&d);
	mbedtls_mpi_init(&z);
		
	do{
		
		if( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
								   (const unsigned char *) pers,
								   strlen( pers ) ) != 0 )
		{
			return_status = 1;
			break;
		}
		
		for (loop_count = 0; loop_count < sizeof(group_id)/sizeof(mbedtls_ecp_group_id); loop_count++)
		{
			return_status = 1;
			public_key_offset = key_offset[loop_count];
			public_key_length = sizeof(public_key);
			
			if(mbedtls_ecp_group_load(&grp, group_id[loop_count]) != 0)
			{
				return_status = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
				break;
			}
			
			//Generate key pair using  optiga libraries in session E102
			return_status = optiga_ecc_generate_keypair(curve_id[loop_count],
																  &optiga_key_id,
																  public_key,
																  &public_key_length);
			if (0 != return_status)
			{
				break;
			}
			
			//Generate key pair using mbedtls API integrated with optiga libraries in session E103
			return_status = mbedtls_ecdh_gen_public(&grp, &d, &Q1, mbedtls_ctr_drbg_random, &ctr_drbg);
			if (0 != return_status)
			{
				break;
			}
			
			//store public key generated from other party into mbedtls structure .
			if (mbedtls_ecp_point_read_binary(&grp, &Q2, &public_key[public_key_offset], (public_key_length-public_key_offset)) != 0)
			{
				return_status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
				break;
			}
		
			//Calculate shared secret using mbedtls api integrated with host libraries
			return_status = mbedtls_ecdh_compute_shared(&grp, &z, &Q2, &d, mbedtls_ctr_drbg_random, &ctr_drbg);
			if (0 != return_status)
			{
				break;
			}			
			
			//Extract the generated shared secret from mbedtls structure into buffer.
			return_status = mbedtls_mpi_write_binary(&z, shared_secret_mbedtls_optiga, mbedtls_mpi_size( &grp.P ));
			if (0 != return_status)
			{
				break;
			}
			
			//Copy the public key generated into the mbedtls structure
			return_status = mbedtls_ecp_point_write_binary(&grp, &Q1, MBEDTLS_ECP_PF_UNCOMPRESSED, (size_t *)&public_key_length, &public_key[public_key_offset], sizeof(public_key) );
			if (0 != return_status)
			{
				break;
			}
			
			public_key[0] = 0x03;
			public_key[1] = 0x81;
			public_key[public_key_offset - 2] = public_key_length + 1;
			public_key[public_key_offset - 1] = 0x00;
			
			public_key_length += public_key_offset;			
			
			peer_public_key_details.public_key = public_key;
			peer_public_key_details.length = public_key_length;
			peer_public_key_details.key_type = curve_id[loop_count];
			
			optiga_key_id = 0xE102;
			//Calculate shared secret using host libraries
			return_status = optiga_ecdh(optiga_key_id,
											  &peer_public_key_details,
											  shared_secret_host);
			if (0 != return_status)
			{
				break;
			}
											  
			return_status = memcmp(shared_secret_host, shared_secret_mbedtls_optiga, mbedtls_mpi_size( &grp.P ));
			if (0 != return_status)
			{
				break;
			}
			
			printf("Success");
			pal_os_timer_delay_in_milliseconds(10);
			mbedtls_ecp_point_free (&Q1);
			mbedtls_ecp_point_free (&Q2);
			mbedtls_ecp_group_free (&grp);
			mbedtls_mpi_free(&d);
			mbedtls_mpi_free(&z);
		}
	}while(0);
	
	if(0 != return_status)
	{	
		mbedtls_ecp_group_free (&grp);
		mbedtls_ecp_point_free (&Q1);
		mbedtls_ecp_point_free (&Q2);
		mbedtls_mpi_free(&d);
		mbedtls_mpi_free(&z);
	}
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );
	return return_status;
}
