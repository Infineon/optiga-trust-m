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

#define PROTECT_UPDATE_MANIFEST_VERSION				(1U)

// This will be replaced by code
const char signature_string[] = "Signature1";
unsigned char signature_header[25];

unsigned char local_manifest_buffer[500];

//Decodes ECC signature compoenents
static int protected_update_decode_ecc_signature(	unsigned char * in_signature,
													unsigned char * out_signature,
													unsigned short sign_len)
{
	unsigned char offset = 3;
	unsigned char round = 0;
	unsigned char out_index = 0;
	unsigned short length_to_copy = 0;
	unsigned short r_s_comp_length = sign_len/2;
	
	while (round < 2)
	{
		// if compoenents are negative
		if (*(in_signature + offset) == (r_s_comp_length + 1))
		{
			length_to_copy = r_s_comp_length;
			offset+=2;
		}
		// if compoenents are less than key size
		else if (*(in_signature + offset) < r_s_comp_length)
		{
			length_to_copy = *(in_signature + offset);
			offset++;
			out_index += (r_s_comp_length - length_to_copy);
		}
		else
		{
			offset++;
			length_to_copy = r_s_comp_length;
		}

		memcpy(	out_signature + out_index,
				in_signature + offset,
				length_to_copy);

		offset += (length_to_copy + 1);
		out_index += length_to_copy;
		round++;
	} ;

	return 0;
}

