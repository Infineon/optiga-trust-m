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
* \file user_input_parser.c
*
* \brief   This file gets the user input and convert them to types and data structures used for protected update data set creation.
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
#include "user_input_parser.h"
#include "pal\pal_crypt.h"
#include "pal\pal_os_memory.h"

#define TOOL_DISPLAY_FORMAT_INTEGER         "\t%-20s : %-8X \n"
#define TOOL_DISPLAY_FORMAT_STRING          "\t%-20s : %-8s \n"

#define TOOL_ASSIGN                             ""
/* Default values and shortname*/
// MANIFEST
#define DESC_PAYLOAD_VERSION                "Payload version"
#define SHORT_NAME_PAYLOAD_VERSION          "payload_version"TOOL_ASSIGN
#define DEFAULT_PAYLOAD_VERSION             "0"

#define DESC_TRUST_ANCHOR_OID               "Trust anchor oid"
#define SHORT_NAME_TRUST_ANCHOR_OID         "trust_anchor_oid"TOOL_ASSIGN
#define DEFAULT_TRUST_ANCHOR_OID            "E0E8"

#define DESC_TARGET_OID                     "Target oid"
#define SHORT_NAME_TARGET_OID               "target_oid"TOOL_ASSIGN
#define DEFAULT_TARGET_OID                  "E0E2"

#define DESC_COUID                          "Co-Processor OID"
#define SHORT_NAME_COUID                    "couid"TOOL_ASSIGN
#define DEFAULT_COUID                       NULL

#define DESC_SIGN_ALGO                      "Signature Algorithm"
#define SHORT_NAME_SIGN_ALGO                "sign_algo"TOOL_ASSIGN
#define DEFAULT_SIGN_ALGO                   "ES_256"

#define DESC_PRIV_KEY                       "Private key"
#define SHORT_NAME_PRIV_KEY                 "priv_key"TOOL_ASSIGN
#define DEFAULT_PRIV_KEY                    NULL

#define DESC_DIGEST_ALGO                    "Digest algorithm"
#define SHORT_NAME_DIGEST_ALGO              "digest_algo"TOOL_ASSIGN
#define DEFAULT_DIGEST_ALGO                 "SHA256"

#define DESC_PAYLOAD_TYPE                   "Type of Payload"
#define SHORT_NAME_PAYLOAD_TYPE             "payload_type"TOOL_ASSIGN
#define DEFAULT_PAYLOAD_TYPE                "data"

//CONFIDENTIALITY
#define DESC_SHARED_SECRET                  "Shared secret"
#define SHORT_NAME_SHARED_SECRET            "secret"TOOL_ASSIGN
#define DEFAULT_SHARED_SECRET               NULL

#define DESC_SHARED_SECRET_OID              "Shared secret OID"
#define SHORT_NAME_SHARED_SECRET_OID        "secret_oid"TOOL_ASSIGN
#define DEFAULT_SHARED_SECRET_OID           "F1D0"

#define DESC_LABEL                          "Label"
#define SHORT_NAME_LABEL                    "label"TOOL_ASSIGN
#define DEFAULT_LABEL                       "Confidentiality"

#define DESC_SEED_LENGTH                     "Length of seed"
#define SHORT_NAME_SEED_LENGTH               "seed_length"TOOL_ASSIGN
#define DEFAULT_SEED_LENGTH                  "64"

#define DESC_KDF                            "Key Derivation Function"
#define SHORT_NAME_KDF                      "kdf"TOOL_ASSIGN
#define DEFAULT_KDF                         "IFX_KDF-TLS12_PRF_SHA256"

#define DESC_ENCRYPT_ALGO                   "Encryption algorithm"
#define SHORT_NAME_ENCRYPT_ALGO             "enc_algo"TOOL_ASSIGN
#define DEFAULT_ENCRYPT_ALGO                "AES-CCM-16-64-128"

// PAYLOAD : DATA
#define DESC_OFFSET_IN_OID                  "Offset in OID"
#define SHORT_NAME_OFFSET_IN_OID            "offset"TOOL_ASSIGN
#define DEFAULT_OFFSET_IN_OID               "0"

#define DESC_WRITE_TYPE                     "Type of Write"
#define SHORT_NAME_WRITE_TYPE               "write_type"TOOL_ASSIGN
#define DEFAULT_WRITE_TYPE                  "2"

#define DESC_PAYLOAD_DATA                   "OID data"
#define SHORT_NAME_PAYLOAD_DATA             "data"TOOL_ASSIGN
#define DEFAULT_PAYLOAD_DATA                NULL

