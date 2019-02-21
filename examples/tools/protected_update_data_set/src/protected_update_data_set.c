
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
* \file protected_update_data_set.c
*
* \brief   This file implements APIs, types and data structures used for protected update data set creation.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include "protected_update_data_set.h"
#include "pal\pal_crypt.h"
#include "pal\pal_logger.h"
#include "cbor.h"




#define PROTECT_UPDATE_SHA256_LENGTH				(0x20)
#define PROTECT_UPDATE_MANIFEST_VERSION				(1U)

// This will be replaced by code
const unsigned char ecc_signature_header[] = { 0x84, 0x4a, 0x53, 0x69, 0x67, 0x6e, 0x61, 0x74, 0x75, 0x72, 0x65, 0x31, 0x43, 0xa1, 0x01, 0x26, 0x40 };
const unsigned char rsa_signature_header[] = { 0x84, 0x4a, 0x53, 0x69, 0x67, 0x6e, 0x61, 0x74, 0x75, 0x72, 0x65, 0x31, 0x47, 0xa1, 0x01, 0x3a, 0x00, 0x01, 0x00, 0xa3, 0x40 };

unsigned char local_manifest_buffer[200];

unsigned char ES_256[] = { 0x26 };
unsigned char RSA_SSA_PKCS1_V1_5_SHA_256[] = { 0x3A,0x00, 0x01, 0x00, 0xA3};

//Decodes RSA signature compoenents
static int protected_update_decode_ecc_signature(	unsigned char * in_signature,
													unsigned char * out_signature)
{
	unsigned char offset = 3;
	unsigned char round = 0;
	unsigned char out_index = 0;
#define PC_ECC_SIGN_LENGTH		(0x20)
	while (round < 2)
	{
		if (*(in_signature + offset++) == 0x21)
		{
			offset++;
		}
		memcpy(	out_signature + out_index,
				in_signature + offset,
				PC_ECC_SIGN_LENGTH);
		offset += (PC_ECC_SIGN_LENGTH + 1);
		out_index += PC_ECC_SIGN_LENGTH;
		round++;
	} ;
#undef PC_ECC_SIGN_LENGTH

	return 0;
}


