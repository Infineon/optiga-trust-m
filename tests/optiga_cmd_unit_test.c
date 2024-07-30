/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_cmd_unit_test.c
 *
 * \brief   This file implements the OPTIGA Command unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_cmd_unit_test.h"

static const uint8_t ut_ecc_public_key_component[] = {
    // NIST-256 Public Key
    0x8b, 0x88, 0x9c, 0x1d, 0xd6, 0x07, 0x58, 0x2e, 0xd6, 0xf8, 0x2c, 0xc2, 0xd9, 0xbe, 0xd0, 0xfe,
    0x64, 0xf3, 0x24, 0x5e, 0x94, 0x7d, 0x54, 0xcd, 0x20, 0xdc, 0x58, 0x98, 0xcf, 0x51, 0x31, 0x44,
    0x22, 0xea, 0x01, 0xd4, 0x0b, 0x23, 0xb2, 0x45, 0x7c, 0x42, 0xdf, 0x3c, 0xfb, 0x0d, 0x33, 0x10,
    0xb8, 0x49, 0xb7, 0xaa, 0x0a, 0x85, 0xde, 0xe7, 0x6a, 0xf1, 0xac, 0x31, 0x31, 0x1e, 0x8c, 0x4b};

// SHA-256 digest to be signed
static const uint8_t ut_digest[] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

// SHA-256 Digest to be verified
static const uint8_t ut_verify_digest[] = {
    0xE9, 0x5F, 0xB3, 0xB1, 0x9F, 0xA4, 0xDD, 0x27, 0xFE, 0xAE, 0xB3, 0x33, 0x40, 0x80, 0xCE, 0x35,
    0xDF, 0x3E, 0x08, 0xF1, 0x6F, 0x36, 0xF3, 0x24, 0x0E, 0xB0, 0xB3, 0x2F, 0xAB, 0xD0, 0x90, 0xCA,
};

// ECDSA Signature
static const uint8_t ut_verify_signature[] = {
    // DER INTEGER Format
    0x02,
    // Length
    0x20,
    0x39,
    0xA4,
    0x70,
    0xE9,
    0x32,
    0x30,
    0xF5,
    0x5F,
    0xA4,
    0xDF,
    0x8A,
    0x07,
    0x36,
    0x58,
    0x65,
    0xC6,
    0xE6,
    0x1B,
    0x07,
    0x51,
    0xFB,
    0xC6,
    0x16,
    0x05,
    0xEB,
    0xDF,
    0x56,
    0x6D,
    0xA9,
    0x50,
    0x3B,
    0x24,
    // DER INTEGER Format
    0x02,
    // Length
    0x1E,
    0x49,
    0x33,
    0x6C,
    0x07,
    0x2B,
    0xD0,
    0x40,
    0x20,
    0x0F,
    0xD4,
    0xE0,
    0x7E,
    0x67,
    0x66,
    0xC4,
    0xF5,
    0x7F,
    0x98,
    0xEC,
    0x38,
    0xB8,
    0xEF,
    0x44,
    0x8F,
    0x6A,
    0xE1,
    0xFD,
    0x1E,
    0x92,
    0xB4,
};

// RSA 1024 public key
static const uint8_t ut_rsa_public_key_modulus[] = {
    // Public key modulus
    0xA1, 0xD4, 0x6F, 0xBA, 0x23, 0x18, 0xF8, 0xDC, 0xEF, 0x16, 0xC2, 0x80, 0x94, 0x8B, 0x1C, 0xF2,
    0x79, 0x66, 0xB9, 0xB4, 0x72, 0x25, 0xED, 0x29, 0x89, 0xF8, 0xD7, 0x4B, 0x45, 0xBD, 0x36, 0x04,
    0x9C, 0x0A, 0xAB, 0x5A, 0xD0, 0xFF, 0x00, 0x35, 0x53, 0xBA, 0x84, 0x3C, 0x8E, 0x12, 0x78, 0x2F,
    0xC5, 0x87, 0x3B, 0xB8, 0x9A, 0x3D, 0xC8, 0x4B, 0x88, 0x3D, 0x25, 0x66, 0x6C, 0xD2, 0x2B, 0xF3,
    0xAC, 0xD5, 0xB6, 0x75, 0x96, 0x9F, 0x8B, 0xEB, 0xFB, 0xCA, 0xC9, 0x3F, 0xDD, 0x92, 0x7C, 0x74,
    0x42, 0xB1, 0x78, 0xB1, 0x0D, 0x1D, 0xFF, 0x93, 0x98, 0xE5, 0x23, 0x16, 0xAA, 0xE0, 0xAF, 0x74,
    0xE5, 0x94, 0x65, 0x0B, 0xDC, 0x3C, 0x67, 0x02, 0x41, 0xD4, 0x18, 0x68, 0x45, 0x93, 0xCD, 0xA1,
    0xA7, 0xB9, 0xDC, 0x4F, 0x20, 0xD2, 0xFD, 0xC6, 0xF6, 0x63, 0x44, 0x07, 0x40, 0x03, 0xE2, 0x11,
};

// RSA 1024 public key
static const uint8_t ut_rsa_public_key_exponent[] = {
    // Public Exponent
    0x00,
    0x01,
    0x00,
    0x01};

// SHA-256 Digest
static const uint8_t ut_rsa_verify_digest[] = {
    0x91, 0x70, 0x02, 0x48, 0x3F, 0xBD, 0x5F, 0xDD, 0xD5, 0x38, 0xEB, 0xDA, 0x9A, 0x5E, 0x1F, 0x46,
    0xFC, 0xAD, 0x8F, 0x1E, 0x2C, 0x75, 0xB0, 0x83, 0xD0, 0x71, 0x2B, 0x80, 0xD4, 0xAA, 0xC6, 0x9B};