#define DESC_INPUT_DATA_FORMAT               "Input data format"
#define SHORT_NAME_INPUT_DATA_FORMAT         "in_data_format"TOOL_ASSIGN
#define DEFAULT_INPUT_DATA_FORMAT            "hex"

// PAYLOAD : KEY
#define DESC_PAYLOAD_KEY                    "Key Data"
#define SHORT_NAME_PAYLOAD_KEY              "key_data"TOOL_ASSIGN
#define DEFAULT_PAYLOAD_KEY                 NULL

#define DESC_KEY_ALGO                       "Key algorithm"
#define SHORT_NAME_KEY_ALGO                 "key_algo"TOOL_ASSIGN
#define DEFAULT_KEY_ALGO                    ""

#define DESC_KEY_USAGE                      "Key Usage"
#define SHORT_NAME_KEY_USAGE                "key_usage"TOOL_ASSIGN
#define DEFAULT_KEY_USAGE                   "02"

// PAYLOAD : METADATA
#define DESC_PAYLOAD_METADATA               "Meta Data"
#define SHORT_NAME_PAYLOAD_METADATA         "metadata"TOOL_ASSIGN
#define DEFAULT_PAYLOAD_METADATA            NULL

#define DESC_CONTENT_RESET                  "Content Reset"
#define SHORT_NAME_CONTENT_RESET            "content_reset"TOOL_ASSIGN
#define DEFAULT_CONTENT_RESET               "0"

#define DEFAULT_ADDITIONAL_FLAG             "0"

// Dataset output to file
#define DESC_DATASET_OUTPUT_FILE            "Dataset output to file"
#define SHORT_NAME_DATASET_OUTPUT_FILE      "dataset_to_file"TOOL_ASSIGN
#define DEFAULT_DATASET_OUTPUT_FILE         NULL

// Details
#define DETAIL_MANIFEST                     "(1) : To create manifest , provide the following details"
#define DETAIL_CONFIDENTIALITY              "(2) : To enable confidentiality,\"secret\" must be provided (All other options are ignored if there is no confidentiality)"
#define DETAIL_DATA_OBJ                     "(3.1) : To update data object, \"payload_type\" should be \"data\" and provide the following details:"
#define DETAIL_KEY_OBJ                      "(3.2) : To update key object, \"payload_type\" should be \"key\" and provide the following details:"
#define DETAIL_METADATA_OBJ                 "(3.3) : To update metadata object, \"payload_type\" should be \"metadata\" and provide the following details:"
#define DETAIL_DATASET_TO_FILE              "(4) : To write dataset to file, \"dataset_to_file\" should be the file path "

#define _NEXT_		"\n\t\t\t\t      :  "

/* Short name*/
static struct options
{
    // MANIFEST
    uint8_t * payload_version;
    uint8_t * trust_anchor_oid;
    uint8_t * target_oid;
    uint8_t * couid;
    uint8_t * signature_algo;
    uint8_t * private_key;
    uint8_t * digest_algo;
    uint8_t * payload_type;

    // CONFIDENTIALITY
    uint8_t * shared_secret;
    uint8_t * shared_secret_oid;
    uint8_t * label;
    uint8_t * encrypt_algo;
    uint8_t * kdf;
    uint8_t * seed_length;
    
    // PAYLOAD :  DATA
    uint8_t * offset_in_oid;
    uint8_t * write_type;
    uint8_t * data;
    uint8_t * input_data_format;

    // PAYLOAD : KEY
    uint8_t * key_data;
    uint8_t * key_usage;
    uint8_t * key_algo;

    // PAYLOAD :  METADATA
    uint8_t * content_reset;
    uint8_t * metadata;

    // FILEPATH NAME
    uint8_t *dataset_to_file_path;
} opt;

typedef struct option_property
{
    const int8_t * description;
    const int8_t * short_name;
    uint8_t ** opt_val;
    uint8_t * default_val;
    uint8_t modified;
    const int8_t * option_details;
    const int8_t * note_details;
}opt_prop_t;

