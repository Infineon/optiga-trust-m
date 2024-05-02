/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_crypt_integration_test.c
 *
 * \brief   This file implements the OPTIGA crypt integration tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_crypt_integration_test.h"

/**
 * Sample metadata
 */
static const uint8_t metadata[] = {
    // Metadata tag in the data object
    0x20,
    0x06,
    // Data object type set to PRESSEC
    0xE8,
    0x01,
    0x21,
    0xD3,
    0x01,
    0x00,
};

/**
 * Default metadata
 */
static const uint8_t default_metadata[] = {
    // Metadata tag in the data object
    0x20,
    0x06,
    // Data object type set to BSTR
    0xE8,
    0x01,
    0x00,
    0xD3,
    0x01,
    0xFF,
};

// RSA 1024 public key
static const uint8_t rsa_public_key_modulus[] = {
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
static const uint8_t rsa_public_key_exponent[] = {
    // Public Exponent
    0x00,
    0x01,
    0x00,
    0x01};

// SHA-256 Digest
static const uint8_t digest[] = {0x91, 0x70, 0x02, 0x48, 0x3F, 0xBD, 0x5F, 0xDD, 0xD5, 0x38, 0xEB,
                                 0xDA, 0x9A, 0x5E, 0x1F, 0x46, 0xFC, 0xAD, 0x8F, 0x1E, 0x2C, 0x75,
                                 0xB0, 0x83, 0xD0, 0x71, 0x2B, 0x80, 0xD4, 0xAA, 0xC6, 0x9B};

// RSA 1024 Signature ( no additional encoding needed )
static const uint8_t signature[] = {
    0x5B, 0xDE, 0x46, 0xE4, 0x35, 0x48, 0xF4, 0x81, 0x45, 0x7C, 0x72, 0x31, 0x54, 0x55, 0xE8, 0x9F,
    0x1D, 0xD0, 0x5D, 0x9D, 0xEC, 0x40, 0xE6, 0x6B, 0x89, 0xF3, 0xBC, 0x52, 0x68, 0xB1, 0xD8, 0x70,
    0x35, 0x05, 0xFC, 0x98, 0xF6, 0x36, 0x99, 0x24, 0x53, 0xF0, 0x17, 0xB8, 0x9B, 0xD4, 0xA0, 0x5F,
    0x12, 0x04, 0x8A, 0xA1, 0xA7, 0x96, 0xE6, 0x33, 0xCA, 0x48, 0x84, 0xD9, 0x00, 0xE4, 0xA3, 0x8E,
    0x2F, 0x6F, 0x3F, 0x6D, 0xE0, 0x1D, 0xF8, 0xEA, 0xE0, 0x95, 0xBA, 0x63, 0x15, 0xED, 0x7B, 0x6A,
    0xB6, 0x6E, 0x20, 0x17, 0xB5, 0x64, 0xDE, 0x49, 0x64, 0x97, 0xCA, 0x5E, 0x4D, 0x84, 0x63, 0xA0,
    0xF1, 0x00, 0x6C, 0xEE, 0x70, 0x89, 0xD5, 0x6E, 0xC5, 0x05, 0x31, 0x0D, 0xAA, 0xB7, 0xBA, 0xA0,
    0xAA, 0xBF, 0x98, 0xE8, 0x39, 0x93, 0x70, 0x07, 0x2D, 0xFF, 0x42, 0xF9, 0xA4, 0x6F, 0x1B, 0x00};

static uint8_t rsa_public_key[150] = {0x00};
static uint16_t rsa_public_key_length = 0;

static const uint8_t ecc_public_key_component[] = {
    // NIST-256 Public Key
    0x8b, 0x88, 0x9c, 0x1d, 0xd6, 0x07, 0x58, 0x2e, 0xd6, 0xf8, 0x2c, 0xc2, 0xd9, 0xbe, 0xd0, 0xfe,
    0x64, 0xf3, 0x24, 0x5e, 0x94, 0x7d, 0x54, 0xcd, 0x20, 0xdc, 0x58, 0x98, 0xcf, 0x51, 0x31, 0x44,
    0x22, 0xea, 0x01, 0xd4, 0x0b, 0x23, 0xb2, 0x45, 0x7c, 0x42, 0xdf, 0x3c, 0xfb, 0x0d, 0x33, 0x10,
    0xb8, 0x49, 0xb7, 0xaa, 0x0a, 0x85, 0xde, 0xe7, 0x6a, 0xf1, 0xac, 0x31, 0x31, 0x1e, 0x8c, 0x4b};

// SHA-256 Digest
static const uint8_t ecc_digest[] = {
    0xE9, 0x5F, 0xB3, 0xB1, 0x9F, 0xA4, 0xDD, 0x27, 0xFE, 0xAE, 0xB3, 0x33, 0x40, 0x80, 0xCE, 0x35,
    0xDF, 0x3E, 0x08, 0xF1, 0x6F, 0x36, 0xF3, 0x24, 0x0E, 0xB0, 0xB3, 0x2F, 0xAB, 0xD0, 0x90, 0xCA,
};

// ECDSA Signature
static const uint8_t ecc_signature[] = {
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

uint8_t ecc_public_key[70] = {0x00};
uint16_t ecc_public_key_length = 0;

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_lib_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}
static const uint8_t label[] = "Firmware update";

static const uint8_t random_seed[] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

// Secret to be written to data object which will be
// later used as part of TLS PRF SHA256 key derivation
static const uint8_t secret_to_be_written[] = {
    0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x61, 0xC7, 0x04, 0xE0, 0x0D, 0x82, 0x8B, 0x7A, 0x36, 0x41,
    0xD5, 0xCD, 0x7A, 0x38, 0x46, 0xDE, 0xF9, 0x0F, 0x21, 0x42, 0x40, 0x25, 0x0A, 0xAF, 0x9C, 0x2E,
};

static const uint8_t info[] = {
    0x8D,
    0xE4,
    0x3F,
    0xFF,
    0x65,
    0x2D,
    0xA0,
    0xA7,
    0xF0,
    0x4E,
    0x8F,
    0x22,
    0x84,
    0xA4,
    0x28,
    0x3B};

static const uint8_t salt[] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

/**
 * Sample metadata of 0xE200
 */
const uint8_t E200_metadata[] = {0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00};
/**
 * Sample metadata of 0xE0F1
 */
const uint8_t E0FC_metadata[] = {0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00};
/**
 * Sample metadata of 0xE0F1
 */
const uint8_t E0F1_metadata[] = {0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00};

const uint8_t message[] = {"RSA PKCS1_v1.5 Encryption of user message"};

// Peer public key details for the ECDH operation
static uint8_t peer_public_key[] = {
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
static public_key_from_host_t peer_public_key_details = {
    (uint8_t *)peer_public_key,
    sizeof(peer_public_key),
    (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256,
};

pal_status_t CalcHMAC(
    const uint8_t *secret_key,
    uint16_t secret_key_len,
    const uint8_t *input_data,
    uint32_t input_data_length,
    uint8_t *hmac
) {
    pal_status_t return_value = PAL_STATUS_FAILURE;

    const mbedtls_md_info_t *hmac_info;
    mbedtls_md_type_t digest_type;

    do {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == input_data) || (NULL == hmac)) {
            break;
        }
#endif  // OPTIGA_LIB_DEBUG_NULL_CHECK

        digest_type = OPTIGA_HMAC_SHA_256;

        hmac_info = mbedtls_md_info_from_type(digest_type);

        if (0
            != mbedtls_md_hmac(
                hmac_info,
                secret_key,
                secret_key_len,
                input_data,
                input_data_length,
                hmac
            )) {
            break;
        }

        return_value = PAL_STATUS_SUCCESS;

    } while (FALSE);

    return return_value;
}

/**
 * Metadata for Secret OID :
 * Execute access condition = Always
 * Data object type  =  Auto Ref
 */
static const uint8_t secret_oid_metadata[] = {0x20, 0x06, 0xD3, 0x01, 0x00, 0xE8, 0x01, 0x31};

/**
 * Shared secret data
 */
static const uint8_t user_secret[] = {
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27,
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6,
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27,
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6};

/**
 * Optional data
 */
static const uint8_t optional_data[] = {
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    0x10};

/**
 * random data
 */
static uint8_t random_data[32] = {0x00};

/**
 * Input data
 */
static uint8_t input_data_buffer[100] = {0x00};
/**
 * Arbitrary data
 */
static uint8_t arbitrary_data[16] = {0x00};

/**
 * Generated hmac
 */
static uint8_t hmac_buffer[32] = {0x00};
static const uint32_t hmac_length = sizeof(hmac_buffer);

optiga_lib_status_t ut_check_tag_in_metadata(
    const uint8_t *buffer,
    const uint8_t buffer_length,
    const uint8_t tag,
    bool_t *tag_available
) {
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    uint8_t offset = 1;
    uint8_t expected_buffer_length = buffer[offset++] + 0x02;  // 0x02 is length for tag and length
    uint8_t tag_length = 0;
    do {
        *tag_available = FALSE;
        // Check if expected buffer length matches input buffer length
        if (buffer_length != expected_buffer_length) {
            break;
        }

        return_status = OPTIGA_LIB_SUCCESS;
        // Check for tag in the input buffer
        while (offset < (buffer_length - 0x02)) {
            if (tag == buffer[offset++]) {
                *tag_available = TRUE;
                break;
            }
            tag_length = buffer[offset];
            offset += (tag_length + 1);
        }
    } while (FALSE);
    return (return_status);
}

void ut_util_encode_ecc_public_key_in_bit_string_format(
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

void ut_util_encode_rsa_public_key_in_bit_string_format(
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

void ut_optiga_crypt_tls_prf_sha256_fct(void) {
    uint8_t decryption_key[16] = {0};
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    /**
     * Write the shared secret to the Arbitrary data object F1D0
     *       - This is typically a one time activity and
     *       - use the this OID as input secret to derive keys further
     * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
     *    to clear the remaining data in the object
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_data(
        ut_optiga_util_instance,
        0xF1D0,
        OPTIGA_UTIL_ERASE_AND_WRITE,
        0x00,
        secret_to_be_written,
        sizeof(secret_to_be_written)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Change data object type to PRESSEC
     */

    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status =
        optiga_util_write_metadata(ut_optiga_util_instance, 0xF1D0, metadata, sizeof(metadata));

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Derive key (e.g. decryption key) using optiga_crypt_tls_prf_sha256 with protected I2C communication.
     *       - Use shared secret from F1D0 data object
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_COMMAND_PROTECTION
    );
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );

    ut_return_status = optiga_crypt_tls_prf_sha256(
        ut_optiga_crypt_instance,
        0xF1D0,
        label,
        sizeof(label),
        random_seed,
        sizeof(random_seed),
        sizeof(decryption_key),
        TRUE,
        decryption_key
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     *  Change meta data to default value
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        0xF1D0,
        default_metadata,
        sizeof(default_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_symmetric_generate_key_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t symmetric_key;
    uint16_t optiga_oid, bytes_to_read;
    uint8_t read_data_buffer[100];
    bool_t tag_available = FALSE;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    /**
     * Read metadata of a data object (e.g. key data object 0xE200)
     * using optiga_util_read_metadata.
     */
    optiga_oid = 0xE200;
    bytes_to_read = sizeof(read_data_buffer);
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_read_metadata(
        ut_optiga_util_instance,
        optiga_oid,
        read_data_buffer,
        &bytes_to_read
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    ut_return_status =
        ut_check_tag_in_metadata(read_data_buffer, (uint8_t)bytes_to_read, 0xE0, &tag_available);
    /* Cannot read metadata, hence the return will be always false */
    // assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    // assert(TRUE == tag_available);

    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_oid = 0xE200;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        optiga_oid,
        E200_metadata,
        sizeof(E200_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Generate symmetric key
     *  - Use AES-128 key type
     *  - OPTIGA_KEY_USAGE_ENCRYPTION as a Key Usage
     *  - Store the Symmetric key in OPTIGA Key store OID(E200)
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    symmetric_key = OPTIGA_KEY_ID_SECRET_BASED;

    ut_return_status = optiga_crypt_symmetric_generate_key(
        ut_optiga_crypt_instance,
        OPTIGA_SYMMETRIC_AES_128,
        (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
        FALSE,
        &symmetric_key
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_symmetric_encrypt_decrypt_cbc_fct() {
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
    uint8_t decrypted_data_buffer_start[32] = {0};
    uint32_t decrypted_data_length_start = sizeof(decrypted_data_buffer_start);
    uint8_t decrypted_data_buffer_continue[32] = {0};
    uint32_t decrypted_data_length_continue = sizeof(decrypted_data_buffer_continue);
    uint8_t decrypted_data_buffer_final[32] = {0};
    uint32_t decrypted_data_length_final = sizeof(decrypted_data_buffer_final);
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Start encryption sequence and encrypt the plain data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_symmetric_encrypt_start(
        ut_optiga_crypt_instance,
        OPTIGA_SYMMETRIC_CBC,
        OPTIGA_KEY_ID_SECRET_BASED,
        plain_data_buffer_start,
        sizeof(plain_data_buffer_start),
        NULL,
        0,
        NULL,
        0,
        0,
        encrypted_data_buffer_start,
        &encrypted_data_length_start
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Continue encrypting the plain data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_encrypt_continue(
        ut_optiga_crypt_instance,
        plain_data_buffer_continue,
        sizeof(plain_data_buffer_continue),
        encrypted_data_buffer_continue,
        &encrypted_data_length_continue
    );

    /**
     * End encryption sequence and encrypt plain data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_encrypt_final(
        ut_optiga_crypt_instance,
        plain_data_buffer_final,
        sizeof(plain_data_buffer_final),
        encrypted_data_buffer_final,
        &encrypted_data_length_final
    );

    /**
     * Start decryption sequence and decrypt the encrypted data from step 3
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_decrypt_start(
        ut_optiga_crypt_instance,
        OPTIGA_SYMMETRIC_CBC,
        OPTIGA_KEY_ID_SECRET_BASED,
        encrypted_data_buffer_start,
        encrypted_data_length_start,
        NULL,
        0,
        NULL,
        0,
        0,
        decrypted_data_buffer_start,
        &decrypted_data_length_start
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Continue to decrypt the encrypted data from step 4
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_decrypt_continue(
        ut_optiga_crypt_instance,
        encrypted_data_buffer_continue,
        encrypted_data_length_continue,
        decrypted_data_buffer_continue,
        &decrypted_data_length_continue
    );

    /**
     * End decryption sequence and decrypt encrypted data from step 5
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_decrypt_final(
        ut_optiga_crypt_instance,
        encrypted_data_buffer_final,
        encrypted_data_length_final,
        decrypted_data_buffer_final,
        &decrypted_data_length_final
    );

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_symmetric_encrypt_decrypt_ecb_fct(void) {
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
    uint32_t plain_data_length = sizeof(plain_data_buffer);
    uint8_t encrypted_data_buffer[32] = {0};
    uint32_t encrypted_data_length = sizeof(encrypted_data_buffer);
    uint8_t decrypted_data_buffer[32] = {0};
    uint32_t decrypted_data_length = sizeof(decrypted_data_buffer);
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Encrypt the plain data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_encrypt_ecb(
        ut_optiga_crypt_instance,
        OPTIGA_KEY_ID_SECRET_BASED,
        plain_data_buffer,
        plain_data_length,
        encrypted_data_buffer,
        &encrypted_data_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Decrypt the encrypted data from step 3
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_symmetric_decrypt_ecb(
        ut_optiga_crypt_instance,
        OPTIGA_KEY_ID_SECRET_BASED,
        encrypted_data_buffer,
        encrypted_data_length,
        decrypted_data_buffer,
        &decrypted_data_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_verify_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    // Form rsa public key in bit string format
    ut_util_encode_rsa_public_key_in_bit_string_format(
        rsa_public_key_modulus,
        sizeof(rsa_public_key_modulus),
        rsa_public_key_exponent,
        sizeof(rsa_public_key_exponent),
        rsa_public_key,
        &rsa_public_key_length
    );

    public_key_from_host_t public_key_details = {
        rsa_public_key,
        rsa_public_key_length,
        (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL};

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Verify RSA signature using public key from host
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_rsa_verify(
        ut_optiga_crypt_instance,
        OPTIGA_RSASSA_PKCS1_V15_SHA256,
        digest,
        sizeof(digest),
        signature,
        sizeof(signature),
        OPTIGA_CRYPT_HOST_DATA,
        &public_key_details,
        0x0000
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_sign_fct(void) {
    // To store the signture generated
    uint8_t signature[200];
    uint16_t signature_length = sizeof(signature);

    // Crypt Instance
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Sign the digest -
     *       - Use Private key from Key Store ID E0FC
     *       - Signature scheme is SHA256,
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_rsa_sign(
        ut_optiga_crypt_instance,
        OPTIGA_RSASSA_PKCS1_V15_SHA256,
        digest,
        sizeof(digest),
        OPTIGA_KEY_ID_E0FC,
        signature,
        &signature_length,
        0x0000
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_generate_keypair_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    uint16_t optiga_oid;

    // To store the generated public key as part of Generate key pair
    /*
     *
     * Give a buffer of minimum size required to store the exported public key.
     * E.g
     * For 1024 bit key : modulus(128 bytes) + public expoenent(3 bytes) + Encoding bytes length(approximately 15 bytes)
     * For 2048 bit key : modulus(256 bytes) + public expoenent(3 bytes) + Encoding bytes length(approximately 15 bytes)
     */
    uint8_t public_key[150];
    uint16_t public_key_length = sizeof(public_key);

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_oid = 0xE0FC;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        optiga_oid,
        E0FC_metadata,
        sizeof(E0FC_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 2. Generate RSA Key pair
     *       - Use 1024 or 2048 bit RSA key
     *       - Specify the Key Usage
     *       - Store the Private key in OPTIGA Key store
     *              (When exporting the private key, provide buffer of sufficient length (key size in bytes +
     *               encoding length))
     *       - Export Public Key
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_key_id = OPTIGA_KEY_ID_E0FC;

    ut_return_status = optiga_crypt_rsa_generate_keypair(
        ut_optiga_crypt_instance,
        OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
        (uint8_t)OPTIGA_KEY_USAGE_SIGN,
        FALSE,
        &optiga_key_id,
        public_key,
        &public_key_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_encrypt_session_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    optiga_rsa_encryption_scheme_t encryption_scheme;

    uint8_t public_key[150];
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    uint16_t public_key_length = sizeof(public_key);
    public_key_from_host_t public_key_from_host;
    const uint8_t optional_data[2] = {0x01, 0x02};
    uint16_t optional_data_length = sizeof(optional_data);

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Generate 1024 bit RSA Key pair
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_key_id = OPTIGA_KEY_ID_E0FC;
    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(ut_optiga_crypt_instance, OPTIGA_COMMS_NO_PROTECTION);
    ut_return_status = optiga_crypt_rsa_generate_keypair(
        ut_optiga_crypt_instance,
        OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
        (uint8_t)OPTIGA_KEY_USAGE_SIGN,
        FALSE,
        &optiga_key_id,
        public_key,
        &public_key_length
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Generate 48 byte RSA Pre master secret in acquired session OID
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_rsa_generate_pre_master_secret(
        ut_optiga_crypt_instance,
        optional_data,
        optional_data_length,
        48
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    optiga_lib_status = OPTIGA_LIB_BUSY;

    /**
     * Encrypt (RSA) the data in session OID
     */

    // OPTIGA Comms Shielded connection settings to enable the protection
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );

    encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
    public_key_from_host.public_key = public_key;
    public_key_from_host.length = public_key_length;
    public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;

    ut_return_status = optiga_crypt_rsa_encrypt_session(
        ut_optiga_crypt_instance,
        encryption_scheme,
        NULL,
        0,
        OPTIGA_CRYPT_HOST_DATA,
        &public_key_from_host,
        encrypted_message,
        &encrypted_message_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_encrypt_message_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_rsa_encryption_scheme_t encryption_scheme;
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    public_key_from_host_t public_key_from_host;

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * 2. RSA encryption
     */
    // Form rsa public key in bit string format
    ut_util_encode_rsa_public_key_in_bit_string_format(
        rsa_public_key_modulus,
        sizeof(rsa_public_key_modulus),
        rsa_public_key_exponent,
        sizeof(rsa_public_key_exponent),
        rsa_public_key,
        &rsa_public_key_length
    );

    encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
    public_key_from_host.public_key = rsa_public_key;
    public_key_from_host.length = rsa_public_key_length;
    public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_rsa_encrypt_message(
        ut_optiga_crypt_instance,
        encryption_scheme,
        message,
        sizeof(message),
        NULL,
        0,
        OPTIGA_CRYPT_HOST_DATA,
        &public_key_from_host,
        encrypted_message,
        &encrypted_message_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_rsa_decrypt_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    optiga_rsa_encryption_scheme_t encryption_scheme;
    public_key_from_host_t public_key_from_host;
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    uint8_t public_key[150];
    uint16_t public_key_length = sizeof(public_key);
    const uint8_t optional_data[] = {0x01, 0x02};

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Generate RSA Key pair
     *       - Use 1024 or 2048 bit RSA key
     *       - Specify the Key Usage
     *       - Store the Private key in OPTIGA Key store
     *       - Export Public Key
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_key_id = OPTIGA_KEY_ID_E0FC;
    ut_return_status = optiga_crypt_rsa_generate_keypair(
        ut_optiga_crypt_instance,
        OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
        (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
        FALSE,
        &optiga_key_id,
        public_key,
        &public_key_length
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);
    /**
     * 3. Generate 0x46 byte RSA Pre master secret which is stored in acquired session OID
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_rsa_generate_pre_master_secret(
        ut_optiga_crypt_instance,
        optional_data,
        sizeof(optional_data),
        30
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    optiga_lib_status = OPTIGA_LIB_BUSY;

    /**
     * 4. Encrypt(RSA) the data stored in session OID
     */

    // OPTIGA Comms Shielded connection settings to enable the protection
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );
    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_COMMAND_PROTECTION
    );

    encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
    public_key_from_host.public_key = public_key;
    public_key_from_host.length = public_key_length;
    public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;

    ut_return_status = optiga_crypt_rsa_encrypt_session(
        ut_optiga_crypt_instance,
        encryption_scheme,
        NULL,
        0,
        OPTIGA_CRYPT_HOST_DATA,
        &public_key_from_host,
        encrypted_message,
        &encrypted_message_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);
    /**
     * 5. RSA decryption
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    encryption_scheme = OPTIGA_RSAES_PKCS1_V15;

    ut_return_status = optiga_crypt_rsa_decrypt_and_store(
        ut_optiga_crypt_instance,
        encryption_scheme,
        encrypted_message,
        encrypted_message_length,
        NULL,
        0,
        optiga_key_id
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_random_fct(void) {
    uint8_t random_data_buffer[32];
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Generate Random -
     *       - Specify the Random type as TRNG
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_random(
        ut_optiga_crypt_instance,
        OPTIGA_RNG_TYPE_TRNG,
        random_data_buffer,
        sizeof(random_data_buffer)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_hmac_fct(void) {
    const uint8_t input_data_buffer_start[] = {
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
    const uint8_t input_data_buffer_update[] = {
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
    const uint8_t input_data_buffer_final[] = {
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
    uint8_t data_to_hash[] = {"OPTIGA, Infineon Technologies AG"};
    hash_data_from_host_t hash_data_host;

    uint8_t digest[32];
    uint8_t mac_buffer[32] = {0};
    uint32_t mac_buffer_length = sizeof(mac_buffer);
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * Start HMAC operation
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_hmac_start(
        ut_optiga_crypt_instance,
        OPTIGA_HMAC_SHA_256,
        0xF1D0,
        input_data_buffer_start,
        sizeof(input_data_buffer_start)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Continue HMAC operation on input data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hmac_update(
        ut_optiga_crypt_instance,
        input_data_buffer_update,
        sizeof(input_data_buffer_update)
    );

    /**
     * End HMAC sequence and return the MAC generated
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hmac_finalize(
        ut_optiga_crypt_instance,
        input_data_buffer_final,
        sizeof(input_data_buffer_final),
        mac_buffer,
        &mac_buffer_length
    );

    hash_data_host.buffer = data_to_hash;
    hash_data_host.length = sizeof(data_to_hash);
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hash(
        ut_optiga_crypt_instance,
        OPTIGA_HASH_TYPE_SHA_256,
        OPTIGA_CRYPT_HOST_DATA,
        &hash_data_host,
        digest
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_hkdf_fct(void) {
    uint8_t decryption_key[16] = {0};

    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /**
     * Create OPTIGA Crypt, util Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    /**
     * 1. Write the shared secret to the Arbitrary data object F1D0
     *       - This is typically a one time activity and
     *       - use the this OID as input secret to derive keys further
     * 2. Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
     *    to clear the remaining data in the object
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_data(
        ut_optiga_util_instance,
        0xF1D0,
        OPTIGA_UTIL_ERASE_AND_WRITE,
        0x00,
        secret_to_be_written,
        sizeof(secret_to_be_written)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 2. Change data object type to PRESSEC
     *
     */

    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status =
        optiga_util_write_metadata(ut_optiga_util_instance, 0xF1D0, metadata, sizeof(metadata));

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 4. Derive key (e.g. decryption key) using optiga_crypt_hkdf with protected I2C communication.
     *       - Use shared secret from F1D0 data object
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_COMMAND_PROTECTION
    );
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );

    ut_return_status = optiga_crypt_hkdf(
        ut_optiga_crypt_instance,
        OPTIGA_HKDF_SHA_256,
        0xF1D0, /* Input secret OID */
        salt,
        sizeof(salt),
        info,
        sizeof(info),
        sizeof(decryption_key),
        TRUE,
        decryption_key
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 5. Change meta data to default value
     *
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        0xF1D0,
        default_metadata,
        sizeof(default_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_hash_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;

    // Note : The size of the hash context provided by OPTIGA is 209 bytes for SHA-256
    uint8_t hash_context_buffer[OPTIGA_HASH_CONTEXT_LENGTH_SHA_256];
    optiga_hash_context_t hash_context;
    uint8_t data_to_hash[] = {"OPTIGA, Infineon Technologies AG"};
    hash_data_from_host_t hash_data_host;
    uint8_t digest[32];
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * 2. Initialize the Hash context
     */

    OPTIGA_HASH_CONTEXT_INIT(
        hash_context,
        hash_context_buffer,
        sizeof(hash_context_buffer),
        (uint8_t)OPTIGA_HASH_TYPE_SHA_256
    );

    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_hash_start(ut_optiga_crypt_instance, &hash_context);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 4. Continue hashing the data
     */
    hash_data_host.buffer = data_to_hash;
    hash_data_host.length = sizeof(data_to_hash);

    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hash_update(
        ut_optiga_crypt_instance,
        &hash_context,
        OPTIGA_CRYPT_HOST_DATA,
        &hash_data_host
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };

    /**
     * 5. Finalize the hash
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hash_finalize(ut_optiga_crypt_instance, &hash_context, digest);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
    }
}

void ut_optiga_crypt_ecdsa_verify_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    ut_util_encode_ecc_public_key_in_bit_string_format(
        ecc_public_key_component,
        sizeof(ecc_public_key_component),
        ecc_public_key,
        &ecc_public_key_length
    );

    public_key_from_host_t public_key_details = {
        ecc_public_key,
        ecc_public_key_length,
        (uint8_t)OPTIGA_ECC_CURVE_NIST_P_256};

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * 2. Verify ECDSA signature using public key from host
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_ecdsa_verify(
        ut_optiga_crypt_instance,
        ecc_digest,
        sizeof(ecc_digest),
        ecc_signature,
        sizeof(ecc_signature),
        OPTIGA_CRYPT_HOST_DATA,
        &public_key_details
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_ecdsa_sign_fct(void) {
    // To store the signture generated
    uint8_t ecdsa_signature[80];
    uint16_t ecdsa_signature_length = sizeof(ecdsa_signature);
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * 2. Sign the digest using Private key from Key Store ID E0F0
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_ecdsa_sign(
        ut_optiga_crypt_instance,
        ecc_digest,
        sizeof(ecc_digest),
        OPTIGA_KEY_ID_E0F0,
        ecdsa_signature,
        &ecdsa_signature_length
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_optiga_crypt_instance->instance_state = OPTIGA_LIB_INSTANCE_FREE;
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_ecdh_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    // To store the generated public key as part of Generate key pair
    uint8_t public_key[100];
    uint16_t public_key_length = sizeof(public_key);

    // To store the generated shared secret as part of ECDH
    uint8_t shared_secret[32];
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     *
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    /**
     * 2. Generate ECC Key pair - To use the private key with ECDH in the next step
     *       - Use ECC NIST P 256 Curve
     *       - Specify the Key Usage as Key Agreement
     *       - Store the Private key with in OPTIGA Session
     *       - Export Public Key
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;

    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_COMMAND_PROTECTION
    );
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );
    ut_return_status = optiga_crypt_ecc_generate_keypair(
        ut_optiga_crypt_instance,
        OPTIGA_ECC_CURVE_NIST_P_256,
        (uint8_t)OPTIGA_KEY_USAGE_KEY_AGREEMENT,
        FALSE,
        &optiga_key_id,
        public_key,
        &public_key_length
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * 3. Perform ECDH using the Peer Public key
     *       - Use ECC NIST P 256 Curve
     *       - Provide the peer public key details
     *       - Export the generated shared secret with protected I2C communication
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_COMMAND_PROTECTION
    );
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );
    ut_return_status = optiga_crypt_ecdh(
        ut_optiga_crypt_instance,
        optiga_key_id,
        &peer_public_key_details,
        TRUE,
        shared_secret
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_ecc_generate_keypair_fct(void) {
    optiga_lib_status_t ut_return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    // To store the generated public key as part of Generate key pair
    uint8_t public_key[100];
    uint16_t public_key_length = sizeof(public_key);
    uint16_t optiga_oid;

    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_oid = 0xE0F1;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        optiga_oid,
        E0F1_metadata,
        sizeof(E0F1_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Generate ECC Key pair
     *       - Use ECC NIST P 256 Curve
     *       - Specify the Key Usage (Key Agreement or Sign based on requirement)
     *       - Store the Private key in OPTIGA Key store
     *       - Export Public Key
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    optiga_key_id = OPTIGA_KEY_ID_E0F1;
    // for Session based, use OPTIGA_KEY_ID_SESSION_BASED as key id as shown below.
    // optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;
    ut_return_status = optiga_crypt_ecc_generate_keypair(
        ut_optiga_crypt_instance,
        OPTIGA_ECC_CURVE_NIST_P_256,
        (uint8_t)OPTIGA_KEY_USAGE_SIGN,
        FALSE,
        &optiga_key_id,
        public_key,
        &public_key_length
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

void ut_optiga_crypt_clear_auto_state_fct(void) {
    optiga_lib_status_t ut_return_status = OPTIGA_CRYPT_ERROR;
    optiga_crypt_t *ut_optiga_crypt_instance = NULL;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /**
     * Create OPTIGA Crypt Instance
     */
    ut_optiga_crypt_instance = optiga_crypt_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_crypt_instance != NULL);

    ut_optiga_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    /**
     * Initialize the protection level and protocol version for the instances
     */
    OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(ut_optiga_util_instance, OPTIGA_COMMS_NO_PROTECTION);
    OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_util_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );

    OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(ut_optiga_crypt_instance, OPTIGA_COMMS_NO_PROTECTION);
    OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_crypt_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );

    /**
     * Set the metadata of secret OID(0xF1D0) using optiga_util_write_metadata.
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_metadata(
        ut_optiga_util_instance,
        0xF1D0,
        secret_oid_metadata,
        sizeof(secret_oid_metadata)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     *  Write shared secret in OID 0xF1D0
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_write_data(
        ut_optiga_util_instance,
        0xF1D0,
        OPTIGA_UTIL_ERASE_AND_WRITE,
        0,
        user_secret,
        sizeof(user_secret)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Call generate auth code with optional data
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_generate_auth_code(
        ut_optiga_crypt_instance,
        OPTIGA_RNG_TYPE_TRNG,
        optional_data,
        sizeof(optional_data),
        random_data,
        32
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    /**
     * Calculate HMAC on host
     */
    pal_os_memset(arbitrary_data, 0x5A, sizeof(arbitrary_data));
    pal_os_memcpy(input_data_buffer, optional_data, sizeof(optional_data));
    pal_os_memcpy(&input_data_buffer[sizeof(optional_data)], random_data, sizeof(random_data));
    pal_os_memcpy(
        &input_data_buffer[sizeof(optional_data) + sizeof(random_data)],
        arbitrary_data,
        sizeof(arbitrary_data)
    );

    CalcHMAC(
        user_secret,
        sizeof(user_secret),
        input_data_buffer,
        sizeof(input_data_buffer),
        hmac_buffer
    );

    /**
     * Perform HMAC verification using OPTIGA
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_crypt_hmac_verify(
        ut_optiga_crypt_instance,
        OPTIGA_HMAC_SHA_256,
        0xF1D0,
        input_data_buffer,
        sizeof(input_data_buffer),
        hmac_buffer,
        hmac_length
    );
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);
    /**
     * Perform clear auto state using OPTIGA
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_crypt_clear_auto_state(ut_optiga_crypt_instance, 0xF1D0);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    if (ut_optiga_crypt_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_crypt_destroy(ut_optiga_crypt_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }

    if (ut_optiga_util_instance) {
        // Destroy the instance after the completion of usecase if not required.
        ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    /*
    optiga_crypt_create, optiga_crypt_tls_prf_sha256, optiga_crypt_destroy Unit tests covered.
    */
    ut_optiga_crypt_tls_prf_sha256_fct();

    /*
    optiga_crypt_symmetric_generate_key Unit tests covered.
    */
    ut_optiga_crypt_symmetric_generate_key_fct();

    /*
    optiga_crypt_symmetric_encrypt_start, optiga_crypt_symmetric_encrypt_continue, optiga_crypt_symmetric_encrypt_final
    optiga_crypt_symmetric_decrypt_start, optiga_crypt_symmetric_decrypt_continue, optiga_crypt_symmetric_decrypt_final
    Unit tests covered.
    */
    ut_optiga_crypt_symmetric_encrypt_decrypt_cbc_fct();
    ut_optiga_crypt_symmetric_encrypt_decrypt_ecb_fct();

    /*
    optiga_crypt_rsa_verify, optiga_crypt_rsa_sign Unit tests covered.
    */
    ut_optiga_crypt_rsa_verify_fct();
    ut_optiga_crypt_rsa_sign_fct();

    /*
    optiga_crypt_rsa_generate_keypair, optiga_crypt_rsa_generate_pre_master_secret, optiga_crypt_rsa_encrypt_session Unit tests covered.
   */
    ut_optiga_crypt_rsa_generate_keypair_fct();
    ut_optiga_crypt_rsa_encrypt_session_fct();

    /*
    optiga_crypt_rsa_encrypt_message, optiga_crypt_rsa_decrypt_and_store Unit tests covered.
   */
    ut_optiga_crypt_rsa_encrypt_message_fct();
    ut_optiga_crypt_rsa_decrypt_fct();

    /*
   optiga_crypt_random, optiga_crypt_hmac_start, optiga_crypt_hmac_update, optiga_crypt_hmac_finalize Unit tests covered.
   */
    ut_optiga_crypt_random_fct();
    ut_optiga_crypt_hmac_fct();

    /*
   optiga_crypt_hkdf, optiga_crypt_hash_start, optiga_crypt_hash_update, optiga_crypt_hash_finalize Unit tests covered.
   */
    ut_optiga_crypt_hkdf_fct();
    ut_optiga_crypt_hash_fct();

    /*
   optiga_crypt_ecdsa_verify, optiga_crypt_ecdsa_sign, optiga_crypt_ecdh
   */
    ut_optiga_crypt_ecdsa_verify_fct();
    ut_optiga_crypt_ecdsa_sign_fct();
    ut_optiga_crypt_ecdh_fct();

    /*
   optiga_crypt_clear_auto_state, optiga_crypt_hmac_verify, optiga_crypt_generate_auth_code, optiga_crypt_ecc_generate_keypair Unit tests covered.
   */
    ut_optiga_crypt_ecc_generate_keypair_fct();
    ut_optiga_crypt_clear_auto_state_fct();
}