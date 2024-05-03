/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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
#ifndef _PROTECTED_UPDATE_DATA_SET_H_
#define _PROTECTED_UPDATE_DATA_SET_H_

#include <stdint.h>

#define FRAGMENT_DIGEST_LENGTH (0x20)
#define MAX_PAYLOAD_SIZE (640U)
#define MAC_SIZE (8U)
#define MAX_PAYLOAD_FRAGMENT_SIZE (MAX_PAYLOAD_SIZE - FRAGMENT_DIGEST_LENGTH)

#define NONCE_LENGTH (11U)
#define FRAGMENT_NUMBER_LENGTH (2U)
#define ASSOCIATED_DATA_LENGTH (8U)
#define SESSION_KEY_LENGTH (27U)

#define PAYLOAD_TYPE_KEY "key"
#define PAYLOAD_TYPE_DATA "data"
#define PROTECTED_INTEGRATY_TYPE "integrity"
#define PROTECTED_CONFIDENTIALITY_TYPE "confidentiality"
#define TRUE (1U)
#define FALSE (0U)
#define COUID_SIZE (25U)

typedef enum key_type { eRSA = 1, eECC = 2, eAES = 3 } key_type_d;

typedef enum payload_type {
    ePAYLOAD_DATA = -1,
    ePAYLOAD_METADATA = -2,
    ePAYLOAD_KEY = -3,
} payload_type_t;

typedef enum key_algorithm {
    eECC_NIST_P_256 = 0x03,
    eECC_NIST_P_384 = 0x04,
    eRSA_1024_EXP = 0x41,
    eRSA_2048_EXP = 0x42,
    eAES_128 = 0x81,
    eAES_192 = 0x82,
    eAES_256 = 0x83,
} key_algorithm_t;

typedef enum key_usage { eAUTH = 0x01, eENC = 0x02, eSIGN = 0x10, eKEY_AGREE = 0x20 } key_usage_t;

typedef enum encryption_algo {
    eAES_CCM_16_64_128 = 10,
} encryption_algo_t;

typedef enum kdf_algo {
    eIFX_KDF_TLS12_PRF_SHA256 = -65720,
} kdf_algo_t;

typedef enum content_reset_type {
    eMETADATA_IDENTIFIER_FLAG = 0,
    eZEROES = 1,
    eRANDOM = 2
} content_reset_type_t;

// Structure to store key derivation
typedef struct kdf {
    uint16_t shared_secret_oid;
    uint8_t *shared_secret;
    uint16_t shared_secret_length;
    const uint8_t *label;
    uint16_t label_length;
    uint8_t *seed;
    uint8_t seed_length;
    kdf_algo_t key_derivation_algo;
} kdf_t;

// Structure to store encrypt data parameters
typedef struct enc_params {
    encryption_algo_t encrypt_algo;
    uint8_t session_key[SESSION_KEY_LENGTH];
    uint8_t session_key_length;
    uint8_t nonce_length;
    uint8_t nonce_data[NONCE_LENGTH];
    uint8_t associated_data[ASSOCIATED_DATA_LENGTH];
    uint8_t mac_size;
} enc_params_t;

typedef struct confidentiality {
    kdf_t kdf_data;
    enc_params_t enc_params;
} confidentiality_t;

typedef struct data_payload {
    uint32_t offset_in_oid;
    uint8_t *data;
    uint16_t data_length;
    uint8_t write_type;
} data_payload_t;

typedef struct ecc_key {
    uint8_t *D;
    uint16_t D_length;
    uint8_t *X;
    uint16_t X_length;
    uint8_t *Y;
    uint16_t Y_length;
} ecc_key_t;

typedef struct rsa_key {
    uint8_t *N;
    uint16_t N_length;
    uint8_t *E;
    uint16_t E_length;
    uint8_t *D;
    uint16_t D_length;
} rsa_key_t;

typedef struct aes_key {
    uint8_t *key;
    uint16_t key_length;
} aes_key_t;

typedef union key_params {
    rsa_key_t rsa_key;
    ecc_key_t ecc_key;
    aes_key_t aes_key;
} key_params_t;

typedef struct key_payload {
    key_type_d key_type;
    key_usage_t key_usage;
    key_algorithm_t key_algorithm;
    key_params_t key_params;
    const int8_t *key_data_file_path;
} key_payload_t;

typedef struct metadata_payload {
    key_type_d content_reset;
    uint8_t additional_flag;
    uint8_t *metadata;
    uint16_t metadata_length;
} metadata_payload_t;

typedef enum signature_algo {
    eES_SHA = -7,
    eRSA_SSA_PKCS1_V1_5_SHA_256 = -65700,
} signature_algo_t;

typedef enum digest_algo {
    eSHA_256 = 41,
} digest_algo_t;

// Structure to store manifest data
typedef struct manifest {
    uint16_t payload_version;
    uint16_t trust_anchor_oid;
    uint16_t target_oid;
    const uint8_t *couid;
    signature_algo_t signature_algo;
    const int8_t *private_key;
    digest_algo_t digest_algo;
    uint8_t *payload;
    uint32_t payload_length;
    payload_type_t payload_type;

    // confidentiality
    confidentiality_t *p_confidentiality;
    // payload data
    data_payload_t *p_data_payload;
    // payload key
    key_payload_t *p_key_payload;
    // payload metadata
    metadata_payload_t *p_metadata_payload;

} manifest_t;

// Structure to store protect update data set
typedef struct protected_update_data_set {
    uint8_t *data;
    uint16_t data_length;

    uint8_t *fragments;
    uint16_t fragments_length;
    uint16_t actual_memory_allocated;

} protected_update_data_set_d;

// Create protected update manifest
int32_t protected_update_create_manifest(
    manifest_t *manifest_data,
    protected_update_data_set_d *p_cbor_manifest
);

// Create protected update payload fragments
int32_t protected_update_create_fragments(
    manifest_t *manifest_data,
    protected_update_data_set_d *p_cbor_manifest
);

void protected_update_print_output_dataset(const protected_update_data_set_d *p_cbor_manifest);

#ifndef _NO_STATIC_H
#define _STATIC_H static
#else
#define _STATIC_H
#endif

#endif  //_PROTECTED_UPDATE_DATA_SET_H_

/**
 * @}
 */