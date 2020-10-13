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
#define FAKE_CLIENT 0X01
#define AUTHENTIC_CLIENT 0X02

#define OPTIGA_CA_PUBKEY_OID		0xF1D0
#define OPTIGA_HOST_PUBKEY_OID 		0xF1D1
#define OPTIGA_CLIENT_PUBKEY_OID	0xF1D2

/**
 * Prepare the hash context
 */
#define OPTIGA_HASH_CONTEXT_INIT(hash_context,p_context_buffer,context_buffer_size,hash_type) \
{                                                               \
    hash_context.context_buffer = p_context_buffer;             \
    hash_context.context_buffer_length = context_buffer_size;   \
    hash_context.hash_algo = hash_type;                         \
}


optiga_key_id_t ca_key_id = OPTIGA_KEY_ID_E0F0;
optiga_key_id_t host_key_id  = OPTIGA_KEY_ID_E0F1;
optiga_key_id_t client_key_id = OPTIGA_KEY_ID_E0F2;

//HOST AND CLIENT DATA OBJECTS, TREATED AS CUSTOMIZED CERTIFICATES
typedef struct {
	uint8_t name[20];
	uint8_t serial_num[10];
	uint8_t public_key[100];
	uint8_t pubkey_size;
	uint8_t signature[100];
	uint8_t signature_size;
}custom_cert_t;

//Assigning the host a name
custom_cert_t host_cert = {
		.name = {"mobile_phone"},
};


//Assigning the client a name
custom_cert_t client_cert = {
		.name = {"client_3d_gatjet"},
};

static uint8_t ca_public_key [] =
{
	//BitString Format
	0x03,
    	//Remaining length
		0x42,
			//unused bits
			0x00,
			//Compression format
			0x04,
			//NIST-256 Public Key
			0x50, 0x1E, 0x23, 0xBC, 0x05, 0x54, 0x0C, 0xCE,
			0xAA, 0x44, 0x07, 0x53, 0x9E, 0x1E, 0xA0, 0x4E,
			0xDE, 0x3A, 0x47, 0x0F, 0xFA, 0x4D, 0x2E, 0x62,
			0x4D, 0x22, 0xC1, 0x35, 0x00, 0x6B, 0xB9, 0x10,
			0x50, 0x11, 0x0E, 0xC7, 0x10, 0x84, 0x89, 0x93,
			0xDA, 0x80, 0xC0, 0x0D, 0xC8, 0x04, 0x3D, 0xD1,
			0xC9, 0x64, 0x6A, 0x4F, 0xD1, 0x73, 0x43, 0x8E,
			0xB5, 0x0B, 0xAB, 0xF7, 0x10, 0x79, 0x16, 0xA4
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
 * GENERATE SHA256 DIGEST
 */
optiga_lib_status_t generate_sha256_digest(uint8_t * message, uint16_t message_len, uint8_t * sha_256_digest)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;
	uint8_t hash_context_buffer [130];
	hash_data_from_host_t hash_data_host;
	optiga_hash_context_t hash_context;

	optiga_crypt_t * crypt_me = NULL;

	do{

		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		OPTIGA_HASH_CONTEXT_INIT(hash_context, hash_context_buffer, sizeof(hash_context_buffer), (uint8_t)OPTIGA_HASH_TYPE_SHA_256);

		optiga_lib_status = OPTIGA_LIB_BUSY;

		return_status = optiga_crypt_hash_start(crypt_me, &hash_context);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_hash_start operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}

		/**
		 * 4. Continue hashing the data
		 */
		hash_data_host.buffer = message;
		hash_data_host.length = message_len;

		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_hash_update(crypt_me,
												 &hash_context,
												 OPTIGA_CRYPT_HOST_DATA,
												 &hash_data_host);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_hash_update operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}

		/**
		 * 5. Finalize the hash
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_hash_finalize(crypt_me,
												   &hash_context,
												   sha_256_digest);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_hash_finalize operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}

		OPTIGA_EXAMPLE_LOG_STATUS(return_status);
	}while(0);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}

/**
 * Generate Random Number
 */