int protected_update_create_manifest(	manifest_d manifest_data, 
										protected_update_data_set_d * p_cbor_manifest)
{
	int status = -1;
	unsigned char digest[PROTECT_UPDATE_SHA256_LENGTH];

	unsigned char signature[256];
	unsigned short signature_length;

	unsigned char data_to_sign[200];
	unsigned short data_to_sign_length;

	unsigned char extracted_signature[0x50];
	unsigned payload_length_for_digest;

	unsigned char sign_algo_length;
	unsigned char * sign_algo_ptr = NULL;
	unsigned char * sign_header_ptr = NULL;
	unsigned char sign_header_length = 0;
	unsigned short offset = 0;
	unsigned char length_marker = 0;
	unsigned char sign_info_length;
	unsigned short hash_algo;
	
	do
	{
		// get details of sign algo
		if (strcmp(manifest_data.signature_algo, "ES_256") == 0)
		{
			sign_algo_length = sizeof(ES_256);
			sign_algo_ptr = ES_256;
			sign_info_length = (2 * PROTECT_UPDATE_SHA256_LENGTH);
			sign_header_ptr = (unsigned char *)ecc_signature_header;
			sign_header_length = sizeof(ecc_signature_header);
		}
		else
		{
			sign_algo_length = sizeof(RSA_SSA_PKCS1_V1_5_SHA_256);
			sign_algo_ptr = RSA_SSA_PKCS1_V1_5_SHA_256;
			sign_info_length = (4 * PROTECT_UPDATE_SHA256_LENGTH);
			sign_header_ptr = (unsigned char *)rsa_signature_header;
			sign_header_length = sizeof(rsa_signature_header);
		}

		if (strcmp(manifest_data.digest_algo, "SHA256") == 0)
		{
			hash_algo = 0x29;
		}

		// 1.COSE
		cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

		// 2.protected signed header trust
		cbor_set_byte_string(local_manifest_buffer, (sign_algo_length + 2), &offset);
		local_manifest_buffer[offset++] = 0xA1;
		local_manifest_buffer[offset++] = 0x01;
		memcpy(local_manifest_buffer + offset, sign_algo_ptr, sign_algo_length);
		offset += sign_algo_length;

		// 3.unprotected -signed header Trust
		local_manifest_buffer[offset++] = 0xA1;
		local_manifest_buffer[offset++] = 0x04;
		cbor_set_byte_string(local_manifest_buffer, sizeof(manifest_data.trust_anchor_oid), &offset);
		local_manifest_buffer[offset++] = (unsigned char)((0xFF00 & manifest_data.trust_anchor_oid) >> 8);
		local_manifest_buffer[offset++] = (unsigned char)manifest_data.trust_anchor_oid;

		// 4.Payload
		length_marker = (unsigned char)offset;
		//length to be improved
		cbor_set_byte_string(local_manifest_buffer, 0x3d, &offset);

		// 4.1 Trust manifest
		cbor_set_array_of_data(local_manifest_buffer, 6, &offset);

		// manifest version
		cbor_set_unsigned_integer(local_manifest_buffer, PROTECT_UPDATE_MANIFEST_VERSION, &offset);
		cbor_set_null(local_manifest_buffer, &offset);
		cbor_set_null(local_manifest_buffer, &offset);
		cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

		local_manifest_buffer[offset++] = CBOR_MAJOR_TYPE_1;
		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data.payload_length, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for payload_length");
			break;
		}
		//payload version
		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data.payload_version, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for payload_version");
			break;
		}

		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		//offset
		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data.offset_in_oid, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for offset_in_oid");
			break;
		}
		//write type
		local_manifest_buffer[offset++] = (unsigned char)manifest_data.write_type;

		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		local_manifest_buffer[offset++] = CBOR_MAJOR_TYPE_1;
		cbor_set_byte_string(local_manifest_buffer, PROTECT_UPDATE_SHA256_LENGTH + 5, &offset);

		//digest fields 
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);

		local_manifest_buffer[offset++] = CBOR_ADDITIONAL_TYPE_0x18;
		//creating digest

		payload_length_for_digest = (manifest_data.payload_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : manifest_data.payload_length;
		if (0 != pal_crypt_calculate_sha256_hash(	(unsigned char *)manifest_data.payload, 
													payload_length_for_digest - PROTECT_UPDATE_SHA256_LENGTH, 
													digest))
		{
			pal_logger_print_message(" FAILED :  protect_update : pal_crypt_calculate_sha256_hash");
			break;
		}

		local_manifest_buffer[offset++] = (unsigned char)hash_algo;
		cbor_set_byte_string(local_manifest_buffer, sizeof(digest), &offset);
		memcpy((local_manifest_buffer+offset), digest, sizeof(digest));
		offset += sizeof(digest);
		cbor_set_null(local_manifest_buffer, &offset);

		//target oid
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		cbor_set_byte_string(local_manifest_buffer, 0, &offset);
		cbor_set_byte_string(local_manifest_buffer, 2, &offset);
		local_manifest_buffer[offset++] = (unsigned char)(manifest_data.target_oid >> 8);
		local_manifest_buffer[offset++] = (unsigned char)manifest_data.target_oid;

		//Updating length filed of payload
		data_to_sign_length = offset - length_marker - 1;
		local_manifest_buffer[length_marker + 1] = data_to_sign_length - 1;

		//signature
		cbor_set_byte_string(local_manifest_buffer, sign_info_length, &offset);

		//Generate signature
		memcpy(data_to_sign, sign_header_ptr, sign_header_length);
		data_to_sign_length += 1;
		memcpy(data_to_sign + sign_header_length, local_manifest_buffer + length_marker, data_to_sign_length);
		data_to_sign_length += sign_header_length;

		signature_length = sizeof(signature);
		if(0 != pal_crypt_calculate_signature(	data_to_sign,
									data_to_sign_length,
									(unsigned char *)manifest_data.private_key,
									signature,
									&signature_length,
									0))
		{
			pal_logger_print_message(" FAILED :  protect_update : pal_crypt_calculate_signature");
            break;
        }
		// DEBUG prints
		//pal_logger_print_hex_data(signature, signature_length);
        
		signature_length = sign_info_length;
		// do only for EC_256
		if (1 == sign_algo_length)
		{ 
			status = protected_update_decode_ecc_signature(signature, extracted_signature);
			if (0 != status)
			{
				pal_logger_print_message(" FAILED :  protect_update : protected_update_signature_parser");
				break;
			}
			// DEBUG prints
			//pal_logger_print_message("\n");
			//pal_logger_print_hex_data(extracted_signature, sign_info_length);
			//pal_logger_print_message("\n");
		    memcpy(&local_manifest_buffer[offset], extracted_signature, signature_length);
		}
        else
        {
            memcpy(&local_manifest_buffer[offset], signature, signature_length);
        }
		offset += signature_length;
		p_cbor_manifest->data_length = offset;
		p_cbor_manifest->data = malloc(offset);
		memcpy(p_cbor_manifest->data, local_manifest_buffer, offset);

		status = 0;
	} while (0);
	return status;

}