//lint --e{64, 786} suppress "Type mismatch "
opt_prop_t option_table[] = 
{
    // MANIFEST
    { "Details", DETAIL_MANIFEST, NULL, "", 0, "", "" },
    { DESC_PAYLOAD_VERSION ,SHORT_NAME_PAYLOAD_VERSION, &opt.payload_version, DEFAULT_PAYLOAD_VERSION, 0, "","Input is a decimal string.E.g. 10" },
    { DESC_TRUST_ANCHOR_OID, SHORT_NAME_TRUST_ANCHOR_OID, &opt.trust_anchor_oid, DEFAULT_TRUST_ANCHOR_OID, 0, "", "Input is a hexadecimal string.E.g. E0E8" },
    { DESC_TARGET_OID, SHORT_NAME_TARGET_OID, &opt.target_oid, DEFAULT_TARGET_OID, 0, "", "Input is a hexadecimal string.E.g. E0E2" },
    { DESC_COUID, SHORT_NAME_COUID, &opt.couid, DEFAULT_COUID, 0, "", "Unicast gets enabled if \"couid\" is provided otherwise it is broadcast."_NEXT_"Input is a hexadecimal string.E.g. A1DE34" },
    { DESC_SIGN_ALGO, SHORT_NAME_SIGN_ALGO, &opt.signature_algo, DEFAULT_SIGN_ALGO, 0, "ES_256 , RSA-SSA-PKCS1-V1_5-SHA-256","" },
    { DESC_PRIV_KEY, SHORT_NAME_PRIV_KEY, &opt.private_key, DEFAULT_PRIV_KEY, 0, "private key file (pem format)", "Refer : samples/integrity/sample_ec_256_priv.pem" },
    { DESC_DIGEST_ALGO, SHORT_NAME_DIGEST_ALGO, &opt.digest_algo, DEFAULT_DIGEST_ALGO, 0, "SHA256", "" },
    { DESC_PAYLOAD_TYPE, SHORT_NAME_PAYLOAD_TYPE, &opt.payload_type, DEFAULT_PAYLOAD_TYPE, 0, "data , key , metadata", "" },
    
    // CONFIDENTIALITY
    { "Details", DETAIL_CONFIDENTIALITY, NULL, "", 0, "", "" },
    { DESC_SHARED_SECRET, SHORT_NAME_SHARED_SECRET, &opt.shared_secret, DEFAULT_SHARED_SECRET, 0, "Text file containing shared secret as hexadecimal string", "Refer : samples/confidentiality/secret.txt" },
    { DESC_SHARED_SECRET_OID, SHORT_NAME_SHARED_SECRET_OID, &opt.shared_secret_oid, DEFAULT_SHARED_SECRET_OID, 0, "", "Input is a hexadecimal string.E.g. F1D0" },
    { DESC_LABEL, SHORT_NAME_LABEL, &opt.label, DEFAULT_LABEL, 0, "", "Input is a string.E.g. "DEFAULT_LABEL },
    { DESC_ENCRYPT_ALGO, SHORT_NAME_ENCRYPT_ALGO, &opt.encrypt_algo, DEFAULT_ENCRYPT_ALGO, 0, "AES-CCM-16-64-128", "" },
    { DESC_KDF, SHORT_NAME_KDF, &opt.kdf, DEFAULT_KDF, 0, "IFX_KDF-TLS12_PRF_SHA256", "" },
    { DESC_SEED_LENGTH, SHORT_NAME_SEED_LENGTH, &opt.seed_length, DEFAULT_SEED_LENGTH, 0, "", "Input is a decimal string.E.g. 64" },
    
    // PAYLOAD : DATA
    { "Details", DETAIL_DATA_OBJ, NULL, "", 0, "", "" },
    { DESC_OFFSET_IN_OID,SHORT_NAME_OFFSET_IN_OID, &opt.offset_in_oid, DEFAULT_OFFSET_IN_OID, 0, "","" },
    { DESC_WRITE_TYPE, SHORT_NAME_WRITE_TYPE, &opt.write_type, DEFAULT_WRITE_TYPE, 0, "Write (1), EraseAndWrite (2)", "" },
    { DESC_PAYLOAD_DATA, SHORT_NAME_PAYLOAD_DATA, &opt.data, DEFAULT_PAYLOAD_DATA, 0, "","Input is a text file with hexadecimal or ascii string content" },
    { DESC_INPUT_DATA_FORMAT, SHORT_NAME_INPUT_DATA_FORMAT, &opt.input_data_format, DEFAULT_INPUT_DATA_FORMAT, 0, "hex , ascii", "Refer : samples/payload/data/ascii_data.txt for input_data_format=ascii"_NEXT_"Refer : samples/payload/data/hex_data.txt for input_data_format=hex"},
    
    // PAYLOAD : KEYS
    { "Details", DETAIL_KEY_OBJ, NULL, "", 0, "", "" },
    { DESC_KEY_USAGE, SHORT_NAME_KEY_USAGE, &opt.key_usage, DEFAULT_KEY_USAGE, 0, "AUTH (0x01) , ENC (0x02) , SIGN (0x10) , KEY_AGREE (0x20)", "Input is a hexadecimal string.E.g. E3"_NEXT_"The values in \"options\" can be bitwise ORED and provided ( Refer SRM )" },
    { DESC_KEY_ALGO, SHORT_NAME_KEY_ALGO, &opt.key_algo, DEFAULT_KEY_ALGO, 0, "ECC-NIST-P-256 (3) , ECC-NIST-P-384 (4), ECC-NIST-P-521 (5), ECC-BRAINPOOL-P-256-R1 (19) , ECC-BRAINPOOL-P-384-R1 (21), ECC-BRAINPOOL-P-512-R1 (22), RSA-1024-Exp (65) , RSA-2048-Exp (66), AES-128 (129), AES-192 (129), AES-256 (131)", "Input is a decimal string.E.g. 129" },
    { DESC_PAYLOAD_KEY, SHORT_NAME_PAYLOAD_KEY, &opt.key_data, DEFAULT_PAYLOAD_KEY, 0, "ECC / RSA key in .pem format or AES key in txt file as hexadecimal string", "Refer : samples/payload/key/sample_ec_256_priv.pem for ECC or RSA key"_NEXT_"Refer : samples/payload/key/aes_key.txt for AES key" },
    
    // PAYLOAD : METADATA
    { "Details", DETAIL_METADATA_OBJ, NULL, "", 0, "", "" },
    { DESC_CONTENT_RESET,SHORT_NAME_CONTENT_RESET, &opt.content_reset, DEFAULT_CONTENT_RESET, 0, "As per metadata identifier flag (0), Zeroes (1), Random (2)","" },
    { DESC_PAYLOAD_METADATA, SHORT_NAME_PAYLOAD_METADATA, &opt.metadata, DEFAULT_PAYLOAD_METADATA, 0, "", "Input is a txt file as hexadecimal string" },

    // Dataset to output file
    { "Details", DETAIL_DATASET_TO_FILE, NULL, "", 0, "", "" },
    { DESC_DATASET_OUTPUT_FILE, SHORT_NAME_DATASET_OUTPUT_FILE, &opt.dataset_to_file_path, DEFAULT_DATASET_OUTPUT_FILE, 0, "Provide the filename for output dataset to be stored", ""},
};