int protected_update_create_manifest(	manifest_t * manifest_data, 
										protected_update_data_set_d * p_cbor_manifest)
{
	int status = -1;
	unsigned char digest[PROTECT_UPDATE_SHA256_LENGTH];

	unsigned char signature[256];
	unsigned short signature_length;

	unsigned char data_to_sign[300];
	unsigned short data_to_sign_length;

	unsigned char extracted_signature[256];
	unsigned payload_length_for_digest;

	unsigned short sign_algo_length;
	unsigned char * sign_header_ptr = NULL;
	unsigned short sign_header_length = 0;
	unsigned short offset = 0;
	unsigned short length_marker = 0;
	unsigned short byte_string_len_marker = 0;
	signed int sign_algo_value;
	unsigned char digest_algo_value;
	
	do
	{
		cbor_set_array_of_data(signature_header, 4, &sign_header_length);
		cbor_set_byte_string(signature_header, strlen(signature_string), &sign_header_length);
		memcpy(signature_header+sign_header_length, signature_string, strlen(signature_string));
		sign_header_length = sign_header_length + strlen(signature_string);

		// get details of sign algo
		if (strcmp(manifest_data->signature_algo, "ES_256") == 0)
		{
			sign_algo_value = (signed int)eES_SHA;
		}
		else if (strcmp(manifest_data->signature_algo, "RSA-SSA-PKCS1-V1_5-SHA-256") == 0)
		{
			sign_algo_value = (signed int)eRSA_SSA_PKCS1_V1_5_SHA_256;
		}
		else 
		{
			pal_logger_print_message(" FAILED :  protect_update : sign algo assignment");
			break;
		}

		sign_header_length++;
		sign_algo_length = sign_header_length;
		cbor_set_map_tag(signature_header, 0x01, &sign_header_length);
		cbor_set_map_signed_type(signature_header, 0x01, sign_algo_value, &sign_header_length);
		sign_algo_length = (sign_header_length - sign_algo_length);
		sign_header_length = sign_header_length - sign_algo_length - 1;
		cbor_set_byte_string(signature_header, sign_algo_length, &sign_header_length);
		sign_header_length = sign_header_length + sign_algo_length;
		cbor_set_byte_string(signature_header, 0x00, &sign_header_length);
		sign_header_ptr = (unsigned char *)signature_header;

		// 1.COSE
		cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

		// 2.protected signed header trust
		cbor_set_byte_string(local_manifest_buffer, (sign_algo_length), &offset);
		cbor_set_map_tag(local_manifest_buffer, 0x01, &offset);
		cbor_set_map_signed_type(local_manifest_buffer, 0x01, sign_algo_value, &offset);

		// 3.unprotected -signed header Trust
		cbor_set_map_tag(local_manifest_buffer, 0x01, &offset);
		cbor_set_map_byte_string_type(local_manifest_buffer, 0x04, (unsigned char *)&manifest_data->trust_anchor_oid, 0x02, &offset);

		// 4.Payload
		length_marker = (unsigned char)offset;
		//length to be improved
		cbor_set_byte_string(local_manifest_buffer, 0x3d, &offset);

		// 4.1 Trust manifest
		cbor_set_array_of_data(local_manifest_buffer, 0x06, &offset);

		// manifest version
		cbor_set_unsigned_integer(local_manifest_buffer, PROTECT_UPDATE_MANIFEST_VERSION, &offset);
		cbor_set_null(local_manifest_buffer, &offset);
		cbor_set_null(local_manifest_buffer, &offset);
		cbor_set_array_of_data(local_manifest_buffer, 4, &offset);

		//Payload Type
		cbor_set_signed_integer(local_manifest_buffer, -1, &offset);

		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->payload_length, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for payload_length");
			break;
		}
		//payload version
		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->payload_version, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for payload_version");
			break;
		}

		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		//offset
		if (0 != cbor_set_unsigned_integer(local_manifest_buffer, manifest_data->offset_in_oid, &offset))
		{
			pal_logger_print_message(" FAILED :  protect_update : protected_update_assign_length for offset_in_oid");
			break;
		}
		//write type
		cbor_set_unsigned_integer(local_manifest_buffer, (unsigned char)manifest_data->write_type, &offset);

		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		cbor_set_signed_integer(local_manifest_buffer, -1, &offset);
		cbor_set_byte_string(local_manifest_buffer, PROTECT_UPDATE_SHA256_LENGTH + 5, &offset);

		//digest fields 
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);

		if (strcmp(manifest_data->digest_algo, "SHA256") == 0)
		{
			digest_algo_value = (signed int)eSHA_256;
		}
		cbor_set_unsigned_integer(local_manifest_buffer, digest_algo_value, &offset);

		//creating digest

		payload_length_for_digest = (p_cbor_manifest->fragments_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : p_cbor_manifest->fragments_length;
		if (0 != pal_crypt_hash(	NULL,
									41,
									(const unsigned char *)p_cbor_manifest->fragments,
									payload_length_for_digest, 
									digest))
		{
			pal_logger_print_message(" FAILED :  protect_update : pal_crypt_hash");
			break;
		}

		cbor_set_byte_string(local_manifest_buffer, sizeof(digest), &offset);
		//digest: IFX_DigestSize
		memcpy((local_manifest_buffer+offset), digest, sizeof(digest));
		offset += sizeof(digest);

		cbor_set_null(local_manifest_buffer, &offset);

		//target oid
		cbor_set_array_of_data(local_manifest_buffer, 2, &offset);
		cbor_set_byte_string(local_manifest_buffer, 0, &offset);
		cbor_set_byte_string(local_manifest_buffer, 2, &offset);
		local_manifest_buffer[offset++] = (unsigned char)(manifest_data->target_oid >> 8);
		local_manifest_buffer[offset++] = (unsigned char)manifest_data->target_oid;

		//Updating length filed of payload
		data_to_sign_length = offset - length_marker - 1;
		local_manifest_buffer[length_marker + 1] = data_to_sign_length - 1;

		//Generate signature
		memcpy(data_to_sign, sign_header_ptr, sign_header_length);
		data_to_sign_length += 1;
		memcpy(data_to_sign + sign_header_length, local_manifest_buffer + length_marker, data_to_sign_length);
		data_to_sign_length += sign_header_length;

		signature_length = sizeof(signature);
		
		// Does ECDSA / RSA signing based on the key provided
		if(0 != pal_crypt_sign(	NULL,
								data_to_sign,
								data_to_sign_length,
								signature,
								&signature_length,
								(const unsigned char *)manifest_data->private_key,
								0))
		{
			pal_logger_print_message(" FAILED :  protect_update : pal_crypt_calculate_signature");
            break;
        }

		if (sign_algo_value == eES_SHA)
		{
			// Get expected signature length
			status = pal_crypt_get_signature_length((unsigned char *)manifest_data->private_key,&signature_length);
			if (0 != status)
			{
				pal_logger_print_message(" FAILED :  pal_crypt : Error in getting signature length");
				break;
			}
			// Remove encoding
			memset(extracted_signature, 0, sizeof(extracted_signature));
			status = protected_update_decode_ecc_signature(signature, extracted_signature, signature_length);
			if (0 != status)
			{
				pal_logger_print_message(" FAILED :  protect_update : protected_update_signature_parser");
				break;
			}
			cbor_set_byte_string(local_manifest_buffer, signature_length, &offset);
		    memcpy(&local_manifest_buffer[offset], extracted_signature, signature_length);
		}
        else if (sign_algo_value == eRSA_SSA_PKCS1_V1_5_SHA_256)
        {
			cbor_set_byte_string(local_manifest_buffer, signature_length, &offset);
            memcpy(&local_manifest_buffer[offset], signature, signature_length);
        }

		offset += signature_length;
		p_cbor_manifest->data_length = offset;
		p_cbor_manifest->data = (unsigned char *)malloc(offset);
		memcpy(p_cbor_manifest->data, local_manifest_buffer, offset);

		status = 0;
	} while (0);
	return status;

}

