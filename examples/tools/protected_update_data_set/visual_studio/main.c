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
* \file main.c
*
* \brief   This file is the main file.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protected_update_data_set.h"
#include "pal\pal_file_system.h"
#include "pal\pal_logger.h"
#include "protected_update_data_set_version.h"

#define DEFAULT_PAYLOAD_VERSION			(0)
#define DEFAULT_TRUST_ANCHOR_OID		"E0E8"
#define DEFAULT_TARGET_OID				"E0E2"
#define DEFAULT_OFFSET_IN_OID			(0)
#define DEFAULT_WRITE_TYPE				(1)
#define DEFAULT_SIGN_ALGO				"ES_256"
#define DEFAULT_DIGEST_ALGO				"SHA256"
#define DEFAULT_PAYLOAD					NULL
#define DEFAULT_PRIV_KEY				NULL

static struct options
{
	int payload_version;
	unsigned char * trust_anchor_oid;
	unsigned char * target_oid;
	unsigned short offset_in_oid;
	unsigned char write_type;
	const unsigned char * signature_algo;
	const unsigned char * digest_algo;
	unsigned char * payload;
	const char * private_key;
} opt;

static void tool_usage()
{
	printf("\t%-20s %-8s %d \n", "payload_version=%%d", "default:", DEFAULT_PAYLOAD_VERSION);
	printf("\t%-20s %-8s %s \n", "trust_anchor_oid=%%s", "default:", DEFAULT_TRUST_ANCHOR_OID);
	printf("\t%-20s %-8s %s \n", "target_oid=%%s", "default:", DEFAULT_TARGET_OID);
	printf("\t%-20s %-8s %d \n", "offset=%%d", "default:", DEFAULT_OFFSET_IN_OID);
	printf("\t%-20s %-8s %d \n", "write_type=%%d", "default:", DEFAULT_WRITE_TYPE);
	printf("\t%-20s %-8s \n", "", "options : Write (1), EraseAndWrite (2)");
	printf("\t%-20s %-8s %s \n", "sign_algo=%%s", "default:", DEFAULT_SIGN_ALGO);
	printf("\t%-20s %-8s \n", "", "options : ES_256 , RSA-SSA-PKCS1-V1_5-SHA-256");
	printf("\t%-20s %-8s %s \n", "priv_key=%%s", "default:", DEFAULT_PAYLOAD);
	printf("\t%-20s %-8s \n", "", "note : Provide private key file(pem format)");
	printf("\t%-20s %-8s %s \n", "payload=%%s", "default:", DEFAULT_PRIV_KEY);
	printf("\t%-20s %-8s \n", "", "note : Provide file with readable content");
}

static void tool_exit_message(const char *name, const char *value)
{
	if (value == NULL)
		printf(" unknown option or missing value: %s\n", name);
	else
		printf(" option %s: illegal value: %s\n", name, value);

	tool_usage();
}

static void tool_show_user_inputs()
{
#if 0
	pal_logger_print_message("User inputs");
	printf("\t%-20s %-8X \n", "payload version :", opt.payload_version);
	printf("\t%-20s %-8s \n", "trust anchor oid :",opt.trust_anchor_oid);
	printf("\t%-20s %-8s \n", "target oid :", opt.target_oid);
	printf("\t%-20s %-8X \n", "offset in target :", opt.offset_in_oid);
	printf("\t%-20s %-8X \n", "write type :", opt.write_type);
	printf("\t%-20s %-8s \n", "signature algo :", opt.signature_algo);
	printf("\t%-20s %-8s \n", "payload :", opt.payload);
	printf("\t%-20s %-8s \n", "private key :", opt.private_key);

	pal_logger_print_message("");
#endif
}

static void tool_show_final_inputs(manifest_d* manifest_data)
{
	pal_logger_print_message("Final inputs");
	printf("\t%-20s %-8X \n", "payload version :", manifest_data->payload_version);
	printf("\t%-20s %-8X \n", "trust anchor oid :", manifest_data->trust_anchor_oid);
	printf("\t%-20s %-8X \n", "target oid :", manifest_data->target_oid);
	printf("\t%-20s %-8X \n", "offset in target :", manifest_data->offset_in_oid);
	printf("\t%-20s %-8X \n", "write type :", manifest_data->write_type);
	printf("\t%-20s %-8s \n", "signature algo :", manifest_data->signature_algo);
	printf("\t%-20s %-8X \n\t", "payload length :", manifest_data->payload_length);
    // Uncomment the below line to print payload data
	//pal_logger_print_hex_data(manifest_data->payload, manifest_data->payload_length);
	printf("\n\t%-20s %-8s \n", "private key :", manifest_data->private_key);

	pal_logger_print_message(""); pal_logger_print_message("");
}