int protected_update_create_fragments(	manifest_d manifest_data, 
										protected_update_data_set_d * p_cbor_manifest)
{
	int status = -1;
	unsigned char digest[PROTECT_UPDATE_SHA256_LENGTH];
	unsigned short remaining_payload_length = manifest_data.payload_length;
	unsigned short max_memory_required;
	unsigned char * fragments = NULL;
	unsigned char count_of_fragments = 0;
	unsigned char * p_current_fragment = NULL;
	unsigned short length_to_digest;

	do
	{

		max_memory_required = (remaining_payload_length / MAX_PAYLOAD_FRAGMENT_SIZE) * MAX_PAYLOAD_SIZE;
		max_memory_required += (remaining_payload_length % MAX_PAYLOAD_FRAGMENT_SIZE);
		fragments = malloc(max_memory_required);
		
		// DEBUG prints
		//pal_logger_print_hex_data(manifest_data.payload, manifest_data.payload_length); pal_logger_print_message(""); 
		//pal_logger_print_message("Total length : %d\n", manifest_data.payload_length);
		//pal_logger_print_message("max_memory_required : %d\n", max_memory_required);
		//pal_logger_print_message("expected number of fragments: %d\n", (remaining_payload_length / MAX_PAYLOAD_FRAGMENT_SIZE));

		do
		{
			p_current_fragment = fragments + (count_of_fragments * MAX_PAYLOAD_SIZE);
			if (remaining_payload_length > MAX_PAYLOAD_SIZE)
			{
				// copy
				memcpy(	p_current_fragment, 
						manifest_data.payload + (MAX_PAYLOAD_FRAGMENT_SIZE * count_of_fragments),
						MAX_PAYLOAD_FRAGMENT_SIZE);

				remaining_payload_length -= MAX_PAYLOAD_FRAGMENT_SIZE;

				//calculate hash
                length_to_digest = (remaining_payload_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_FRAGMENT_SIZE : remaining_payload_length;
				count_of_fragments++;
				pal_crypt_calculate_sha256_hash(	(unsigned char *)manifest_data.payload + (MAX_PAYLOAD_FRAGMENT_SIZE * count_of_fragments),
										length_to_digest,
										digest);
				// copy hash
				memcpy(	p_current_fragment + MAX_PAYLOAD_FRAGMENT_SIZE,
						digest,
						sizeof(digest));
				//pal_logger_print_hex_data(p_current_fragment, MAX_PAYLOAD_SIZE);
				p_cbor_manifest->fragments_length += MAX_PAYLOAD_SIZE;
			}
			else
			{
				memcpy(	p_current_fragment,
						manifest_data.payload + (MAX_PAYLOAD_FRAGMENT_SIZE * count_of_fragments),
						remaining_payload_length);
				p_cbor_manifest->fragments_length += remaining_payload_length;
				//pal_logger_print_hex_data(p_current_fragment, remaining_payload_length); 
				remaining_payload_length = 0;
				status = 0;
			}
		} while (remaining_payload_length != 0);

		if (0 == status)
		{
			p_cbor_manifest->fragments = fragments;
			// below variable can be deleted
			p_cbor_manifest->actual_memory_allocated = max_memory_required;

		}

	} while (0);
	return status;
}

/**
* @}
*/