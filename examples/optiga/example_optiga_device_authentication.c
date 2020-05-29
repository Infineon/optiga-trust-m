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
	uint8_t serial_num[32];
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

uint8_t ca_public_key [] =
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
            0x8b,0x88,0x9c,0x1d,0xd6,0x07,0x58,0x2e,
            0xd6,0xf8,0x2c,0xc2,0xd9,0xbe,0xd0,0xfe,
            0x64,0xf3,0x24,0x5e,0x94,0x7d,0x54,0xcd,
            0x20,0xdc,0x58,0x98,0xcf,0x51,0x31,0x44,
            0x22,0xea,0x01,0xd4,0x0b,0x23,0xb2,0x45,
            0x7c,0x42,0xdf,0x3c,0xfb,0x0d,0x33,0x10,
            0xb8,0x49,0xb7,0xaa,0x0a,0x85,0xde,0xe7,
            0x6a,0xf1,0xac,0x31,0x31,0x1e,0x8c,0x4b
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
		return_status = OPTIGA_LIB_SUCCESS;

	}while(0);

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

	return return_status;
}

/**
 * Generate Random Number
 */
optiga_lib_status_t generate_random_number(uint8_t * random_num_32)
{
	optiga_lib_status_t return_status = 0;

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
											random_num_32,
											32);

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
		return_status = optiga_crypt_destroy(crypt_me);
		if(OPTIGA_LIB_SUCCESS != return_status)
		{
			//lint --e{774} suppress This is a generic macro
			OPTIGA_EXAMPLE_LOG_STATUS(return_status);
		}
	}

	return return_status;
}

/**
 * Verify challenge
 */