static int tool_get_user_inputs(int argc, char *argv[])
{
	int status = 0;
	int i;
	char *p, *q;

	opt.payload_version = DEFAULT_PAYLOAD_VERSION;
	opt.trust_anchor_oid =	DEFAULT_TRUST_ANCHOR_OID;
	opt.target_oid =		DEFAULT_TARGET_OID;
	opt.offset_in_oid =		DEFAULT_OFFSET_IN_OID;
	opt.write_type =		DEFAULT_WRITE_TYPE;
	opt.signature_algo =	DEFAULT_SIGN_ALGO;
	opt.digest_algo =		DEFAULT_DIGEST_ALGO;
	opt.payload =			DEFAULT_PAYLOAD;
	opt.private_key =		DEFAULT_PRIV_KEY;

	do
	{
		for (i = 1; i < argc; i++)
		{
			p = argv[i];
			if ((q = strchr(p, '=')) == NULL)
			{
				status = -1;
				break;
			}

			*q++ = '\0';

			if (strcmp(p, "payload_version") == 0)
				opt.payload_version = atoi(q);
			else if (strcmp(p, "trust_anchor_oid") == 0)
				opt.trust_anchor_oid = q;
			else if (strcmp(p, "target_oid") == 0)
				opt.target_oid = q;
			else if (strcmp(p, "offset") == 0)
				opt.offset_in_oid = atoi(q);
			else if (strcmp(p, "write_type") == 0)
				opt.write_type = atoi(q);
			else if (strcmp(p, "sign_algo") == 0)
				opt.signature_algo = q;
			else if (strcmp(p, "digest_algo") == 0)
				opt.digest_algo = q;
			else if (strcmp(p, "priv_key") == 0)
				opt.private_key = q;
			else if (strcmp(p, "payload") == 0)
				opt.payload = q;

			else
			{
				status = -1;
				break;
			}
		}
	} while (0);

	if (0 == status)
	{
		tool_show_user_inputs();
	}
	else
	{
		tool_exit_message(p, NULL);
	}
	return status;
}

static int tool_set_user_inputs(manifest_d* manifest_data )
{
	int status = -1;
	manifest_data->digest_algo = opt.digest_algo;
	manifest_data->offset_in_oid = opt.offset_in_oid;
	manifest_data->payload = opt.payload;
	manifest_data->payload_version = opt.payload_version;
	manifest_data->signature_algo = opt.signature_algo;
	manifest_data->target_oid = (unsigned short)strtol(opt.target_oid, NULL, 16);
	manifest_data->trust_anchor_oid = (unsigned short)strtol(opt.trust_anchor_oid, NULL, 16);
	manifest_data->write_type = opt.write_type;
	manifest_data->private_key = opt.private_key;

	status = pal_file_system_read_file_to_array(opt.payload, &((unsigned char *)manifest_data->payload), &(manifest_data->payload_length));

	if (0 == status)
	{
		tool_show_final_inputs(manifest_data);
	}
	return status;
}

int main(int argc, char *argv[])
{
	int exit_status = 1;
	manifest_d manifest = {0};
	protected_update_data_set_d cbor_manifest = { 0 };
	cbor_manifest.data = NULL;
	cbor_manifest.fragments = NULL;
	do
	{
		pal_logger_print_message("Tool Version\t");
		pal_logger_print_message(PROTECTED_UPDATE_VERSION);
		pal_logger_print_message("");
		if (0 != tool_get_user_inputs(argc, argv))
		{
			break;
		}
		if( 0 != tool_set_user_inputs(&manifest))
		{
			break;
		}
		// Generate fragements
		if (0 != protected_update_create_fragments(manifest, &cbor_manifest))
		{
			pal_logger_print_message("Error while creating fragments");
			break;
		}
		// Generate Manifest
		if (0 != protected_update_create_manifest(manifest, &cbor_manifest))
		{
			pal_logger_print_message("Error while creating manifest");
            break;
		}
		pal_logger_print_manifest(&cbor_manifest);
		pal_logger_print_fragments(&cbor_manifest);
		exit_status = 0;
	} while (0);
	//free memory
	if (NULL != cbor_manifest.data)
	{
		free(cbor_manifest.data);
	}
	if (NULL != cbor_manifest.fragments)
	{
		free(cbor_manifest.fragments);
	}
	return exit_status;
}


/**
* @}
*/