// RSA 1024 Signature ( no additional encoding needed )
static const uint8_t ut_rsa_signature[] = {
    0x5B, 0xDE, 0x46, 0xE4, 0x35, 0x48, 0xF4, 0x81, 0x45, 0x7C, 0x72, 0x31, 0x54, 0x55, 0xE8, 0x9F,
    0x1D, 0xD0, 0x5D, 0x9D, 0xEC, 0x40, 0xE6, 0x6B, 0x89, 0xF3, 0xBC, 0x52, 0x68, 0xB1, 0xD8, 0x70,
    0x35, 0x05, 0xFC, 0x98, 0xF6, 0x36, 0x99, 0x24, 0x53, 0xF0, 0x17, 0xB8, 0x9B, 0xD4, 0xA0, 0x5F,
    0x12, 0x04, 0x8A, 0xA1, 0xA7, 0x96, 0xE6, 0x33, 0xCA, 0x48, 0x84, 0xD9, 0x00, 0xE4, 0xA3, 0x8E,
    0x2F, 0x6F, 0x3F, 0x6D, 0xE0, 0x1D, 0xF8, 0xEA, 0xE0, 0x95, 0xBA, 0x63, 0x15, 0xED, 0x7B, 0x6A,
    0xB6, 0x6E, 0x20, 0x17, 0xB5, 0x64, 0xDE, 0x49, 0x64, 0x97, 0xCA, 0x5E, 0x4D, 0x84, 0x63, 0xA0,
    0xF1, 0x00, 0x6C, 0xEE, 0x70, 0x89, 0xD5, 0x6E, 0xC5, 0x05, 0x31, 0x0D, 0xAA, 0xB7, 0xBA, 0xA0,
    0xAA, 0xBF, 0x98, 0xE8, 0x39, 0x93, 0x70, 0x07, 0x2D, 0xFF, 0x42, 0xF9, 0xA4, 0x6F, 0x1B, 0x00};

// Peer public key details for the ECDH operation
static uint8_t ut_peer_public_key[] = {
    // Bit string format
    0x03,
    // Remaining length
    0x42,
    // Unused bits
    0x00,
    // Compression format
    0x04,
    // Public Key
    0x94,
    0x89,
    0x2F,
    0x09,
    0xEA,
    0x4E,
    0xCA,
    0xBC,
    0x6A,
    0x4E,
    0xF2,
    0x06,
    0x36,
    0x26,
    0xE0,
    0x5D,
    0xE0,
    0xD5,
    0xF9,
    0x77,
    0xEA,
    0xC3,
    0xB2,
    0x70,
    0xAC,
    0xE2,
    0x19,
    0x00,
    0xF5,
    0xDB,
    0x56,
    0xE7,
    0x37,
    0xBB,
    0xBE,
    0x46,
    0xE4,
    0x49,
    0x76,
    0x38,
    0x25,
    0xB5,
    0xF8,
    0x94,
    0x74,
    0x9E,
    0x1A,
    0xB6,
    0x5A,
    0xF1,
    0x29,
    0xD7,
    0x3A,
    0xB6,
    0x9B,
    0x80,
    0xAC,
    0xC5,
    0xE1,
    0xC3,
    0x10,
    0xF2,
    0x16,
    0xC6,
};

static const uint8_t ut_label[] = "Unit Test Derived Key";

static const uint8_t ut_random_seed[] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