optiga_lib_status_t generate_random_number(uint8_t * random_num, uint16_t random_num_len)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;

	optiga_crypt_t * crypt_me = NULL;
	do
	{
		/**
		 * 1. Create OPTIGA Crypt Instance
		 *
		 */
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		/**
		 * 2. Generate Random -
		 *       - Specify the Random type as TRNG
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;

		return_status = optiga_crypt_random(crypt_me,
											OPTIGA_RNG_TYPE_TRNG,
											random_num,
											random_num_len);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_random operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}
		return_status = OPTIGA_LIB_SUCCESS;

	} while (FALSE);
	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}

/**
 * Verify challenge
 */
optiga_lib_status_t challenge_verify(uint8_t * pubkey, uint8_t pubkey_size, uint8_t * challenge_digest, uint8_t digest_len, uint8_t * signature, uint8_t signature_size)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;

	public_key_from_host_t public_key_details = {
			                                 pubkey,
											 pubkey_size,
											 (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256
											};

	optiga_crypt_t * crypt_me = NULL;
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

		/**
		 * 2. Verify ECDSA signature using public key from host
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_ecdsa_verify (crypt_me,
												   challenge_digest,
												   digest_len,
												   signature,
												   signature_size,
												   OPTIGA_CRYPT_HOST_DATA,
												   &public_key_details);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_ecdsa_verify operation is completed
		}

		if ((OPTIGA_LIB_SUCCESS != optiga_lib_status))
		{
			//Signature verification failed.
			return_status = optiga_lib_status;
			break;
		}

	} while (FALSE);
	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}

/**
 * Sign challenge
 */
optiga_lib_status_t challenge_sign(optiga_key_id_t optiga_key_id, uint8_t * message_digest, uint8_t digest_len, uint8_t * challenge_signature, uint16_t * signature_size)
{
	optiga_crypt_t * crypt_me = NULL;
	optiga_lib_status_t crypt_me_return_status = 0;
	optiga_lib_status_t return_status = 0;

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

		/**
		 * 2. Sign the digest using Private key from Key Store ID E0F0
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_ecdsa_sign(crypt_me,
												message_digest,
												digest_len,
												optiga_key_id,
												challenge_signature,
												signature_size);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_ecdsa_sign operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}

	} while (FALSE);


	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}
	return return_status;
}


/**
 * Verify customized certificate
 */
optiga_lib_status_t cert_verify_vs_ca(custom_cert_t * custom_cert, uint8_t * sha256_digest)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;

	public_key_from_host_t public_key_details = {
			                             ca_public_key,
										 sizeof(ca_public_key),
										 (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256
										};

	optiga_crypt_t * crypt_me = NULL;
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

		/**
		 * 2. Verify ECDSA signature using public key from host
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_ecdsa_verify (crypt_me,
				                                   sha256_digest,
												   32,
												   custom_cert->signature,
												   custom_cert->signature_size,
												   OPTIGA_CRYPT_HOST_DATA,
												   &public_key_details);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_ecdsa_verify operation is completed
		}

		if ((OPTIGA_LIB_SUCCESS != optiga_lib_status))
		{
			//Signature verification failed.
			return_status = optiga_lib_status;
			break;
		}

	} while (FALSE);
	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}

/**
 * Sign customized certificate
 */
optiga_lib_status_t cert_sign(custom_cert_t * custom_cert, uint8_t * sha256_digest)
{

	//To store the signture generated
	uint8_t signature [80];
	uint16_t signature_length = sizeof(signature);

	optiga_crypt_t * crypt_me = NULL;
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;

	OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

	do
	{
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_crypt_ecdsa_sign(crypt_me,
				                                sha256_digest,
												32,
												ca_key_id,
												signature,
												&signature_length);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_crypt_ecdsa_sign operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			return_status = optiga_lib_status;
			break;
		}

		memcpy(custom_cert->signature, signature, signature_length);
		custom_cert->signature_size = signature_length;

	} while (FALSE);
	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}


/**
 * Generate HOST Key Pair
 */
optiga_lib_status_t genkey_host(custom_cert_t * custom_cert)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;
	optiga_lib_status_t util_me_return_status = 0;
	uint16_t offset;

	//To store the generated public key as part of Generate key pair
	uint8_t public_key [100]= {};
	uint16_t public_key_length = sizeof(public_key);

	optiga_crypt_t * crypt_me = NULL;
	optiga_util_t * util_me = NULL;

	do{

        //Create OPTIGA Crypt Instance
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		 //Create OPTIGA Util Instance
		util_me = optiga_util_create(0, optiga_util_callback, NULL);
		if (NULL == util_me)
		{
			break;
		}

		optiga_lib_status = OPTIGA_LIB_BUSY;
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

		OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(util_me, OPTIGA_COMMS_NO_PROTECTION);
		offset = 0x00;
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_write_data(util_me,
											   OPTIGA_HOST_PUBKEY_OID,
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

		memcpy(custom_cert->public_key, public_key, public_key_length);
		custom_cert->pubkey_size = public_key_length;

	}while(0);
	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

    if (util_me)
    {
        //Destroy the instance after the completion of usecase if not required.
    	util_me_return_status = optiga_util_destroy(util_me);
        if(OPTIGA_LIB_SUCCESS != util_me_return_status)
        {
        	return_status = util_me_return_status;
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }

	return return_status;
}




/**
 * Generate CLIENT Key Pair
 */
optiga_lib_status_t genkey_client(custom_cert_t * custom_cert)
{
	optiga_lib_status_t return_status = 0;
	optiga_lib_status_t crypt_me_return_status = 0;
	optiga_lib_status_t util_me_return_status = 0;
	uint16_t offset;

	//To store the generated public key as part of Generate key pair
	uint8_t public_key [100]= {};
	uint16_t public_key_length = sizeof(public_key);

	optiga_crypt_t * crypt_me = NULL;
	optiga_util_t * util_me = NULL;

	do{

        //Create OPTIGA Crypt Instance
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		 //Create OPTIGA Util Instance
		util_me = optiga_util_create(0, optiga_util_callback, NULL);
		if (NULL == util_me)
		{
			break;
		}

		optiga_lib_status = OPTIGA_LIB_BUSY;
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

		OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(util_me, OPTIGA_COMMS_NO_PROTECTION);
		offset = 0x00;
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_write_data(util_me,
											   OPTIGA_CLIENT_PUBKEY_OID,
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

		memcpy(custom_cert->public_key, public_key, public_key_length);
		custom_cert->pubkey_size = public_key_length;

	}while(0);

	OPTIGA_EXAMPLE_LOG_STATUS(return_status);

	if (crypt_me)
	{
		//Destroy the instance after the completion of usecase if not required.
		crypt_me_return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != crypt_me_return_status)
		{
			return_status = crypt_me_return_status;
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

    if (util_me)
    {
        //Destroy the instance after the completion of usecase if not required.
    	util_me_return_status = optiga_util_destroy(util_me);
        if(OPTIGA_LIB_SUCCESS != util_me_return_status)
        {
        	return_status = util_me_return_status;
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }

	return return_status;
}

/**
 * The below example demonstrates the complete process
 * for  simplified device authentication use/case.
 *
 * The example can be extended by using complete x509 certificates instead of a customized "cert"
 */
void example_optiga_device_authentication(uint8_t test_device)
{
	optiga_lib_status_t return_status = OPTIGA_CRYPT_ERROR;
	uint8_t sprintf_buf[1500] = {};
	uint8_t digest[32] = {};
	uint8_t host_random_challenge[32] = {};
	uint8_t host_signature_challenge[80] = {};
	uint16_t host_signature_challenge_len = sizeof(host_signature_challenge);
	uint8_t client_random_challenge[32] = {};
	uint8_t client_signature_challenge[80] = {};
	uint16_t client_signature_challenge_len = sizeof(client_signature_challenge);
	uint8_t sha256_digest[32] = {};

	OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

	OPTIGA_EXAMPLE_LOG_MESSAGE("Emulating Provisioning at Infineon");

	do{

		sprintf((char*)host_cert.name, "Mobile_Phone");
		return_status = generate_random_number(host_cert.serial_num, sizeof(host_cert.serial_num));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		sprintf((char*)client_cert.name, "Client_Display");
		return_status = generate_random_number(client_cert.serial_num, sizeof(host_cert.serial_num));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		//1.- Generate HOST Key Pair on ObjectID -OID- E0F1
		OPTIGA_EXAMPLE_LOG_MESSAGE("Generate HOST Key Pair on ObjectID -OID- E0F1");
		return_status = genkey_host(&host_cert);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		//2.- Generate CLIENT Key Pair on ObjectID -OID- E0F2
		OPTIGA_EXAMPLE_LOG_MESSAGE("Generate HOST Key Pair on ObjectID -OID- E0F2");
		return_status = genkey_client(&client_cert);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		//3.- Sign Host customized Cert
		OPTIGA_EXAMPLE_LOG_MESSAGE("SHA256 Digest of Host Cert");
		return_status = generate_sha256_digest((uint8_t*)&host_cert, (sizeof(host_cert) - sizeof(host_cert.signature) - sizeof(host_cert.signature_size)), sha256_digest);

		optiga_lib_print_message("HOST CERTIFICATE SHA256 DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(sha256_digest, sizeof(sha256_digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		OPTIGA_EXAMPLE_LOG_MESSAGE("Sign Host Certificate using CA");
		return_status = cert_sign(&host_cert, sha256_digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		//4.- Sign Client customized Cert
		OPTIGA_EXAMPLE_LOG_MESSAGE("SHA256 Digest of Host Cert");
		return_status = generate_sha256_digest((uint8_t*)&client_cert, (sizeof(client_cert) - sizeof(client_cert.signature) - sizeof(client_cert.signature_size)), sha256_digest);

		optiga_lib_print_message("CLIENT CERTIFICATE SHA256 DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(sha256_digest, sizeof(sha256_digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		OPTIGA_EXAMPLE_LOG_MESSAGE("Sign Client Certificate using CA");
		return_status = cert_sign(&client_cert, sha256_digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		optiga_lib_print_message("HOST CERTIFICATE", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		sprintf((char *)sprintf_buf, "HOST NAME: %s", host_cert.name);
		optiga_lib_print_message((char *)sprintf_buf,"",OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("HOST SERIAL NUMBER:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_array_hex_format(host_cert.serial_num, 10, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("HOST PUBLIC KEY:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		sprintf((char *)sprintf_buf, "%s", host_cert.public_key);
		optiga_lib_print_array_hex_format(host_cert.public_key, host_cert.pubkey_size, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("HOST SIGNATURE:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_array_hex_format(host_cert.signature, host_cert.signature_size, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("CLIENT CERTIFICATE", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		sprintf((char *)sprintf_buf, "CLIENT NAME: %s", client_cert.name);
		optiga_lib_print_message((char *)sprintf_buf,"",OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("CLIENT SERIAL NUMBER:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_array_hex_format(client_cert.serial_num, 10, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("CLIENT PUBLIC KEY:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		sprintf((char *)sprintf_buf, "%s", client_cert.public_key);
		optiga_lib_print_array_hex_format(client_cert.public_key, client_cert.pubkey_size, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

		optiga_lib_print_message("CLIENT SIGNATURE:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_array_hex_format(client_cert.signature, client_cert.signature_size, OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);


		/*
		 * START MUTUAL AUTHENTICATION BETWEEN HOST AND CLIENT
		 *
		 * In a mutual authentication architecture the challenger sends the random challenge
		 * */

		//5.- Client Sends Certificate to Host and Host Verifies Cert
		//    Host gets the certificate and starts the verification process
		// 5.a - Host computes certificate digest using OPTIGA Trust M API
		return_status = generate_sha256_digest((uint8_t *)&client_cert, (sizeof(client_cert) - sizeof(client_cert.signature) - sizeof(client_cert.signature_size)), digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}


		optiga_lib_print_message("", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("START MUTUAL AUTHENTICATION BETWEEN HOST AND CLIENT:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_message("HOST AUTHENTICATES CLIENT:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_message("CA PUBLIC KEY:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(ca_public_key, sizeof(ca_public_key), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("CLIENT CERTIFICATE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format((uint8_t*)&client_cert, sizeof(client_cert), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("CLIENT CERTIFICATE SHA256 DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(digest, sizeof(digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("HOST VERIFIES CLIENT CERTIFICATE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		// 5.b - Host verifies certificate signature using OPTIGA Trust M API
		return_status = cert_verify_vs_ca(&client_cert, digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		// 5.c - If Client certificate is successfully verified Host will send a random challenge to validate that
		//       Client has possession of private key.
		optiga_lib_print_message("HOST GENERATES RANDOM CHALLENGE AND SENDS IT TO CLIENT:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status =  generate_random_number(host_random_challenge, sizeof(host_random_challenge));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(host_random_challenge, sizeof(host_random_challenge), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		// 5.d - Client receives random challenge and proceeds to sign it and respond to host
		optiga_lib_print_message("CLIENT GENERATES SHA256 DIGEST OF RANDOM CHALLENGE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status = generate_sha256_digest(host_random_challenge, sizeof(host_random_challenge), digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(digest, sizeof(digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		optiga_lib_print_message("CLIENT SIGNS RANDOM CHALLENGE DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status = challenge_sign(client_key_id, digest, sizeof(digest), client_signature_challenge, &client_signature_challenge_len);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(client_signature_challenge, client_signature_challenge_len, OPTIGA_LIB_LOGGER_COLOR_CYAN);

		// 5.e - Host Verifies Random Challenge signature using same Public key contained in the Client certificate it just verified
		optiga_lib_print_message("HOST VERIFIES THE RANDOM CHALLENGE SIGNATURE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		if(test_device == FAKE_CLIENT)
		{
			optiga_lib_print_message("FAKE DEVICE TEST ENABLED--- WILL CHANGE ONE BYTE FROM RANDOM CHALLENGE DIGEST TO 0xAA -----:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
			digest[30] = 0xAA;
		}
		return_status = challenge_verify(client_cert.public_key, client_cert.pubkey_size, digest, sizeof(digest), client_signature_challenge, client_signature_challenge_len);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		/*
		 * NOW CLIENT AUTHENTICATES HOST
		 *
		 * In a mutual authentication architecture the challenger sends the random challenge
		 * */

		//6.- Host Sends Certificate to Client and Client Verifies Cert
		//    Client gets the certificate and starts the verification process
		// 6.a - Client computes certificate digest using OPTIGA Trust M API
		return_status = generate_sha256_digest((uint8_t *)&host_cert, (sizeof(host_cert) - sizeof(host_cert.signature) - sizeof(host_cert.signature_size)), digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		optiga_lib_print_message("", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("NOW CLIENT AUTHENTICATES HOST:", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
		optiga_lib_print_message("CA PUBLIC KEY:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(ca_public_key, sizeof(ca_public_key), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("HOST CERTIFICATE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format((uint8_t*)&host_cert, sizeof(host_cert), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("HOST CERTIFICATE SHA256 DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_array_hex_format(digest, sizeof(digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);
		optiga_lib_print_message("CLIENT VERIFIES CLIENT CERTIFICATE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		// 5.b - Host verifies certificate signature using OPTIGA Trust M API
		return_status = cert_verify_vs_ca(&host_cert, digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		// 6.b - Client verifies certificate signature using OPTIGA Trust M API
		return_status = cert_verify_vs_ca(&host_cert, digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		// 6.c - If Host certificate is successfully verified Client will send a random challenge to validate that
		//       Host has possession of private key.
		optiga_lib_print_message("CLIENT GENERATES RANDOM CHALLENGE AND SENDS IT TO HOST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status =  generate_random_number(client_random_challenge, sizeof(client_random_challenge));
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(client_random_challenge, sizeof(client_random_challenge), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		// 6.d - Host receives random challenge and proceeds to sign it and respond to host
		optiga_lib_print_message("HOST GENERATES SHA256 DIGEST OF RANDOM CHALLENGE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status = generate_sha256_digest(client_random_challenge, sizeof(client_random_challenge), digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(digest, sizeof(digest), OPTIGA_LIB_LOGGER_COLOR_CYAN);

		optiga_lib_print_message("HOST SIGNS RANDOM CHALLENGE DIGEST:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);
		return_status = challenge_sign(host_key_id, digest, sizeof(digest), host_signature_challenge, &host_signature_challenge_len);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
		optiga_lib_print_array_hex_format(host_signature_challenge, host_signature_challenge_len, OPTIGA_LIB_LOGGER_COLOR_CYAN);

		// 6.e - Client Verifies Random Challenge signature using same Public key contained in the Host certificate it just verified
		optiga_lib_print_message("CLIENT VERIFIES THE RANDOM CHALLENGE SIGNATURE:", "", OPTIGA_LIB_LOGGER_COLOR_CYAN);

		return_status = challenge_verify(host_cert.public_key, host_cert.pubkey_size, digest, sizeof(digest), host_signature_challenge, host_signature_challenge_len);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		optiga_lib_print_message("MUTUAL AUTHENTICATION WAS SUCCESFUL. NOW INTERACTIOIN BETWEEN HOST AND CLIENT IS ALLOWED", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);

	}while(0);

	if(OPTIGA_LIB_SUCCESS != return_status)
	{
		optiga_lib_print_message("MUTUAL AUTHENTICATION FAILED. NO INTERACTIOIN BETWEEN HOST AND CLIENT IS ALLOWED", "", OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA);
	}
}


























