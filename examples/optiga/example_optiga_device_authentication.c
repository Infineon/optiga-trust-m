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
* \file example_optiga_crypt_ecdh.c
*
* \brief    This file provides the example for ECDH operation using #optiga_crypt_ecdh.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga_example.h"

//HOST AND CLIENT DATA OBJECTS, TREATED AS CUSTOMIZED CERTIFICATES
struct host_cert_t{
	uint8_t name[20];
	uint8_t serial_num[32];
	uint8_t public_key[100];
	uint8_t pubkey_size;
	uint8_t signature[100];
	uint8_t gignature_size;
};

struct client_cert_t {
	uint8_t name[20];
	uint8_t serial_num[32];
	uint8_t public_key[100];
	uint8_t pubkey_size;
	uint8_t signature[100];
	uint8_t gignature_size;
};


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

static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

/**
 * The below example demonstrates the complete process
 * for  simplified device authentication use/case.
 *
 * The example can be extended by using complete x509 certificates instead of a customized "cert"
 */
void example_optiga_device_authentication(void)
{
	optiga_lib_status_t return_status = 0;
	uint16_t offset;

	//To store the generated public key as part of Generate key pair
	uint8_t public_key [100];
	uint16_t public_key_length = sizeof(public_key);

	optiga_crypt_t * crypt_me = NULL;
	optiga_util_t * util_me = NULL;
	OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

	struct host_cert_t host_cert = {

	};


	do{
		//create crypt object
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		//create util object
        util_me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == util_me)
        {
            break;
        }

		optiga_lib_status = OPTIGA_LIB_BUSY;


		/* The OPTIGA Trust M contains 4 persistent ECC key objects and 2 RSA.
		*  They are:
		*  E0F0 -> ECC Persistent Key Pair - Used for device key pair -locked by Infineon in Secure Fab-
		*  E0F1 -> ECC Persistent Key Pair - Will be used to emulate HOST
		*  E0F2 -> ECC Persistent Key Pair - Will be used to emulate CLIENT
		*  E0F3 -> ECC Persistent Key Pair - Not used in this example
		*
		*  E0FC -> RSA Persistent Key Pair - Not used in this example
		*  E0FD -> RSA Persistent Key Pair - Not used in this example
		*
		*  OPTIGA Trust M has many locations of user memory that can be individually configured with different security policies.
		*  They are divided in two groups:
		*  -> One group of type 3 objects of size 140
		*  -> Second group of type 2 objects with size of 1500
		*
		*  We will be using two objects of type 3 to store the public key for HOST and Client to facilitate quick access.
		*/

		optiga_key_id_t ca_key_id = OPTIGA_KEY_ID_E0F0;
		optiga_key_id_t host_key_id  = OPTIGA_KEY_ID_E0F1;
		optiga_key_id_t client_key_id = OPTIGA_KEY_ID_E0F2;

		//1.- Generate HOST Key Pair on ObjectID -OID- E0F1
		return_status = optiga_crypt_ecc_generate_keypair(crypt_me,
															OPTIGA_ECC_CURVE_NIST_P_256,
															(uint8_t)OPTIGA_KEY_USAGE_SIGN,
															FALSE,
															&host_key_id,
															public_key,
															&public_key_length);
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

		//2.- We are using the arbitrary object type 3 F1D0 for HOST Public
		uint16_t host_pub_key_oid = 0xF1D0;
		offset = 0x00;

		OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(util_me, OPTIGA_COMMS_NO_PROTECTION);

		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_write_data(util_me,
				                               host_pub_key_oid,
											   OPTIGA_UTIL_ERASE_AND_WRITE,
											   offset,
											   public_key,
											   public_key_length);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_util_write_data operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//writing data to a data object failed.
			return_status = optiga_lib_status;
			break;
		}

		struct host_cert_t host_cert = {
				.name = {"mobile_phone"},
		};

		memcpy(host_cert.public_key, public_key, public_key_length);
		host_cert.pubkey_size = public_key_length;


		//3.- Reseting public_key size used as temporary storage for size of pubkey
		//Changing to CLIENT KEY OID
		public_key_length = sizeof(public_key);
		memset(public_key, 0x00, sizeof(public_key));

		//Generate HOST Key Pair on ObjectID -OID- E0F1
		return_status = optiga_crypt_ecc_generate_keypair(crypt_me,
															OPTIGA_ECC_CURVE_NIST_P_256,
															(uint8_t)OPTIGA_KEY_USAGE_SIGN,
															FALSE,
															&client_key_id,
															public_key,
															&public_key_length);
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

		//4.- We are using the arbitrary object type 3 F1D1 for CLIENT Public
		uint16_t client_pub_key_oid = 0xF1D1;
		offset = 0x00;

		OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(util_me, OPTIGA_COMMS_NO_PROTECTION);

		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_write_data(util_me,
				                               client_pub_key_oid,
											   OPTIGA_UTIL_ERASE_AND_WRITE,
											   offset,
											   public_key,
											   public_key_length);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_util_write_data operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//writing data to a data object failed.
			return_status = optiga_lib_status;
			break;
		}

		struct client_cert_t client_cert = {
				.name = {"client_3d_gatjet"},
		};

		memcpy(client_cert.public_key, public_key, public_key_length);
		client_cert.pubkey_size = public_key_length;



	}while(0);


}


