optiga_lib_status_t challenge_verify(uint8_t * pubkey, uint8_t pubkey_size, uint8_t * challenge_digest, uint8_t * signature, uint8_t signature_size)
{
	optiga_lib_status_t return_status = 0;

	public_key_from_host_t public_key_details = {
										pubkey,
										pubkey_size,
										(uint8_t)OPTIGA_ECC_CURVE_NIST_P_256
									};
	optiga_crypt_t * crypt_me = NULL;

	do{
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		//With OPTIGA Trust M we have two options to verify a certificate, OPTIGA_CRYPT_HOST_DATA, OPTIGA_CRYPT_OID_DATA:
		// a) OPTIGA_CRYPT_HOST_DATA: If certificate is customized and not an x509 compliant then user would need to use the basic approach and provide all
		//    elements to verify the signature (public_key, digest, signatture)
		// b) OPTIGA_CRYPT_OID_DATA: If the certificate is x509 complient user can use the OID where the CA certificate is located and verification will be
	    //    done internally as the OPTIGA Trust M is able to parse the certificate directly within.

		return_status = optiga_crypt_ecdsa_verify (crypt_me,
				                                   challenge_digest,
												   32,
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
		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}

/**
 * Sign challenge
 */
optiga_lib_status_t challenge_sign(optiga_key_id_t * optiga_key_id, uint8_t * message_digest, uint8_t * signature, uint16_t * signature_size)
{
	optiga_lib_status_t return_status = 0;

	optiga_crypt_t * crypt_me = NULL;



	do{
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		return_status = optiga_crypt_ecdsa_sign(crypt_me,
				                                message_digest,
												32,
												*optiga_key_id,
												signature,
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

		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}


/**
 * Verify customized certificate
 */
optiga_lib_status_t cert_verify_vs_ca(custom_cert_t * custom_cert, uint8_t * sha256_digest)
{
	optiga_lib_status_t return_status = 0;
    uint8_t digest[32] = {};

	public_key_from_host_t public_key_details = {
									    ca_public_key,
										sizeof(ca_public_key),
										(uint8_t)OPTIGA_ECC_CURVE_NIST_P_256
									};
	optiga_crypt_t * crypt_me = NULL;

	do{
		crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
		if (NULL == crypt_me)
		{
			break;
		}

		//With OPTIGA Trust M we have two options to verify a certificate, OPTIGA_CRYPT_HOST_DATA, OPTIGA_CRYPT_OID_DATA:
		// a) OPTIGA_CRYPT_HOST_DATA: If certificate is customized and not an x509 compliant then user would need to use the basic approach and provide all
		//    elements to verify the signature (public_key, digest, signatture)
		// b) OPTIGA_CRYPT_OID_DATA: If the certificate is x509 complient user can use the OID where the CA certificate is located and verification will be
	    //    done internally as the OPTIGA Trust M is able to parse the certificate directly within.

		return_status = optiga_crypt_ecdsa_verify (crypt_me,
												   digest,
												   sizeof(digest),
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
		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}

/**
 * Sign customized certificate
 */
optiga_lib_status_t cert_sign(custom_cert_t * custom_cert, uint8_t * sha256_digest)
{
    //To store the signture generated
	uint8_t signature [80] = {};
	uint16_t signature_length = sizeof(signature);
    uint8_t digest[32] = {};
	optiga_lib_status_t return_status = 0;

	optiga_crypt_t * crypt_me = NULL;

	crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);


	do{

		if (NULL == crypt_me)
		{
			break;
		}

		return_status = optiga_crypt_ecdsa_sign(crypt_me,
												digest,
												sizeof(digest),
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

		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}


/**
 * Generate HOST Key Pair
 */
optiga_lib_status_t genkey_host(custom_cert_t * custom_cert)
{
	optiga_lib_status_t return_status = 0;
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

		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}




/**
 * Generate CLIENT Key Pair
 */
optiga_lib_status_t genkey_client(custom_cert_t * custom_cert)
{
	optiga_lib_status_t return_status = 0;
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

		return_status = OPTIGA_LIB_SUCCESS;
	}while(0);

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

	return return_status;
}

/**
 * The below example demonstrates the complete process
 * for  simplified device authentication use/case.
 *
 * The example can be extended by using complete x509 certificates instead of a customized "cert"
 */
void example_optiga_device_authentication(void)
{
	optiga_lib_status_t return_status = OPTIGA_CRYPT_ERROR;
	uint8_t digest[32] = {};
	uint8_t host_random_challenge[32] = {};
	uint8_t host_signature_challenge[100] = {};
	uint16_t host_signature_challenge_len = sizeof(host_signature_challenge);
	uint8_t client_random_challenge[32] = {};
	uint8_t client_signature_challenge[100] = {};
	uint16_t client_signature_challenge_len = sizeof(client_signature_challenge);
	uint8_t sha256_digest[32] = {};

	OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

	OPTIGA_EXAMPLE_LOG_MESSAGE("Emulating Provisioning at Infineon");

	do{
		return_status = generate_random_number(host_cert.serial_num);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}


		return_status = generate_random_number(client_cert.serial_num);
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
		return_status = generate_sha256_digest((uint8_t*)&host_cert, sizeof(host_cert), sha256_digest);

		OPTIGA_EXAMPLE_LOG_MESSAGE("Sign Host Certificate using CA");
		return_status = cert_sign(&host_cert, sha256_digest);
		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}
//		//4.- Sign Client customized Cert
//		OPTIGA_EXAMPLE_LOG_MESSAGE("SHA256 Digest of Host Cert");
//		return_status = generate_sha256_digest((uint8_t*)&client_cert, sizeof(client_cert), sha256_digest);
//
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Sign Client Certificate using CA");
//		return_status = cert_sign(&client_cert, sha256_digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Host Customized Certificate");
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Host Name:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)host_cert.name, sizeof(host_cert.name));
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Host Serial Number:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)host_cert.serial_num, sizeof(host_cert.serial_num));
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Host PubKey:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)host_cert.public_key, host_cert.pubkey_size);
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Host Signature:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)host_cert.signature, host_cert.signature_size);
//
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client Customized Certificate");
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client Customized Certificate");
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client Name:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)client_cert.name, sizeof(client_cert.name));
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client Serial Number:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)client_cert.serial_num, sizeof(client_cert.serial_num));
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client PubKey:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)client_cert.public_key, client_cert.pubkey_size);
//		OPTIGA_EXAMPLE_LOG_MESSAGE("Client Signature:");
//		OPTIGA_EXAMPLE_LOG_HEX_DATA((uint8_t *)client_cert.signature, client_cert.signature_size);
//
//		/*
//		 * START MUTUAL AUTHENTICATION BETWEEN HOST AND CLIENT
//		 *
//		 * In a mutual authentication architecture the challenger sends the random challenge
//		 * */
//
//		//5.- Client Sends Certificate to Host and Host Verifies Cert
//		//    Host gets the certificate and starts the verification process
//		// 5.a - Host computes certificate digest using OPTIGA Trust M API
//		return_status = generate_sha256_digest((uint8_t *)&client_cert, sizeof(client_cert), digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 5.b - Host verifies certificate signature using OPTIGA Trust M API
//		return_status = cert_verify_vs_ca(&client_cert, digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 5.c - If Client certificate is successfully verified Host will send a random challenge to validate that
//		//       Client has possession of private key.
//		return_status =  generate_random_number(host_random_challenge);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 5.d - Client receives random challenge and proceeds to sign it and respond to host
//		return_status = generate_sha256_digest(host_random_challenge, sizeof(host_random_challenge), digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		return_status = challenge_sign(&client_key_id, digest, client_signature_challenge, &client_signature_challenge_len);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 5.e - Host Verifies Random Challenge signature using same Public key contained in the Client certificate it just verified
//		return_status = challenge_verify(client_cert.public_key, client_cert.pubkey_size, digest, client_signature_challenge, client_signature_challenge_len);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		/*
//		 * NOW CLIENT AUTHENTICATES HOST
//		 *
//		 * In a mutual authentication architecture the challenger sends the random challenge
//		 * */
//
//		//6.- Host Sends Certificate to Client and Client Verifies Cert
//		//    Client gets the certificate and starts the verification process
//		// 6.a - Client computes certificate digest using OPTIGA Trust M API
//		return_status = generate_sha256_digest((uint8_t *)&host_cert, sizeof(host_cert), digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 6.b - Client verifies certificate signature using OPTIGA Trust M API
//		return_status = cert_verify_vs_ca(&host_cert, digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 6.c - If Host certificate is successfully verified Client will send a random challenge to validate that
//		//       Host has possession of private key.
//		return_status =  generate_random_number(client_random_challenge);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 6.d - Host receives random challenge and proceeds to sign it and respond to host
//		return_status = generate_sha256_digest(client_random_challenge, sizeof(client_random_challenge), digest);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		return_status = challenge_sign(&host_key_id, digest, host_signature_challenge, &host_signature_challenge_len);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}
//
//		// 6.e - Client Verifies Random Challenge signature using same Public key contained in the Host certificate it just verified
//		return_status = challenge_verify(host_cert.public_key, host_cert.pubkey_size, digest, host_signature_challenge, host_signature_challenge_len);
//		if (OPTIGA_LIB_SUCCESS != return_status)
//		{
//			break;
//		}


	}while(0);

}


