static public_key_from_host_t ut_peer_public_key_details = {
    (uint8_t *)ut_peer_public_key,
    sizeof(ut_peer_public_key),
    (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256,
};

extern void optiga_cmd_set_shielded_connection_option(
    optiga_cmd_t *me,
    uint8_t value,
    uint8_t shielded_connection_option
);

static uint8_t ut_rsa_public_key[150] = {0x00};
static uint16_t ut_rsa_public_key_length = 0;

uint8_t ut_ecc_public_key[70] = {0x00};
uint16_t ut_ecc_public_key_length = 0;

void ut_encode_ecc_public_key_in_bit_string_format(
    const uint8_t *q_buffer,
    uint8_t q_length,
    uint8_t *pub_key_buffer,
    uint16_t *pub_key_length
) {
#define OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH (0x02)

    uint16_t index = 0;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG;
    pub_key_buffer[index++] = q_length + OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS;
    // Compression format. Supports only 04 [uncompressed]
    pub_key_buffer[index++] = 0x04;

    pal_os_memcpy(&pub_key_buffer[index], q_buffer, q_length);
    index += q_length;

    *pub_key_length = index;

#undef OPTIGA_EXAMPLE_UTIL_ECC_DER_ADDITIONAL_LENGTH
}

void ut_encode_rsa_public_key_in_bit_string_format(
    const uint8_t *n_buffer,
    uint16_t n_length,
    const uint8_t *e_buffer,
    uint16_t e_length,
    uint8_t *pub_key_buffer,
    uint16_t *pub_key_length
) {
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD (0x80)
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG (0x30)
#define OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG (0x02)

    uint16_t index = 0;
    uint16_t modulus_signed_bit_len = 0;
    uint8_t length_field_in_bytes = 0;

    // Check if the first byte of modulus is a negative integer
    modulus_signed_bit_len =
        ((OPTIGA_EXAMPLE_UTIL_RSA_NEGATIVE_INTEGER < n_buffer[0]) ? 0x01 : 0x00);

    // Check the number of bytes required to represent the length field
    length_field_in_bytes = ((n_length > 0xFF) ? 0x02 : 0x01);

    // RSA public key formation according to DER encoded format
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 7 bytes = Unused Bits(1 byte) + Sequence Tag (1 byte) +
    //           Length tag (1 byte) * 3 occurrence + Integer tag (1 byte) * 2 occurrence
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(
        pub_key_buffer,
        &index,
        (0x07 + n_length + modulus_signed_bit_len + e_length + (2 * length_field_in_bytes))
    )
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    // 4 bytes = Integer tag (1 byte) * 2 occurrence + Length tag (1 byte) * 2 occurrence
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(
        pub_key_buffer,
        &index,
        (0x04 + n_length + modulus_signed_bit_len + e_length + length_field_in_bytes)
    )

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG;
    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD + length_field_in_bytes;
    OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(pub_key_buffer, &index, (n_length + modulus_signed_bit_len))
    index += modulus_signed_bit_len;
    pal_os_memcpy(&pub_key_buffer[index], n_buffer, n_length);
    index += n_length;

    pub_key_buffer[index++] = OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG;
    pub_key_buffer[index++] = (uint8_t)e_length;
    pal_os_memcpy(&pub_key_buffer[index], e_buffer, e_length);
    index += e_length;

    *pub_key_length = index;

#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_MIN_LEN_FIELD
#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_SEQUENCE_TAG
#undef OPTIGA_EXAMPLE_UTIL_RSA_DER_INTEGER_TAG
}

static void ut_optiga_util_callback(void *context, optiga_lib_status_t return_status) {
    /* to remove warning for unused parameter */
    (void)(context);
    (void)(return_status);
}

void ut_optiga_cmd_get_data_objects(
    optiga_cmd_t *ut_optiga_cmd,
    optiga_get_data_object_params_t *ut_params
) {
    uint8_t ut_read_data_buffer[1024] = {0};
    uint16_t ut_bytes_to_read = sizeof(ut_read_data_buffer);
    optiga_lib_status_t ut_optiga_result;

    ut_params->oid = 0xE0E0;
    ut_params->offset = 0x00;
    ut_params->data_or_metadata = 0; /* Read data */
    ut_params->buffer = ut_read_data_buffer;
    ut_params->bytes_to_read = ut_bytes_to_read;
    ut_params->ref_bytes_to_read = &ut_bytes_to_read;
    ut_params->accumulated_size = 0;
    ut_params->last_read_size = 0;
    /* optiga_cmd_get_data_object check
     *  Read Data
     */
    ut_optiga_result =
        optiga_cmd_get_data_object(ut_optiga_cmd, ut_params->data_or_metadata, ut_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_get_data_object check
     *  Read Metadata
     */
    ut_params->oid = 0xE0E0;
    ut_params->data_or_metadata = 1; /* Read data */
    ut_bytes_to_read = sizeof(ut_read_data_buffer);
    ut_optiga_result =
        optiga_cmd_get_data_object(ut_optiga_cmd, ut_params->data_or_metadata, ut_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
}

void ut_optiga_cmd_set_data_objects(
    optiga_cmd_t *ut_optiga_cmd,
    optiga_set_data_object_params_t *ut_set_params
) {
    uint8_t ut_data_buffer[1024] = {0};
    uint16_t ut_bytes_to_read = sizeof(ut_data_buffer);
    optiga_lib_status_t ut_optiga_result;

    /* optiga_cmd_set_data_object check
     *  Set Data + Count data object
     */
    ut_set_params->count = ut_data_buffer[0];
    ut_bytes_to_read = sizeof(ut_data_buffer);
    ut_set_params->buffer = NULL;
    ut_set_params->oid = 0xE120;
    ut_set_params->offset = 0x00;
    ut_set_params->data_or_metadata = 0; /* for Data */
    ut_set_params->size = ut_bytes_to_read;
    ut_set_params->written_size = 0;
    ut_set_params->write_type = OPTIGA_UTIL_COUNT_DATA_OBJECT;
    ut_optiga_result =
        optiga_cmd_set_data_object(ut_optiga_cmd, ut_set_params->write_type, ut_set_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_set_data_object check
     *  Set Data + No Count data object
     */
    ut_set_params->count = 0;
    ut_set_params->buffer = ut_data_buffer;
    ut_set_params->oid = 0xE120;
    ut_set_params->offset = 0x00;
    ut_set_params->data_or_metadata = 0; /* for Data */
    ut_set_params->size = ut_bytes_to_read;
    ut_set_params->written_size = 0;
    ut_set_params->write_type = 0x00;
    ut_optiga_result =
        optiga_cmd_set_data_object(ut_optiga_cmd, ut_set_params->write_type, ut_set_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_set_data_object check
     *  Set Metadata + Count data object
     */
    ut_set_params->count = ut_data_buffer[0];
    ut_bytes_to_read = sizeof(ut_data_buffer);
    ut_set_params->buffer = NULL;
    ut_set_params->oid = 0xE120;
    ut_set_params->offset = 0x00;
    ut_set_params->data_or_metadata = 1; /* for Metadata */
    ut_set_params->size = ut_bytes_to_read;
    ut_set_params->written_size = 0;
    ut_set_params->write_type = OPTIGA_UTIL_COUNT_DATA_OBJECT;
    ut_optiga_result =
        optiga_cmd_set_data_object(ut_optiga_cmd, ut_set_params->write_type, ut_set_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_set_data_object check
     *  Set Metadata + No Count data object
     */
    ut_set_params->count = 0;
    ut_set_params->buffer = ut_data_buffer;
    ut_set_params->oid = 0xE120;
    ut_set_params->offset = 0x00;
    ut_set_params->data_or_metadata = 1; /* for Metadata */
    ut_set_params->size = ut_bytes_to_read;
    ut_set_params->written_size = 0;
    ut_set_params->write_type = 0x00;
    ut_optiga_result =
        optiga_cmd_set_data_object(ut_optiga_cmd, ut_set_params->write_type, ut_set_params);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
}

void ut_optiga_cmd_calc_hash(
    optiga_cmd_t *ut_optiga_cmd,
    optiga_calc_hash_params_t *ut_hash_param
) {
    optiga_lib_status_t ut_optiga_result;
    uint8_t ut_hash_context_buffer[OPTIGA_HASH_CONTEXT_LENGTH_SHA_256];
    optiga_hash_context_t ut_hash_context;
    uint8_t data_to_hash[] = {"OPTIGA, Infineon Technologies AG"};
    hash_data_from_host_t hash_data_host;
    hash_data_in_optiga_t hash_oid_data;
    uint8_t ut_output[32];

    /* Initialize the Hash context */
    ut_hash_context.context_buffer = ut_hash_context_buffer;
    ut_hash_context.context_buffer_length = sizeof(ut_hash_context_buffer);
    ut_hash_context.hash_algo = OPTIGA_HASH_TYPE_SHA_256;

    /* HASH start */
    ut_hash_param->hash_sequence = OPTIGA_CRYPT_HASH_START;
    ut_hash_param->p_hash_context = &ut_hash_context;
    ut_hash_param->apparent_context_size = 0;
    ut_hash_param->p_hash_data = NULL;
    ut_hash_param->p_hash_oid = NULL;
    ut_hash_param->p_out_digest = NULL;
    ut_hash_param->export_hash_ctx = TRUE;

    ut_optiga_result =
        optiga_cmd_calc_hash(ut_optiga_cmd, ut_hash_context.hash_algo, ut_hash_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* HASH continue */
    ut_hash_param->hash_sequence = OPTIGA_CRYPT_HASH_CONTINUE;
    hash_data_host.buffer = data_to_hash;
    hash_data_host.length = sizeof(data_to_hash);
    ut_hash_param->p_hash_data = (hash_data_from_host_t *)&hash_data_host;
    ut_optiga_result =
        optiga_cmd_calc_hash(ut_optiga_cmd, ut_hash_context.hash_algo, ut_hash_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* HASH continue : old data*/
    ut_hash_param->hash_sequence |= OPTIGA_CRYPT_HASH_FOR_OID;
    ut_hash_param->p_hash_oid = (hash_data_in_optiga_t *)&hash_oid_data;
    ut_hash_param->hash_sequence = OPTIGA_CRYPT_HASH_CONTINUE;
    hash_oid_data.oid = 0;
    hash_oid_data.offset = 0x00;
    hash_oid_data.length = 5;
    ut_optiga_result =
        optiga_cmd_calc_hash(ut_optiga_cmd, ut_hash_context.hash_algo, ut_hash_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* HASH Final */
    ut_hash_param->hash_sequence = OPTIGA_CRYPT_HASH_FINAL;
    ut_hash_param->p_hash_data = NULL;
    ut_hash_param->export_hash_ctx = FALSE;
    ut_hash_param->p_out_digest = ut_output;
    ut_optiga_result =
        optiga_cmd_calc_hash(ut_optiga_cmd, ut_hash_context.hash_algo, ut_hash_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
}

void ut_optiga_encrypt_sym_fct(
    optiga_cmd_t *ut_optiga_cmd,
    optiga_encrypt_sym_params_t *ut_encrypt_sym_param
) {
    optiga_lib_status_t ut_optiga_result;
    const uint8_t plain_data_buffer[] = {
        0x6b,
        0xc1,
        0xbe,
        0xe2,
        0x2e,
        0x40,
        0x9f,
        0x96,
        0xe9,
        0x3d,
        0x7e,
        0x11,
        0x73,
        0x93,
        0x17,
        0x2a};
    const uint8_t plain_data_buffer_start[] = {
        0x6b,
        0xc1,
        0xbe,
        0xe2,
        0x2e,
        0x40,
        0x9f,
        0x96,
        0xe9,
        0x3d,
        0x7e,
        0x11,
        0x73,
        0x93,
        0x17,
        0x2a};
    const uint8_t plain_data_buffer_continue[] = {
        0x7c,
        0xd2,
        0xcf,
        0xf3,
        0x3f,
        0x51,
        0xa0,
        0xa7,
        0xf0,
        0x4e,
        0x8f,
        0x22,
        0x84,
        0xa4,
        0x28,
        0x3b};
    const uint8_t plain_data_buffer_final[] = {
        0x5a,
        0xb0,
        0xad,
        0xd1,
        0x1d,
        0x3f,
        0x8e,
        0x85,
        0xd8,
        0x2c,
        0x6d,
        0xf1,
        0x62,
        0x82,
        0x06,
        0x19};

    uint8_t encrypted_data_buffer_start[32] = {0};
    uint32_t encrypted_data_length_start = sizeof(encrypted_data_buffer_start);
    uint8_t encrypted_data_buffer_continue[32] = {0};
    uint32_t encrypted_data_length_continue = sizeof(encrypted_data_buffer_continue);
    uint8_t encrypted_data_buffer_final[32] = {0};
    uint32_t encrypted_data_length_final = sizeof(encrypted_data_buffer_final);
    uint8_t encrypted_data_buffer[32] = {0};
    uint32_t encrypted_data_length = sizeof(encrypted_data_buffer);

    /* optiga_cmd_encrypt_sym check : OPTIGA_CRYPT_SYM_START_FINAL*/
    ut_encrypt_sym_param->mode = OPTIGA_SYMMETRIC_CBC;
    ut_encrypt_sym_param->symmetric_key_oid = OPTIGA_KEY_ID_SECRET_BASED;
    ut_encrypt_sym_param->in_data = plain_data_buffer;
    ut_encrypt_sym_param->in_data_length = sizeof(plain_data_buffer);
    ut_encrypt_sym_param->iv = NULL;
    ut_encrypt_sym_param->iv_length = 0;
    ut_encrypt_sym_param->associated_data = NULL;
    ut_encrypt_sym_param->associated_data_length = 0;
    ut_encrypt_sym_param->total_input_data_length = 0;
    ut_encrypt_sym_param->out_data = encrypted_data_buffer;
    ut_encrypt_sym_param->out_data_length = &encrypted_data_length;
    ut_encrypt_sym_param->generated_hmac = NULL;
    ut_encrypt_sym_param->generated_hmac_length = 0;
    ut_encrypt_sym_param->original_sequence = OPTIGA_CRYPT_SYM_START_FINAL;
    ut_encrypt_sym_param->operation_mode = OPTIGA_CRYPT_SYMMETRIC_ENCRYPTION;
    ut_optiga_result = optiga_cmd_encrypt_sym(
        ut_optiga_cmd,
        (uint8_t)ut_encrypt_sym_param->mode,
        ut_encrypt_sym_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_encrypt_sym : OPTIGA_CRYPT_SYM_START */
    ut_encrypt_sym_param->in_data = plain_data_buffer_start;
    ut_encrypt_sym_param->in_data_length = sizeof(plain_data_buffer_start);
    ut_encrypt_sym_param->out_data = encrypted_data_buffer_start;
    ut_encrypt_sym_param->out_data_length = &encrypted_data_length_start;
    ut_encrypt_sym_param->original_sequence = OPTIGA_CRYPT_SYM_START;
    ut_optiga_result = optiga_cmd_encrypt_sym(
        ut_optiga_cmd,
        (uint8_t)ut_encrypt_sym_param->mode,
        ut_encrypt_sym_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
    optiga_cmd_set_shielded_connection_option(
        ut_optiga_cmd,
        OPTIGA_SET_PROTECTION_LEVEL,
        OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL
    );

    /* optiga_cmd_encrypt_sym : OPTIGA_CRYPT_SYM_CONTINUE */
    ut_encrypt_sym_param->mode = 0;
    ut_encrypt_sym_param->symmetric_key_oid = 0;
    ut_encrypt_sym_param->in_data = plain_data_buffer_continue;
    ut_encrypt_sym_param->in_data_length = sizeof(plain_data_buffer_continue);
    ut_encrypt_sym_param->out_data = encrypted_data_buffer_continue;
    ut_encrypt_sym_param->out_data_length = &encrypted_data_length_continue;
    ut_encrypt_sym_param->original_sequence = OPTIGA_CRYPT_SYM_CONTINUE;
    ut_optiga_result = optiga_cmd_encrypt_sym(
        ut_optiga_cmd,
        (uint8_t)ut_encrypt_sym_param->mode,
        ut_encrypt_sym_param
    );
    printf(" ut_optiga_result : %d \n\r", ut_optiga_result);
    // TODO : assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
    optiga_cmd_set_shielded_connection_option(
        ut_optiga_cmd,
        OPTIGA_SET_PROTECTION_LEVEL,
        OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL
    );

    /* optiga_cmd_encrypt_sym : OPTIGA_CRYPT_SYM_FINAL */
    ut_encrypt_sym_param->mode = 0;
    ut_encrypt_sym_param->symmetric_key_oid = 0;
    ut_encrypt_sym_param->in_data = plain_data_buffer_final;
    ut_encrypt_sym_param->in_data_length = sizeof(plain_data_buffer_final);
    ut_encrypt_sym_param->out_data = encrypted_data_buffer_final;
    ut_encrypt_sym_param->out_data_length = &encrypted_data_length_final;
    ut_encrypt_sym_param->original_sequence = OPTIGA_CRYPT_SYM_FINAL;
    ut_optiga_result = optiga_cmd_release_lock(ut_optiga_cmd);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
    ut_optiga_result = optiga_cmd_encrypt_sym(
        ut_optiga_cmd,
        (uint8_t)ut_encrypt_sym_param->mode,
        ut_encrypt_sym_param
    );
    // TODO : assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
}

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    optiga_cmd_t *ut_optiga_cmd;
    optiga_lib_status_t ut_optiga_result;
    optiga_get_data_object_params_t *ut_params = malloc(sizeof(optiga_get_data_object_params_t));
    optiga_set_data_object_params_t *ut_set_params =
        malloc(sizeof(optiga_set_data_object_params_t));
    optiga_get_random_params_t *ut_random_param = malloc(sizeof(optiga_get_random_params_t));
    optiga_calc_hash_params_t *ut_hash_param = malloc(sizeof(optiga_calc_hash_params_t));
    optiga_calc_sign_params_t *ut_sign_param = malloc(sizeof(optiga_calc_sign_params_t));
    optiga_verify_sign_params_t *ut_verify_param = malloc(sizeof(optiga_verify_sign_params_t));
    optiga_calc_ssec_params_t *ut_ssec_param = malloc(sizeof(optiga_calc_ssec_params_t));
    optiga_derive_key_params_t *ut_derive_key_param = malloc(sizeof(optiga_derive_key_params_t));
    optiga_gen_keypair_params_t *ut_gen_keypair_param = malloc(sizeof(optiga_gen_keypair_params_t));
    optiga_encrypt_asym_params_t *ut_encrypt_asym_param =
        malloc(sizeof(optiga_encrypt_asym_params_t));
    optiga_set_object_protected_params_t *ut_set_object_protected_param =
        malloc(sizeof(optiga_set_object_protected_params_t));
    optiga_encrypt_sym_params_t *ut_encrypt_sym_param = malloc(sizeof(optiga_encrypt_sym_params_t));
    const uint8_t ut_optional_data[] = {0x01, 0x02};
    uint16_t ut_pre_master_secret_length = 30;
    uint8_t ut_signature[200];
    uint16_t ut_signature_length = sizeof(ut_signature);
    char_t *ut_manifest = "Optiga Trust - Unit Tests";

    /* optiga_cmd_create check
     *  Nothing to be checked here, just for coverage
     */
    ut_optiga_cmd = optiga_cmd_create(0, ut_optiga_util_callback, NULL);

    /* optiga_cmd_release_lock check */
    ut_optiga_result = optiga_cmd_release_lock(ut_optiga_cmd);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_open_application check
     *  Restore set to False
     */
    ut_optiga_result = optiga_cmd_open_application(ut_optiga_cmd, FALSE, NULL);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_release_lock check
     *  Hibernate set to False
     */
    ut_optiga_result = optiga_cmd_close_application(ut_optiga_cmd, FALSE, NULL);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_release_lock check
     *  Restore set to True
     */
    ut_optiga_result = optiga_cmd_open_application(ut_optiga_cmd, TRUE, NULL);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_get_data_object check
    Reads data or metadata of the specified data object, by issuing the GetDataObject command, based on input parameters.
    * - Acquires the OPTIGA lock for #optiga_util_read_data/#optiga_util_read_metadata.
    * - Forms the ReadData/ReadMetaData commands based on inputs.
    * - Issues the ReadData/ReadMetaData command through #optiga_comms_transceive.
    * - In case the data to be read by the user is greater than the <b>OPTIGA_MAX_COMMS_BUFFER_SIZE,
    *   the data is retrieved by internal chaining (by calling GetDataObject command multiple times).
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    ut_optiga_cmd_get_data_objects(ut_optiga_cmd, ut_params);

    /* optiga_cmd_set_data_object check
    Writes data or metadata to the specified data object, by issuing the SetDataObject command, based on input parameters.
    * - Acquires the OPTIGA lock for #optiga_util_write_data/#optiga_util_write_metadata.
    * - Forms the SetData/SetMetaData commands based on inputs.
    * - Issues the SetData/SetMetaData command through #optiga_comms_transceive.
    * - In case the data to be written by the user is greater than the <b>OPTIGA_MAX_COMMS_BUFFER_SIZE
    *   the data is written to the OPTIGA by internal chaining (by calling SetDataObject command multiple times).
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    ut_optiga_cmd_set_data_objects(ut_optiga_cmd, ut_set_params);

    /* optiga_cmd_get_random check
    Generates random data or pre-master secret for RSA key exchange, by issuing the GetRandom command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_rsa_generate_pre_master_secret/#optiga_crypt_random.
    * - For pre-master secret, it acquires a session, else acquires the lock.
    * - Forms the GetRandom command based on inputs.
    * - Issues the GetRandom command through #optiga_comms_transceive.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    ut_random_param->optional_data = ut_optional_data;
    ut_random_param->optional_data_length = sizeof(ut_optional_data);
    ut_random_param->random_data = NULL;
    ut_random_param->random_data_length = (ut_pre_master_secret_length - sizeof(ut_optional_data));
    ut_random_param->store_in_session = TRUE;
    ut_optiga_result = optiga_cmd_get_random(
        ut_optiga_cmd,
        OPTIGA_CRYTP_RANDOM_PARAM_PRE_MASTER_SECRET,
        ut_random_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_calc_hash check
    Generates hash on external data or data in OID by issuing Calc Hash command to OPTIGA.
    * - Acquires the OPTIGA lock for #optiga_crypt_hash_start/#optiga_crypt_hash_update/#optiga_crypt_hash_finalize.
    * - Forms the CalcHash command based on inputs.
    * - Issues the CalcHash command through #optiga_comms_transceive.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    ut_optiga_cmd_calc_hash(ut_optiga_cmd, ut_hash_param);

    /* optiga_cmd_calc_sign check
    Calculate signature on digest by issuing Calc Sign command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdsa_sign/#optiga_crypt_rsa_sign.
    * - Forms the Generate KeyPair command based on inputs.
    * - Issues the Generate KeyPair command through #optiga_comms_transceive.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    ut_sign_param->p_digest = ut_digest;
    ut_sign_param->digest_length = sizeof(ut_digest);
    ut_sign_param->private_key_oid = OPTIGA_KEY_ID_E0FC;
    ut_sign_param->p_signature = ut_signature;
    ut_sign_param->p_signature_length = &ut_signature_length;
    ut_optiga_result =
        optiga_cmd_calc_sign(ut_optiga_cmd, OPTIGA_RSASSA_PKCS1_V15_SHA256, ut_sign_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_verify_sign check : ECDSA
    Verifies the signature over the given digest by issuing VerifySign command.
    * - Acquires the OPTIGA lock for #optiga_crypt_ecdsa_verify/#optiga_crypt_rsa_verify.
    * - Forms the VerifySign command based on inputs and invokes the optiga_comms_transceive to send the same to OPTIGA.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    * */
    ut_verify_param->p_digest = ut_verify_digest;
    ut_verify_param->digest_length = sizeof(ut_verify_digest);
    ut_verify_param->p_signature = ut_verify_signature;
    ut_verify_param->signature_length = sizeof(ut_verify_signature);
    ut_verify_param->public_key_source_type = OPTIGA_CRYPT_HOST_DATA;
    ut_encode_ecc_public_key_in_bit_string_format(
        ut_ecc_public_key_component,
        sizeof(ut_ecc_public_key_component),
        ut_ecc_public_key,
        &ut_ecc_public_key_length
    );
    public_key_from_host_t ut_ecdsa_public_key_details = {
        ut_ecc_public_key,
        ut_ecc_public_key_length,
        (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256};
    ut_verify_param->public_key = (public_key_from_host_t *)&ut_ecdsa_public_key_details;
    ut_optiga_result = optiga_cmd_verify_sign(
        ut_optiga_cmd,
        OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
        ut_verify_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_verify_sign check : RSA
    Verifies the signature over the given digest by issuing VerifySign command.
    * - Acquires the OPTIGA lock for #optiga_crypt_ecdsa_verify/#optiga_crypt_rsa_verify.
    * - Forms the VerifySign command based on inputs and invokes the optiga_comms_transceive to send the same to OPTIGA.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    * */
    ut_verify_param->p_digest = ut_rsa_verify_digest;
    ut_verify_param->digest_length = sizeof(ut_rsa_verify_digest);
    ut_verify_param->p_signature = ut_rsa_signature;
    ut_verify_param->signature_length = sizeof(ut_rsa_signature);
    ut_verify_param->public_key_source_type = OPTIGA_CRYPT_HOST_DATA;
    ut_encode_rsa_public_key_in_bit_string_format(
        ut_rsa_public_key_modulus,
        sizeof(ut_rsa_public_key_modulus),
        ut_rsa_public_key_exponent,
        sizeof(ut_rsa_public_key_exponent),
        ut_rsa_public_key,
        &ut_rsa_public_key_length
    );
    public_key_from_host_t ut_rsa_public_key_details = {
        ut_rsa_public_key,
        ut_rsa_public_key_length,
        (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL};
    ut_verify_param->public_key = (public_key_from_host_t *)&ut_rsa_public_key_details;
    ut_optiga_result = optiga_cmd_verify_sign(
        ut_optiga_cmd,
        OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH,
        ut_verify_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_calc_ssec check
    Calculates shared secret by issuing CalcSSec command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdh.<br>
    * - Forms the command apdu based on inputs.<br>
    * - Issues the command through #optiga_comms_transceive.<br>
    * - If the private key is session based or shared secret is to be stored in OPTIGA, it acquires a session if not already available.<br>
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
    */
    uint8_t ut_shared_secret[32];
    ut_ssec_param->private_key = 0xE102;
    ut_ssec_param->public_key = &ut_peer_public_key_details;
    ut_ssec_param->export_to_host = TRUE;
    ut_ssec_param->shared_secret = ut_shared_secret;
    ut_optiga_result = optiga_cmd_calc_ssec(
        ut_optiga_cmd,
        OPTIGA_CRYPT_ECDH_KEY_AGREEMENT_ALGORITHM,
        ut_ssec_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_derive_key check
    Derives a key by issuing DeriveKey command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdh.
    * - Issues the calculate shared secret command through #optiga_comms_transceive.
    * - Based on the shared secret location in OPTIGA and target storage, it requests to acquire a session.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    uint8_t ut_decryption_key[16] = {0};
    ut_derive_key_param->input_shared_secret_oid = 0xF1D0;
    ut_derive_key_param->random_data = ut_random_seed;
    ut_derive_key_param->random_data_length = sizeof(ut_random_seed);
    ut_derive_key_param->label = ut_label;
    ut_derive_key_param->label_length = sizeof(ut_label);
    ut_derive_key_param->info = NULL;
    ut_derive_key_param->info_length = 0;
    ut_derive_key_param->derived_key_length = sizeof(ut_decryption_key);
    ut_derive_key_param->derived_key = (uint8_t *)ut_decryption_key;
    /* optiga_cmd_derive_key : PRF SHA-256 check */
    ut_optiga_result =
        optiga_cmd_derive_key(ut_optiga_cmd, OPTIGA_TLS12_PRF_SHA_256, ut_derive_key_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
    /* optiga_cmd_derive_key : PRF SHA-384 check */
    ut_optiga_result =
        optiga_cmd_derive_key(ut_optiga_cmd, OPTIGA_TLS12_PRF_SHA_384, ut_derive_key_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
    /* optiga_cmd_derive_key : PRF SHA-512 check */
    ut_optiga_result =
        optiga_cmd_derive_key(ut_optiga_cmd, OPTIGA_TLS12_PRF_SHA_512, ut_derive_key_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_gen_keypair check
    Generate ECC/RSA key-pair by issuing Generate KeyPair command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_ecc_generate_keypair/#optiga_crypt_rsa_generate_keypair.
    * - Forms the Generate KeyPair command based on inputs.
    * - Issues the Generate KeyPair command through #optiga_comms_transceive.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.
    */
    uint8_t ut_public_key[100];
    uint16_t ut_public_key_length = sizeof(ut_public_key);
    optiga_key_id_t optiga_key_id;
    optiga_key_id = OPTIGA_KEY_ID_E0F1;
    ut_gen_keypair_param->key_usage = (uint8_t)OPTIGA_KEY_USAGE_SIGN;
    ut_gen_keypair_param->export_private_key = FALSE;

    if (ut_gen_keypair_param->export_private_key) {
        ut_gen_keypair_param->private_key = (uint8_t *)&optiga_key_id;
    } else {
        ut_gen_keypair_param->private_key_oid = (optiga_key_id_t)(*((uint16_t *)&optiga_key_id));
    }

    ut_gen_keypair_param->public_key = ut_public_key;
    ut_gen_keypair_param->public_key_length = &ut_public_key_length;
    ut_optiga_result =
        optiga_cmd_gen_keypair(ut_optiga_cmd, OPTIGA_ECC_CURVE_NIST_P_256, ut_gen_keypair_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_encrypt_asym, optiga_cmd_decrypt_asym check
    Encrypts data using RSA public key by issuing Encrypt Asym command to OPTIGA.
    * - Acquires the OPTIGA session/lock for #optiga_crypt_rsa_encrypt_message/#optiga_crypt_rsa_encrypt_session.<br>
    * - Forms the Encrypt Asym command based on inputs.
    * - Issues the Encrypt Asym command through #optiga_comms_transceive.
    * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
    */
    uint8_t loop_count = 0;
    public_key_from_host_t public_key;
    uint8_t encrypted_message[256];
    size_t encrypted_message_length = sizeof(encrypted_message);
    uint8_t rsa_plaintext[] = {"UT RSA PKCS1_v1.5 Encryption user message"};
    uint8_t decrypted_message[256];
    uint16_t decrypted_message_length = sizeof(decrypted_message);
    optiga_rsa_key_type_t rsa_key_type[] = {
        OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
        OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL};

    public_key.public_key = ut_gen_keypair_param->public_key;
    public_key.length = ut_public_key_length;
    public_key.key_type = (uint8_t)rsa_key_type[loop_count];

    ut_encrypt_asym_param->message = rsa_plaintext;
    ut_encrypt_asym_param->message_length = sizeof(rsa_plaintext);
    ut_encrypt_asym_param->processed_message = encrypted_message;
    ut_encrypt_asym_param->processed_message_length = (uint16_t *)&encrypted_message_length;
    ut_encrypt_asym_param->key = &public_key;
    ut_encrypt_asym_param->public_key_source_type = OPTIGA_CRYPT_HOST_DATA;
    ut_optiga_result =
        optiga_cmd_encrypt_asym(ut_optiga_cmd, OPTIGA_RSAES_PKCS1_V15, ut_encrypt_asym_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_decrypt_asym */
    ut_encrypt_asym_param->message = encrypted_message;
    ut_encrypt_asym_param->message_length = encrypted_message_length;
    ut_encrypt_asym_param->processed_message = decrypted_message;
    ut_encrypt_asym_param->processed_message_length = &decrypted_message_length;
    ut_encrypt_asym_param->private_key_id = optiga_key_id;
    ut_encrypt_asym_param->key = &ut_encrypt_asym_param->private_key_id;
    ut_encrypt_asym_param->public_key_source_type = 0;
    ut_optiga_result =
        optiga_cmd_decrypt_asym(ut_optiga_cmd, OPTIGA_RSAES_PKCS1_V15, ut_encrypt_asym_param);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_set_object_protected check
    Writes manifest or data fragement(s) to the specified data object, by issuing the SetObjectProtected command, in a securely.
    * - Acquires the strict sequence for #optiga_util_protected_update_start.
    * - Forms the SetObjectProtected commands based on inputs.
    * - Issues the SetObjectProtected command through #optiga_comms_transceive.
    * - Releases the strict sequence in case of an error or after #optiga_util_protected_update_final is successfully completed.
    */
    ut_set_object_protected_param->manifest_version = 0x00;
    ut_set_object_protected_param->p_protected_update_buffer = (const uint8_t *)ut_manifest;
    ut_set_object_protected_param->p_protected_update_buffer_length = sizeof(ut_manifest);
    ut_set_object_protected_param->set_obj_protected_tag = OPTIGA_SET_PROTECTED_UPDATE_START;
    ut_optiga_result = optiga_cmd_set_object_protected(
        ut_optiga_cmd,
        ut_set_object_protected_param->manifest_version,
        ut_set_object_protected_param
    );
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_encrypt_sym check
    Encrypts data using selected encryption scheme by issuing Encrypt Sym command to OPTIGA.
    * - Acquires the strict sequence.
    * - Forms the Encrypt Sym command based on inputs.
    * - Issues the Encrypt Sym command through #optiga_comms_transceive.
    * - Releases the strict sequence in case of an error or after #optiga_crypt_symmetric_encrypt_final is successfully completed.
    */
    ut_optiga_encrypt_sym_fct(ut_optiga_cmd, ut_encrypt_sym_param);

    /* optiga_cmd_release_lock check
     *  Hibernate set to True
     */
    ut_optiga_result = optiga_cmd_close_application(ut_optiga_cmd, TRUE, NULL);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);

    /* optiga_cmd_release_lock check */
    ut_optiga_result = optiga_cmd_destroy(ut_optiga_cmd);
    assert(ut_optiga_result == OPTIGA_LIB_SUCCESS);
}