int protected_update_create_fragments(	manifest_t * manifest_data, 
										protected_update_data_set_d * p_cbor_manifest)
{
	int status = -1;
	unsigned char digest[PROTECT_UPDATE_SHA256_LENGTH];
	unsigned short remaining_payload_length = manifest_data->payload_length;
	unsigned short max_memory_required;
	unsigned char * fragments = NULL;
	unsigned char count_of_fragments = 0, index_for_hashing = 0;
	unsigned char * p_current_fragment = NULL;
	unsigned short length_to_digest;
	unsigned short payload_len_to_copy = 0;

	do
	{
		max_memory_required = (remaining_payload_length / MAX_PAYLOAD_FRAGMENT_SIZE) * MAX_PAYLOAD_SIZE;
		max_memory_required += (remaining_payload_length % MAX_PAYLOAD_FRAGMENT_SIZE);
		fragments = (unsigned char *)malloc(max_memory_required);
		
		// DEBUG prints
		//pal_logger_print_hex_data(manifest_data->payload, manifest_data->payload_length); pal_logger_print_message(""); 
		//printf("Total length : %d\n", manifest_data->payload_length);

		// Copy all the user data into fragment payloads
		do
		{
			p_current_fragment = fragments + (count_of_fragments * MAX_PAYLOAD_SIZE);

			payload_len_to_copy = (remaining_payload_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_FRAGMENT_SIZE : remaining_payload_length;
			memcpy(p_current_fragment,
				manifest_data->payload + (MAX_PAYLOAD_FRAGMENT_SIZE * count_of_fragments),
				payload_len_to_copy);

			remaining_payload_length -= payload_len_to_copy;
			count_of_fragments++;
			p_cbor_manifest->fragments_length += payload_len_to_copy;
			
		} while (remaining_payload_length != 0);

		// Index to start hashing last fragment
		index_for_hashing = count_of_fragments-1;
		// Start adding hash to fragement
		do
		{
			if (0 != index_for_hashing)
			{
				p_current_fragment = fragments + (index_for_hashing * MAX_PAYLOAD_SIZE);

				length_to_digest = ((count_of_fragments - index_for_hashing) == 1) ? payload_len_to_copy : MAX_PAYLOAD_SIZE;
			
				// Calculate hash on the current fragment and add to previous fragment
				pal_crypt_hash( NULL, 
								41,
								(const unsigned char *)p_current_fragment,
								length_to_digest,
								digest);		
				memcpy(	p_current_fragment - PROTECT_UPDATE_SHA256_LENGTH,
						digest,
						sizeof(digest));

				p_cbor_manifest->fragments_length += sizeof(digest);
			
				index_for_hashing--;
			}
			//pal_logger_print_hex_data(p_current_fragment, MAX_PAYLOAD_SIZE);

		} while (index_for_hashing != 0);

		p_cbor_manifest->fragments = fragments;
		// Below variable can be deleted
		p_cbor_manifest->actual_memory_allocated = max_memory_required;

	} while (0);

	status = 0;
	return status;
}

/**
* @}
*/