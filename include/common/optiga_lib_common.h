/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file optiga_lib_common.h
*
* \brief   This file provides the prototypes for the commonly used functions and structures of OPTIGA Library.
*
* \ingroup  grOptigaLibCommon
*
* @{
*/

#ifndef _OPTIGA_LIB_COMMON_H_
#define _OPTIGA_LIB_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_lib_config.h"
#include "optiga_lib_types.h"
#include "optiga_lib_return_codes.h"

///Instance id of OPTIGA slave
#define OPTIGA_INSTANCE_ID_0                              (0x00)

/** @brief When command data and response data is unprotected */
#define OPTIGA_COMMS_NO_PROTECTION                        (0x00)
/** @brief When command data is protected and response data is unprotected */
#define OPTIGA_COMMS_COMMAND_PROTECTION                   (0x01)
/** @brief When command data is unprotected and response data is protected */
#define OPTIGA_COMMS_RESPONSE_PROTECTION                  (0x02)
/** @brief Both command data and response data are protected */
#define OPTIGA_COMMS_FULL_PROTECTION                      (0x03)
/** @brief Re-establish shielded connection */
#define OPTIGA_COMMS_RE_ESTABLISH                         (0x80)
/** @brief Pre shared secret protocol version */
#define OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET   (0x01)

/** @brief Data is provided by host*/
#define OPTIGA_CRYPT_HOST_DATA                            (0x01)
/** @brief Data in internal to optiga OID */
#define OPTIGA_CRYPT_OID_DATA                             (0x00)

/** @brief OPTIGA instance is busy */
#define OPTIGA_LIB_INSTANCE_BUSY                          (0x0001)
/** @brief OPTIGA instance is free */
#define OPTIGA_LIB_INSTANCE_FREE                          (0x0000)

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
/** @brief Configure shielded connection protection level for instance */
#define OPTIGA_COMMS_PROTECTION_LEVEL                     (0x01)
/** @brief Configure shielded connection protocol version for instance */
#define OPTIGA_COMMS_PROTOCOL_VERSION                     (0x02)
#endif

/**
 * \brief Specifies the key location in OPTIGA.
 */
typedef enum optiga_key_id
{
    /// Key from key store (non-volatile)
    OPTIGA_KEY_ID_E0F0 = 0xE0F0,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_ID_E0F1 = 0xE0F1,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_ID_E0F2 = 0xE0F2,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_ID_E0F3 = 0xE0F3,
    /// Key from key store for RSA (non-volatile)
    OPTIGA_KEY_ID_E0FC = 0xE0FC,
    /// Key from key store for RSA (non-volatile)
    OPTIGA_KEY_ID_E0FD = 0xE0FD,
    /// Key from session (volatile)
    OPTIGA_KEY_ID_SESSION_BASED = 0x0000,
#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
    /// Key from key store for symmetric operations
    OPTIGA_KEY_ID_SECRET_BASED = 0xE200
#endif    
} optiga_key_id_t;

/**
 * \brief Specifies the key usage type in OPTIGA.
 */
typedef enum optiga_key_usage
{
    /// This enables the private key for the signature generation as part of authentication commands
    OPTIGA_KEY_USAGE_AUTHENTICATION = 0x01,
    /// This enables the private key for the signature generation
    OPTIGA_KEY_USAGE_SIGN = 0x10,
    /// This enables the private key for key agreement (e.g. ecdh operations)
    OPTIGA_KEY_USAGE_KEY_AGREEMENT = 0x20,
    /// This enables the private key for encrypt and decrypt
    OPTIGA_KEY_USAGE_ENCRYPTION = 0x02,
} optiga_key_usage_t;

/**
 * \brief Specifies the set protected object tag.
 */
typedef enum optiga_set_obj_protected_tag
{
    /// This enables to start of the protected update
    OPTIGA_SET_PROTECTED_UPDATE_START = 0x00,
    /// This enables to continue of the protected update
    OPTIGA_SET_PROTECTED_UPDATE_CONTINUE = 0x02,
    /// This enables to finish of the protected update
    OPTIGA_SET_PROTECTED_UPDATE_FINAL = 0x01
} optiga_set_obj_protected_tag_t;

/**
 * \brief Specifies the key curve type in OPTIGA.
 */
typedef enum optiga_ecc_curve
{
    /// Generate elliptic curve key based on NIST P256
    OPTIGA_ECC_CURVE_NIST_P_256 = 0x03,
    /// Generate elliptic curve key based on NIST P384
    OPTIGA_ECC_CURVE_NIST_P_384 = 0x04,
#ifdef OPTIGA_CRYPT_ECC_NIST_P_521_ENABLED    
    /// Generate elliptic curve key based on ECC NIST P521
    OPTIGA_ECC_CURVE_NIST_P_521 = 0x05,
#endif
#ifdef OPTIGA_CRYPT_ECC_BRAINPOOL_P_R1_ENABLED    
    /// Generate elliptic curve key based on ECC Brainpool 256R1
    OPTIGA_ECC_CURVE_BRAIN_POOL_P_256R1 = 0x13,
    /// Generate elliptic curve key based on ECC Brainpool 384R1
    OPTIGA_ECC_CURVE_BRAIN_POOL_P_384R1 = 0x15,
    /// Generate elliptic curve key based on ECC Brainpool 512R1
    OPTIGA_ECC_CURVE_BRAIN_POOL_P_512R1 = 0x16
#endif    
} optiga_ecc_curve_t;

/**
 * \brief Specifies the RSA encryption schemes.
 */
typedef enum optiga_rsa_encryption_scheme
{
    /// RSA PKCS1 v1.5 encryption scheme
    OPTIGA_RSAES_PKCS1_V15 = 0x11
} optiga_rsa_encryption_scheme_t;

/**
 * \brief Specifies the RSA key type in OPTIGA.
 */
typedef enum optiga_rsa_key_type
{
    /// Generate 1024 bit RSA key
    OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL = 0x41,
    /// Generate 2048 bit RSA key
    OPTIGA_RSA_KEY_2048_BIT_EXPONENTIAL = 0x42
} optiga_rsa_key_type_t;


/**
 * \brief Specifies the RSA signature schemes type in OPTIGA.
 */
typedef enum optiga_rsa_signature_scheme
{
    /// Signature schemes RSA SSA PKCS1-v1.5 with SHA256 digest
    OPTIGA_RSASSA_PKCS1_V15_SHA256 = 0x01,
    /// Signature schemes RSA SSA PKCS1-v1.5 with SHA384 digest
    OPTIGA_RSASSA_PKCS1_V15_SHA384 = 0x02,
#ifdef OPTIGA_CRYPT_RSA_SSA_SHA512_ENABLED
    /// Signature schemes RSA SSA PKCS1-v1.5 with SHA512 digest
    OPTIGA_RSASSA_PKCS1_V15_SHA512 = 0x03
#endif    
}optiga_rsa_signature_scheme_t;

#if defined (OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) || defined (OPTIGA_CRYPT_SYM_DECRYPT_ENABLED) || \
defined (OPTIGA_CRYPT_HMAC_ENABLED) || defined (OPTIGA_CRYPT_HMAC_VERIFY_ENABLED)
/**
 * \brief Specifies the symmetric encryption schemes type in OPTIGA.
 */
typedef enum optiga_symmetric_encryption_mode
{
    /// Symmetric encryption mode with ECB mode
    OPTIGA_SYMMETRIC_ECB = 0x08,
    /// Symmetric encryption mode with CBC mode
    OPTIGA_SYMMETRIC_CBC = 0x09,
    /// Symmetric encryption mode with CBC_MAC mode
    OPTIGA_SYMMETRIC_CBC_MAC = 0x0A,
    /// Symmetric encryption mode with CMAC mode
    OPTIGA_SYMMETRIC_CMAC = 0x0B
}optiga_symmetric_encryption_mode_t;
#endif

/**
 * \brief Specifies the hashing algorithm type in OPTIGA.
 */
typedef enum optiga_hash_type
{
    /// Hash algorithm type SHA256
    OPTIGA_HASH_TYPE_SHA_256 = 0xE2
} optiga_hash_type_t;

/**
 * \brief Specifies the hash context length in bytes.
 */
typedef enum optiga_hash_context_length
{
    /// Hash context length (in bytes) in case of SHA256.
    OPTIGA_HASH_CONTEXT_LENGTH_SHA_256 = 209 
} optiga_hash_context_length_t; 

/**
 * \brief Specifies the random generation types
 */
typedef enum optiga_rng_type
{
    /// Generate Random data using TRNG
    OPTIGA_RNG_TYPE_TRNG = 0x00,
    /// Generate Random data using DRNG
    OPTIGA_RNG_TYPE_DRNG = 0x01
} optiga_rng_type_t;

#ifdef OPTIGA_CRYPT_HMAC_ENABLED
/**
 * \brief Specifies the HMAC generation types in OPTIGA.
 */
typedef enum optiga_hmac_type
{
    /// Generated MAC using HMAC-SHA256
    OPTIGA_HMAC_SHA_256 = 0x20,
    /// Generated MAC using HMAC-SHA384
    OPTIGA_HMAC_SHA_384 = 0x21,
    /// Generated MAC using HMAC-SHA512
    OPTIGA_HMAC_SHA_512 = 0x22
}optiga_hmac_type_t;
#endif

#ifdef OPTIGA_CRYPT_HKDF_ENABLED
/**
 * \brief Specifies the HKDF key derivation types in OPTIGA.
 */
typedef enum optiga_hkdf_type
{
    /// Key derivation using HKDF-SHA256
    OPTIGA_HKDF_SHA_256 = 0x08,
    /// Key derivation using HKDF-SHA384
    OPTIGA_HKDF_SHA_384 = 0x09,
    /// Key derivation using HKDF-SHA512
    OPTIGA_HKDF_SHA_512 = 0x0A
}optiga_hkdf_type_t;
#endif

/**
 * \brief Specifies the key derivation types.
 */
typedef enum optiga_tls_prf_type
{
    /// Key derivation using TLSv1.2 PRF SHA256
    OPTIGA_TLS12_PRF_SHA_256 = 0x01,
#ifdef OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED    
    /// Key derivation using TLSv1.2 PRF SHA384
    OPTIGA_TLS12_PRF_SHA_384 = 0x02,
#endif
#ifdef OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED
    /// Key derivation using TLSv1.2 PRF SHA512
    OPTIGA_TLS12_PRF_SHA_512 = 0x03
#endif    
}optiga_tls_prf_type_t;

#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
/**
 * \brief Specifies the symmetric key types supported by OPTIGA.
 */
typedef enum optiga_symmetric_key_type
{
    /// Symmetric key type of AES-128
    OPTIGA_SYMMETRIC_AES_128 = 0x81,
    /// Symmetric key type of AES-192
    OPTIGA_SYMMETRIC_AES_192 = 0x82,
    /// Symmetric key type of AES-256
    OPTIGA_SYMMETRIC_AES_256 = 0x83
}optiga_symmetric_key_type_t;
#endif
/**
 * \brief Specifies the structure to the Hash context details managed by OPTIGA.
 */
typedef struct optiga_hash_context
{
    ///buffer to hold the hash context data
    uint8_t *context_buffer;
    ///context length
    uint16_t context_buffer_length;
    ///hashing algorithm
    uint8_t hash_algo;
} optiga_hash_context_t;

/**
 * \brief Specifies the structure to provide the details of data to be hashed from host.
 */
typedef struct hash_data_from_host
{
    /// data to hash
    const uint8_t * buffer;
    /// Length of data
    uint32_t length;
} hash_data_from_host_t;

/**
 * \brief Specifies the structure to provide the details of data to be hashed from OPTIGA.
 */
typedef struct hash_data_in_optiga
{
    ///OID of data object
    uint16_t oid;
    ///Offset within the data object
    uint16_t offset;
    ///Number of data bytes starting from the offset
    uint16_t length;
} hash_data_in_optiga_t;

/**
 * \brief Specifies the data structure of the Public Key details (key, size and type)
 */
typedef struct public_key_from_host
{
    /// Pointer to Public Key
    uint8_t * public_key;
    /// Length of public key buffer
    uint16_t length;
    /// Public key type details. For ECC key use #optiga_ecc_curve_t and for RSA key use #optiga_rsa_key_type_t
    uint8_t key_type;
} public_key_from_host_t;

/**
 * \brief Specifies the data structure for data to be read from OPTIGA
 */
typedef struct optiga_get_data_object
{
    /// Pointer to the read buffer length
    uint16_t * ref_bytes_to_read;
    /// Read data buffer pointer
    uint8_t * buffer;	
    /// Object ID to be read
    uint16_t oid;
    /// Offset of data with the object ID
    uint16_t offset;
    /// Data size to be read
    uint16_t bytes_to_read;
    /// Contains length of data received in across multiple transceive calls. Used for chaining purpose
    uint16_t accumulated_size;
    /// Contains the data length received in last transceive. Used for chaining purpose
    uint16_t last_read_size;
    /// Read to data or metadata
    uint8_t data_or_metadata;
} optiga_get_data_object_params_t;

/**
 * \brief Specifies the data structure for data to be written to OPTIGA
 */
typedef struct optiga_set_data_object
{
    /// Wrtie data buffer pointer
    const uint8_t * buffer;	
    /// Object ID to be written
    uint16_t oid;
    /// Offset of data with the object ID
    uint16_t offset;
    /// Write data size
    uint16_t size;
    /// Contains length of data written in across multiple transceive calls. Used for chaining purpose
    uint16_t written_size;
    /// Write to data or metadata
    uint8_t data_or_metadata;
    /// Type of write - Write only or Erase and write
    uint8_t write_type;
    ///Count value
    uint8_t count;
} optiga_set_data_object_params_t;

/**
 * \brief Specifies the data structure of calculate hash
 */
typedef struct optiga_calc_hash
{
    ///OID hash pointer
    hash_data_in_optiga_t * p_hash_oid;
    ///Data buffer pointer
    hash_data_from_host_t * p_hash_data;
    /// Context buffer pointer
    optiga_hash_context_t * p_hash_context;
    ///Out digest
    uint8_t * p_out_digest;	
    ///Data length has been sent
    uint32_t data_sent;
    ///Possible context size to send in a fragment
    uint32_t apparent_context_size;	
    ///Type of hash operation
    uint8_t hash_sequence;
    ///Current type of hash operation
    uint8_t current_hash_sequence;    
    ///export hash ctx
    bool_t export_hash_ctx;
} optiga_calc_hash_params_t;


/**
 * \brief Specifies the data structure of random generation
 */
typedef struct optiga_get_random
{
    /// User buffer for storing random data
    uint8_t * random_data;
    /// User buffer which holds the optional data
    const uint8_t * optional_data;	
    /// Random data length
    uint16_t random_data_length;
    /// Optional data length
    uint16_t optional_data_length; 
    /// Use to indicate to acquire session
    bool_t store_in_session;
} optiga_get_random_params_t;


/**
 * \brief Specifies the data structure for generate key pair
 */
typedef struct optiga_gen_keypair
{
    ///  Private key buffer pointer
    uint8_t * private_key;
    /// Private key length
    uint16_t * private_key_length;
    /// Public key buffer pointer
    uint8_t * public_key;
    /// Public key length
    uint16_t * public_key_length;
    /// Type of public key OID
    optiga_key_id_t private_key_oid;	
    /// Key usage type
    uint8_t key_usage;
    /// Private key export option
    bool_t export_private_key;
} optiga_gen_keypair_params_t;

/**
 * \brief Specifies the data structure for ECDSA signature
 */
typedef struct optiga_calc_sign
{
    /// Digest buffer pointer
    const uint8_t * p_digest;
    /// Signature buffer pointer
    uint8_t * p_signature;
    /// Signature length
    uint16_t * p_signature_length;
    /// OID of the Private Key (either Key store or Session based). Refer #optiga_key_id_t for possible values.
    optiga_key_id_t private_key_oid;
    /// Digest data length
    uint8_t digest_length;
} optiga_calc_sign_params_t;

/**
 * \brief Specifies the data structure for ECDSA signature verification
 */
typedef struct optiga_verify_sign
{
    /// Digest buffer pointer
    const uint8_t * p_digest;
    /// Signature buffer pointer
    const uint8_t * p_signature;
    /// Public key provided by host
    public_key_from_host_t * public_key;
    /// Signature data length
    uint16_t signature_length;	
    /// Public key certificate OID
    uint16_t certificate_oid;
    /// Digest data length
    uint8_t digest_length;
    /// Source of provided public key
    uint8_t public_key_source_type;	
} optiga_verify_sign_params_t;

/**
 * \brief Specifies the data structure for ecdh secret generation
 */
typedef struct optiga_calc_ssec
{
    /// Public Key of the peer
    public_key_from_host_t * public_key;
    /// Pointer to a buffer where the exported shared secret to be stored.
    uint8_t * shared_secret;
    /// OID of the Private Key (either Key store or Session based). Refer #optiga_key_id_t for possible values.
    optiga_key_id_t private_key;
    /// Export to Host (store in OPTIGA Session or export to host)
    uint8_t export_to_host;
} optiga_calc_ssec_params_t;

/**
 * \brief Specifies the structure for derivation of key using pseudo random function
 */
typedef struct optiga_derive_key
{
    /// Random Seed/Salt
    const uint8_t * random_data;
    /// Label input as a constant string
    const uint8_t * label;
    /// Application specific info
    const uint8_t * info;    
    /// Pointer to a buffer where the exported key to be stored.
    uint8_t * derived_key;
    /// Session based (#optiga_key_id_t) or Data object which has the pre-shared secret
    uint16_t input_shared_secret_oid;
    /// Random Seed/Salt length
    uint16_t random_data_length;
    /// Label length
    uint16_t label_length;
    /// Info length
    uint16_t info_length;
    /// Derived Key length
    uint16_t derived_key_length;
}optiga_derive_key_params_t;

#if defined (OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED) || defined (OPTIGA_CRYPT_RSA_DECRYPT_ENABLED)
/**
 * \brief Specifies the structure for asymmetric encryption and decryption
 */
typedef struct optiga_enc_dec_asym
{
    /// Pointer to the length of the encrypted or decrypted message
    uint16_t * processed_message_length;
    /// Public key provided by host
    const void * key;
    /// Pointer to buffer where encrypted or decrypted message is stored
    uint8_t * processed_message;
    /// Message to be encrypted. Set NULL if data from session OID
    const uint8_t * message;	
    /// Length of message to be encrypted. Set 0 if data from session OID
    uint16_t message_length;
    /// Store private key OID
    optiga_key_id_t private_key_id;
    /// Source of provided public key for encryption and Private key for decryption
    uint8_t public_key_source_type;
}optiga_encrypt_asym_params_t,optiga_decrypt_asym_params_t;
#endif

/**
 * \brief Specifies the data structure for protected update
 */
typedef struct optiga_set_object_protected_params
{
    /// Pointer to the buffer which contains manifest/fragment
    const uint8_t * p_protected_update_buffer;
    /// Manifest/Fragment length
    uint16_t p_protected_update_buffer_length;
    /// Set protected object tag
    optiga_set_obj_protected_tag_t set_obj_protected_tag;
    /// manifest version
    uint8_t manifest_version;
} optiga_set_object_protected_params_t;

#if defined (OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) || defined (OPTIGA_CRYPT_SYM_DECRYPT_ENABLED)
/**
 * \brief Specifies the data structure for symmetric encrypt and decrypt
 */
typedef struct optiga_symmetric_enc_dec_params
{
    /// Pointer to plain text
    const uint8_t * in_data;
    /// Pointer to initialization vector
    const uint8_t * iv;	
    /// Pointer to associated data
    const uint8_t * associated_data;
    /// Pointer to output data
    uint8_t * out_data;
    /// Length of output data
    uint32_t * out_data_length;
    /// Pointer to generated hmac
    const uint8_t * generated_hmac;
    /// Length of sent data
    uint32_t sent_data_length;
    /// Length of received data
    uint32_t received_data_length;	
    /// Length of generated hmac
    uint32_t generated_hmac_length;
    /// Length of plain text
    uint32_t in_data_length;
    /// Symmetric key OID
    uint16_t symmetric_key_oid;
    /// Length of initialization vector
    uint16_t iv_length;
    /// Length of associated data
    uint16_t associated_data_length;
    /// Variable to indicate complete input data length required for CCM
    uint16_t total_input_data_length;	
    /// Requested sequence
    uint8_t  original_sequence;
    /// Variable to store current encrypt decrypt sequence
    uint8_t  current_sequence;
    /// Encryption or hmac mode
    uint8_t mode;
    /// Symmetric mode of operation
    uint8_t operation_mode;
} optiga_encrypt_sym_params_t,optiga_decrypt_sym_params_t;
#endif
#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
/**
 * \brief Specifies the data structure for symmetric generate key 
 */
typedef struct optiga_gen_symkey_params
{
    /// Symmetric key buffer pointer or oid pointer
    void * symmetric_key;	
    /// Key usage type
    uint8_t key_usage;
    /// Symmetric key export option
    bool_t export_symmetric_key;
} optiga_gen_symkey_params_t;
#endif
/**
 * \brief Prepares uint32 [Big endian] type value from the buffer and store
 *
 * \details
 * Prepares uint32 [Big endian] type value from the buffer and store
 * - Return first 4 bytes from input buffer as uint32_t.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in]  p_input_buffer          Pointer to the buffer
 *
 * \retval     return 32 bit value
 *
 */
uint32_t optiga_common_get_uint32(const uint8_t* p_input_buffer);

/**
 * \brief Copies 2 bytes of uint16 type value to the buffer
 *
 * \details
 * Copies 2 bytes of uint16 type value to the buffer
 * - Copies the 2 bytes value to input buffer in big endian format.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in,out]  p_output_buffer         Pointer to the buffer
 * \param[in]      two_byte_value          16 bit value
 *
 */
void optiga_common_set_uint16(uint8_t * p_output_buffer,
                              uint16_t two_byte_value);

/**
 * \brief Copies 4 bytes of uint32 [Big endian] type value to the buffer and stores in the output pointer
 *
 * \details
 * Copies 4 bytes of uint32 [Big endian] type value to the buffer and store
 * - Copies the 4 bytes value to input buffer in big endian format.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in,out]  p_output_buffer          Pointer to the buffer
 * \param[in]      four_byte_value          32 bit value
 *
 */
void optiga_common_set_uint32(uint8_t* p_output_buffer,
                              uint32_t four_byte_value);

/**
 * \brief Prepares uint16 [Big endian] type value from the buffer and stores in the output pointer
 *
 * \details
 * Prepares uint16 [Big endian] type value from the buffer and store
 * - Return first 2 bytes from input buffer as uint16_t.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in]      p_input_buffer    Pointer to the buffer
 * \param[in,out]  p_two_byte_value  Pointer to the value tobe assigne
 *
 */
void optiga_common_get_uint16(const uint8_t * p_input_buffer,
                              uint16_t* p_two_byte_value);

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_LIB_COMMON_H_ */

/**
* @}
*/
