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
* \file protected_update_data_set.h
*
* \brief   This file defines APIs, types and data structures used for protected update data set creation.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _PROTECTED_UPDATE_
#define _PROTECTED_UPDATE_

#define MAX_PAYLOAD_SIZE            (640)
#define MAX_PAYLOAD_FRAGMENT_SIZE   (608)

// Structure to store manifest data
typedef struct manifest
{
	unsigned short payload_version;
	unsigned short trust_anchor_oid;
	unsigned short target_oid;
	unsigned short offset_in_oid;
	const unsigned char * signature_algo;
	const unsigned char * digest_algo;
	const unsigned char * payload;
	unsigned short payload_length;
	unsigned short write_type;
	const char * private_key;
} manifest_d;

// Structure to store protect update data set
typedef struct protected_update_data_set
{
	unsigned char * data;
	unsigned short data_length;

	unsigned char * fragments;
	unsigned short fragments_length;
	unsigned short actual_memory_allocated;

}protected_update_data_set_d;

// Create protected update manifest
int protected_update_create_manifest(	manifest_d manifest_data, 
										protected_update_data_set_d * p_cbor_manifest);

// Create protected update payload fragments
int protected_update_create_fragments(	manifest_d manifest_data, 
										protected_update_data_set_d * p_cbor_manifest);

#endif //_PROTECTED_UPDATE_

/**
* @}
*/