extern uint8_t * dataset_file_path;

static int8_t buffer[200];

void _tool_show_usage( void)
{
    uint8_t index;
    uint8_t count = sizeof(option_table) / sizeof(opt_prop_t);

    sprintf(buffer, "Usage : <.exe> input1=<value> input2=<value> ..\n");
    pal_logger_print_message(buffer);

    for (index = 0; index < count; index++)
    {
        if (strcmp("Details", option_table[index].description) == 0)
        {
            sprintf(buffer,"\n\t%-20s\n", option_table[index].short_name);
            pal_logger_print_message(buffer);
        }
        else
        {
            sprintf(buffer, "\t%-20s %-8s :  %-8s \n", option_table[index].short_name, "default", option_table[index].default_val);
            pal_logger_print_message(buffer);
            if (strcmp("", option_table[index].option_details) != 0)
            {
                sprintf(buffer, "\t%-20s %-8s :  %-8s \n", "","options", option_table[index].option_details);
                pal_logger_print_message(buffer);
            }
            if (strcmp("", option_table[index].note_details) != 0)
            {
                sprintf(buffer, "\t%-20s %-8s :  %-8s \n", "", "note",option_table[index].note_details);
                pal_logger_print_message(buffer);
            }
        }
    }
}

_STATIC_H int32_t set_str_to_long(const int8_t * src, uint32_t * dest)
{
    int32_t status = -1;
    int64_t val = atoi(src);
    // Destination is unsigned value of 32 bits, hence compared with 0x80000000
    if (0x80000000 >= val)
    {
        *dest = (uint32_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H int32_t set_str_to_short(const int8_t * src, uint16_t * dest)
{
    int32_t status = -1;
    int32_t val = atoi(src);
    // Destination is unsigned value of 16 bits, hence compared with 0x8000
    if (0x8000 > val)
    {
        *dest = (uint16_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H int32_t set_str_to_byte(const int8_t * src, uint8_t * dest)
{
    int32_t status = -1;
    int32_t val = atoi(src);
    // Destination is unsigned value of 8 bits, hence compared with 0x80
    if (0x80 > val)
    {
        *dest = (uint8_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H int32_t set_str_to_byte_signed(const int8_t * src, int8_t * dest)
{
    int32_t status = -1;
    int32_t val = atoi(src);
    if (0xFF >= val)
    {
        *dest = (int8_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H int32_t set_str_to_hex_one_byte(const int8_t * src, uint8_t * dest)
{
    int32_t status = -1;
    int64_t val = strtol(src, NULL, 16);
    if (0x80 > (uint8_t)val)
    {
        *dest = (uint8_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H int32_t set_str_to_hex(const int8_t * src, uint16_t * dest)
{
    int32_t status = -1;
    int64_t val = strtol(src, NULL, 16);
    if (0xffff > val)
    {
        *dest = (uint16_t)val;
        status = 0;
    }
    return status;
}

_STATIC_H void _tool_show_user_inputs( void)
{
    uint8_t index;
    uint8_t count = sizeof(option_table) / sizeof(opt_prop_t);

    pal_logger_print_message("Info : User provided inputs\n");
    for (index = 0; index < count; index++)
    {
        if (1 == option_table[index].modified)
        {
            sprintf(buffer, "\t%-20s : %-8s \n", option_table[index].description, *(option_table[index].opt_val));
            pal_logger_print_message(buffer);
        }
    }
    pal_logger_print_message("\n");
}

_STATIC_H void _tool_set_default_value( void)
{
    uint8_t index;
    uint8_t count = sizeof(option_table) / sizeof(opt_prop_t);
    for (index = 0; index < count; index++)
    {
        if (NULL != option_table[index].opt_val)
        {
            *(option_table[index].opt_val) = option_table[index].default_val;
        }
    }
    pal_logger_print_message("Info : Default values are set\n");
}

_STATIC_H int32_t _tool_set_option(int8_t * p_option, int8_t * p_user_value)
{
    int32_t status = -1;
    uint8_t index;
    uint8_t count = sizeof(option_table) / sizeof(opt_prop_t);
    
    for (index = 0; index < count; index++)
    {
        if (strcmp(p_option, option_table[index].short_name) == 0)
        {
            *(option_table[index].opt_val) = p_user_value;
            option_table[index].modified = 1;
            status = 0;
            break;
        }
    }
    if (-1 == status)
    {
        sprintf(buffer, "Error :  User provided option %s cannot be found", p_option);
        pal_logger_print_message(buffer);
    }
    return status;
}

_STATIC_H int32_t _tool_set_manifest_info(manifest_t* p_manifest_data)
{
    int32_t status = -1;

    do
    {
        //MANIFEST
        if (0 != set_str_to_short(opt.payload_version, &(p_manifest_data->payload_version)))
        {
            pal_logger_print_message("Error : Invalid input payload version provided\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_PAYLOAD_VERSION, p_manifest_data->payload_version);
        pal_logger_print_message(buffer);

        p_manifest_data->trust_anchor_oid = (uint16_t)strtol(opt.trust_anchor_oid, NULL, 16);
        if (0 != set_str_to_hex(opt.trust_anchor_oid, &(p_manifest_data->trust_anchor_oid)))
        {
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_TRUST_ANCHOR_OID, p_manifest_data->trust_anchor_oid);
        pal_logger_print_message(buffer);
    
        if (0 != set_str_to_hex(opt.target_oid, &(p_manifest_data->target_oid)))
        {
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_TARGET_OID, p_manifest_data->target_oid);
        pal_logger_print_message(buffer);

        p_manifest_data->private_key = opt.private_key;
        if (strcmp("SHA256", opt.digest_algo) == 0)
        {
            p_manifest_data->digest_algo = eSHA_256;
        }
        else
        {
            pal_logger_print_message("Error : Invalid input provided for digest algorithm\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_DIGEST_ALGO, p_manifest_data->digest_algo);
        pal_logger_print_message(buffer);

        p_manifest_data->couid = NULL;
        if (NULL != opt.couid)
        {
            p_manifest_data->couid = opt.couid;
            sprintf(buffer, TOOL_DISPLAY_FORMAT_STRING, DESC_COUID, p_manifest_data->couid);
            pal_logger_print_message(buffer);
        }
        if (strcmp("ES_256", opt.signature_algo) == 0)
        {
            p_manifest_data->signature_algo = eES_SHA;
        }
        else if (strcmp("RSA-SSA-PKCS1-V1_5-SHA-256", opt.signature_algo) == 0)
        {
            p_manifest_data->signature_algo = eRSA_SSA_PKCS1_V1_5_SHA_256;
        }
        else
        {
            pal_logger_print_message("Error : Invalid input provided for signature algorithm\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_SIGN_ALGO, p_manifest_data->signature_algo);
        pal_logger_print_message(buffer);

        if (0 == strcmp("data", opt.payload_type))
        {
            p_manifest_data->payload_type = ePAYLOAD_DATA;
        }
        else if (0 == strcmp("key", opt.payload_type))
        {
            p_manifest_data->payload_type = ePAYLOAD_KEY;
        }
        else if (0 == strcmp("metadata", opt.payload_type))
        {
            p_manifest_data->payload_type = ePAYLOAD_METADATA;
        }
        else
        {
            pal_logger_print_message("Error : Invalid input provided for payload type\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_PAYLOAD_TYPE, p_manifest_data->payload_type);
        pal_logger_print_message(buffer);
        status = 0;
    } while (0);
    return status;
}

_STATIC_H int32_t _tool_set_confidentiality_info(confidentiality_t * p_confidentiality)
{
    int32_t status = 1;
    uint8_t default_seed_length;
    do
    {
        //shared secret
        status = pal_file_system_read_file_to_array_in_hex((const int8_t *)opt.shared_secret,
            &p_confidentiality->kdf_data.shared_secret,
            &(p_confidentiality->kdf_data.shared_secret_length));
        if (0 != status)
        {
            pal_logger_print_message("Error : Unable to read shared secret length\n");
            break;
        }
        status = -1;

        if (0 != set_str_to_byte(opt.seed_length, &(p_confidentiality->kdf_data.seed_length)))
        {
            break;
        }
        (void)set_str_to_byte(DEFAULT_SEED_LENGTH, &default_seed_length);
        if ( default_seed_length < p_confidentiality->kdf_data.seed_length)
        {
            pal_logger_print_message("Error : Seed length not supported\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_SEED_LENGTH, p_confidentiality->kdf_data.seed_length);
        pal_logger_print_message(buffer);

        // Generate seed 
        p_confidentiality->kdf_data.seed = (uint8_t *)pal_os_malloc(sizeof(uint8_t) * p_confidentiality->kdf_data.seed_length);

        if (0 != pal_crypt_set_seed(NULL,
            p_confidentiality->kdf_data.seed, (uint16_t)p_confidentiality->kdf_data.seed_length))
        {
            pal_logger_print_message(" Error : Failed in pal_crypt_set_seed\n");
            break;
        }

        //Label
        p_confidentiality->kdf_data.label = opt.label;
        sprintf(buffer, TOOL_DISPLAY_FORMAT_STRING, DESC_LABEL, p_confidentiality->kdf_data.label);
        pal_logger_print_message(buffer);

        p_confidentiality->kdf_data.label_length = (uint16_t)strlen((const int8_t *)opt.label);

        // Derive the key
        if (0 != pal_crypt_tls_prf_sha256(    NULL,
                                            p_confidentiality->kdf_data.shared_secret,
                                            p_confidentiality->kdf_data.shared_secret_length, 
                                            p_confidentiality->kdf_data.label,
                                            p_confidentiality->kdf_data.label_length,
                                            p_confidentiality->kdf_data.seed,
                                            p_confidentiality->kdf_data.seed_length,
                                            p_confidentiality->enc_params.session_key,
                                            sizeof(p_confidentiality->enc_params.session_key)))
        {
            pal_logger_print_message(" Error : Failed in pal_crypt_tls_prf_sha256\n");
            break;
        }

        // Fetch the nonce and key
        p_confidentiality->enc_params.session_key_length = sizeof(p_confidentiality->enc_params.session_key);
        
        memcpy(p_confidentiality->enc_params.nonce_data, p_confidentiality->enc_params.session_key + (p_confidentiality->enc_params.session_key_length - NONCE_LENGTH), NONCE_LENGTH);
        p_confidentiality->enc_params.nonce_length = NONCE_LENGTH;

        // shared secret oid
        p_confidentiality->kdf_data.shared_secret_oid = (uint16_t)strtol(opt.shared_secret_oid, NULL, 16);
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_SHARED_SECRET_OID, p_confidentiality->kdf_data.shared_secret_oid);
        pal_logger_print_message(buffer);

        // shared secret 
        sprintf(buffer, "\t%-20s : \n\t", DESC_SHARED_SECRET);
        pal_logger_print_hex_data (p_confidentiality->kdf_data.shared_secret, p_confidentiality->kdf_data.shared_secret_length);

        // Encryption algorithm
        if (strcmp("AES-CCM-16-64-128", opt.encrypt_algo) == 0)
        {
            // set enum below
            p_confidentiality->enc_params.encrypt_algo = eAES_CCM_16_64_128;
            p_confidentiality->enc_params.mac_size = MAC_SIZE;
        }
        else
        {
            pal_logger_print_message("Error : Invalid input provided for encryption algorithm\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_ENCRYPT_ALGO, p_confidentiality->enc_params.encrypt_algo);
        pal_logger_print_message(buffer);

        // KDF
        if (strcmp("IFX_KDF-TLS12_PRF_SHA256", opt.kdf) == 0)
        {
            p_confidentiality->kdf_data.key_derivation_algo = eIFX_KDF_TLS12_PRF_SHA256;
        }
        else
        {
            pal_logger_print_message("Error : Invalid input provided for KDF\n");
            break;
        }
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_KDF, p_confidentiality->kdf_data.key_derivation_algo);
        pal_logger_print_message(buffer);


        status = 0;
    } while (0);
    return status;
}

_STATIC_H int32_t _tool_set_object_data_info(data_payload_t * p_data_payload)
{
    int32_t status = 1;

    do
    {
        if(0 != set_str_to_long((const int8_t *)opt.offset_in_oid, &p_data_payload->offset_in_oid))
		{
			pal_logger_print_message("Error : Invalid offset in oid provided\n");
			break;
		}
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_OFFSET_IN_OID, p_data_payload->offset_in_oid);
        pal_logger_print_message(buffer);
		 
        if(0 != set_str_to_byte((const int8_t *)opt.write_type, &p_data_payload->write_type))
		{
			pal_logger_print_message("Error : Invalid write type provided\n");
			break;
		}
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_WRITE_TYPE, p_data_payload->write_type);
        pal_logger_print_message(buffer);

        sprintf(buffer, TOOL_DISPLAY_FORMAT_STRING, DESC_INPUT_DATA_FORMAT, opt.input_data_format);
        pal_logger_print_message(buffer);
        if(!strcmp("ascii", (const int8_t *)opt.input_data_format))
        {
            status = pal_file_system_read_file_to_array((const int8_t *)opt.data, &(p_data_payload->data), &(p_data_payload->data_length));
        }
        else if(!strcmp("hex", (const int8_t *)opt.input_data_format))
        {
            status = pal_file_system_read_file_to_array_in_hex((const int8_t *)opt.data, &(p_data_payload->data), &(p_data_payload->data_length));
        }
        else
        {
            pal_logger_print_message("Error : Invalid input data format provided\n");
			status = 1;
			break;
        }
        if (0 != status)
        {
            pal_logger_print_message("Error : Unable to read data to be written to OID\n");
            break;
        }
        status = 0;
    } while (0);
    return status;
}

_STATIC_H int32_t _tool_set_key_data_info(key_payload_t * p_key_payload)
{
    int32_t status = 1;
    int8_t * test_file = ".txt";
    uint8_t key_type = (uint8_t)eAES;
    do
    {
        if (NULL == strstr(opt.key_data,test_file))
        {
            key_type = (uint8_t)(pal_crypt_get_key_type(opt.key_data));
        }
        
        switch (key_type)
        {
        case eRSA:
#define RSA_KEY            (p_key_payload->key_params.rsa_key)

            status = pal_crypt_parse_rsa_key(
                                             opt.key_data, &(RSA_KEY.N), &(RSA_KEY.N_length),
                                             &(RSA_KEY.E), &(RSA_KEY.E_length),
                                             &(RSA_KEY.D), &(RSA_KEY.D_length));
            if (0 != status)
            {
                pal_logger_print_message("Error : Unable to parse RSA key\n");
                break;
            }
            p_key_payload->key_type = eRSA;

#undef RSA_KEY
            break;

        case eECC:
#define ECC_KEY            (p_key_payload->key_params.ecc_key)

            status = pal_crypt_parse_ecc_key(
                                             opt.key_data,
                                             &(ECC_KEY.D), &(ECC_KEY.D_length),
                                             &(ECC_KEY.X), &(ECC_KEY.X_length),
                                             &(ECC_KEY.Y), &(ECC_KEY.Y_length));
            if (0 != status)
            {
                pal_logger_print_message("Error : Unable to parse ECC key\n");
                break;
            }
            p_key_payload->key_type = eECC;

#undef ECC_KEY
            break;

        case eAES:
            status = pal_file_system_read_file_to_array_in_hex(opt.key_data, &(p_key_payload->key_params.aes_key.key), &(p_key_payload->key_params.aes_key.key_length));
            if (0 != status)
            {
                pal_logger_print_message("Error : Unable to read AES key\n");
                break;
            }
            if(((p_key_payload->key_params.aes_key.key_length) % 2) != 0)
            {
                pal_logger_print_message("Error : Key length is not correct\n");
                break;
            }

            p_key_payload->key_type = eAES;
            break;

        default:
            break;
        }
        p_key_payload->key_data_file_path = opt.key_data;
    } while (0);
    return status;
}

_STATIC_H int32_t _tool_set_meta_data_info(metadata_payload_t * p_metadata_payload)
{
    int32_t status = 1;
    do
    {
        (void)set_str_to_byte((const int8_t *)opt.content_reset, (uint8_t *)&p_metadata_payload->content_reset);
        sprintf(buffer, TOOL_DISPLAY_FORMAT_INTEGER, DESC_CONTENT_RESET, (uint8_t)p_metadata_payload->content_reset);
        pal_logger_print_message(buffer);

        (void)set_str_to_byte((const int8_t *)DEFAULT_ADDITIONAL_FLAG, &p_metadata_payload->additional_flag);

        status = pal_file_system_read_file_to_array_in_hex((const int8_t *)opt.metadata, &(p_metadata_payload->metadata), &(p_metadata_payload->metadata_length));
        if (0 != status)
        {
            pal_logger_print_message("Error : Unable to read meta data to be written to OID\n");
            break;
        }
        status = 0;
    } while (0);
    return status;
}

int32_t tool_get_user_inputs(int32_t argc, int8_t *argv[])
{
    int32_t status = 0;
    int32_t i;
    int8_t *p, *q;

    _tool_set_default_value();
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
            status = _tool_set_option(p, q);    
            if (0 != status)
            {
                break;
            }
        }
    } while (0);

    if (0 == status)
    {
        _tool_show_user_inputs();
    }
    else
    {
        _tool_show_usage();
    }
    return status;
}

int32_t tool_set_user_inputs(manifest_t* p_manifest_data)
{

    int32_t status = -1;
    pal_logger_print_message("Info : Setting value for data formatter\n");
    // set all structure pointer to NULL.

    p_manifest_data->p_confidentiality = NULL;
    // precedence is data, key and metadata
    p_manifest_data->p_data_payload = NULL;
    p_manifest_data->p_key_payload = NULL;
    p_manifest_data->p_metadata_payload = NULL;
    do
    {
        if (0 != _tool_set_manifest_info(p_manifest_data))
        {
            break;
        }

        //CONFIDENTIALITY
        // set your pointer for shared secret structure to NULL
        if (NULL != opt.shared_secret)
        {
            // create structure and assign to your pointer
            p_manifest_data->p_confidentiality = (confidentiality_t *)pal_os_malloc(sizeof(confidentiality_t));
            if (0 != _tool_set_confidentiality_info(p_manifest_data->p_confidentiality))
            {
                break;
            }
        }
            
        //PAYLOAD : Data
        if (NULL != opt.data)
        {
            p_manifest_data->p_data_payload = (data_payload_t *)pal_os_malloc(sizeof(data_payload_t));
            if (0 != _tool_set_object_data_info(p_manifest_data->p_data_payload))
            {
                break;
            }
        }
        //PAYLOAD : Key
        else if (NULL != opt.key_data)
        {
            p_manifest_data->p_key_payload = (key_payload_t *)pal_os_malloc(sizeof(key_payload_t));
            if (0 != _tool_set_key_data_info(p_manifest_data->p_key_payload))
            {
                break;
            }

            if( 0 != set_str_to_hex_one_byte((const int8_t *)opt.key_usage, (uint8_t *)&p_manifest_data->p_key_payload->key_usage))
            {
                pal_logger_print_message("Error : Invalid input key usage provided \n");
                break;
            }
            (void)set_str_to_byte_signed((const int8_t *)opt.key_algo, (uint8_t *)&p_manifest_data->p_key_payload->key_algorithm);
        }
        //PAYLOAD : Metadata
        else if (NULL != opt.metadata)
        {
            p_manifest_data->p_metadata_payload = (metadata_payload_t *)pal_os_malloc(sizeof(metadata_payload_t));
            if (0 != _tool_set_meta_data_info(p_manifest_data->p_metadata_payload))
            {
                break;
            }

        }
        else
        {
            pal_logger_print_message("Error : No data available for payload\n");
            break;
        }

        if(NULL != opt.dataset_to_file_path)
        {
              dataset_file_path = opt.dataset_to_file_path;
        }
        status = 0;
    } while (0);

    if (0 != status)
    {
        _tool_show_usage();
        (void)tool_clear_inputs(p_manifest_data);
    }

    return status;
}

int32_t tool_clear_inputs(manifest_t* p_manifest_data)
{
    int32_t status = -1;

    if (NULL != p_manifest_data->p_confidentiality)
    {
        if (NULL != p_manifest_data->p_confidentiality->kdf_data.label)
        {
            pal_os_free(p_manifest_data->p_confidentiality);
        }
    }
    if (NULL != p_manifest_data->p_data_payload)
    {
        pal_os_free(p_manifest_data->p_data_payload);
    }
    if (NULL != p_manifest_data->p_key_payload)
    {
        pal_os_free(p_manifest_data->p_key_payload);
    }
    if (NULL != p_manifest_data->p_metadata_payload)
    {
        pal_os_free(p_manifest_data->p_metadata_payload);
    }
    return status;
}

/**
* @}
*/

