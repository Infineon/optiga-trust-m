/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_crypt.h
 *
 * \brief   This file implements the prototype declarations of OPTIGA Crypt module.
 *
 * \ingroup  grOptigaCrypt
 *
 * @{
 */

#ifndef _OPTIGA_CRYPT_H_
#define _OPTIGA_CRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_cmd.h"

/** \brief union for OPTIGA crypt parameters */
typedef union optiga_crypt_params {
    /// get random params
    optiga_get_random_params_t optiga_get_random_params;
    /// get key pair params
    optiga_gen_keypair_params_t optiga_gen_keypair_params;
    /// calc sign params
    optiga_calc_sign_params_t optiga_calc_sign_params;
    /// verify sign params
    optiga_verify_sign_params_t optiga_verify_sign_params;
#if defined(OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED) || defined(OPTIGA_CRYPT_RSA_DECRYPT_ENABLED)
    /// asymmetric encryption params
    optiga_encrypt_asym_params_t optiga_encrypt_asym_params;
#endif
    /// calc hash params
    optiga_calc_hash_params_t optiga_calc_hash_params;
    /// calc ssec params
    optiga_calc_ssec_params_t optiga_calc_ssec_params;
    /// derive key params
    optiga_derive_key_params_t optiga_derive_key_params;
#if defined(OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) && defined(OPTIGA_CRYPT_SYM_DECRYPT_ENABLED)
    /// derive key params
    optiga_encrypt_sym_params_t optiga_symmetric_enc_dec_params;
#endif
#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
    /// generate symmetric key params
    optiga_gen_symkey_params_t optiga_gen_sym_key_params;
#endif
} optiga_crypt_params_t;

/** \brief OPTIGA crypt instance structure */
struct optiga_crypt {
    /// Details/references (pointers) to the Application Inputs
    optiga_crypt_params_t params;
    /// Command module instance
    optiga_cmd_t *my_cmd;
    /// Caller context
    void *caller_context;
    /// Callback handler
    callback_handler_t handler;
    /// To provide the busy/free status of the crypt instance
    uint16_t instance_state;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide the encryption and decryption need for command and response
    uint8_t protection_level;
    /// To provide the presentation layer protocol version to be used
    uint8_t protocol_version;
#endif  // OPTIGA_COMMS_SHIELDED_CONNECTION
};

/** \brief OPTIGA crypt instance structure type*/
typedef struct optiga_crypt optiga_crypt_t;

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
/**
 * \brief Sets/updates the OPTIGA Comms Shielded connection configuration in
 *        the respective (optiga_util) instance.
 *
 *\details
 * Sets/updates the OPTIGA Comms Shielded connection configuration in the respective (optiga_util) instance.
 * - The #OPTIGA_COMMS_PROTECTION_LEVEL configuration settings using this API, will be used in the next immediate usage of the instance.
 * - Once the API is invoked, this level gets reset to the default protection level #OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL
 *
 *\pre
 * - None
 *
 *\note
 * - None
 *
 * \param[in,out]  me                     Valid instance of #optiga_util_t
 * \param[in]      parameter_type         Parameter Type
 *                                        Possible Types are
 *                                        #OPTIGA_COMMS_PROTECTION_LEVEL
 *                                        #OPTIGA_COMMS_PROTOCOL_VERSION
 *
 * \param[in]      value                  Value part for the respective configuration
 *
 * <b>Example</b><br>
 *
 */
void optiga_crypt_set_comms_params(optiga_crypt_t *me, uint8_t parameter_type, uint8_t value);
#endif

/**
 * \brief Create an instance of #optiga_crypt_t.
 *
 * \details
 * Create an instance of #optiga_crypt_t.
 * - Stores the callers context and callback handler.
 * - Allocate memory for #optiga_crypt_t.
 *
 * \pre
 * - None
 *
 * \note
 * - This API is implemented in synchronous mode.
 * - For <b>protected I2C communication</b>,
 *      - Default protection level for this API is #OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL.
 *      - Default protocol version for this API is #OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET.
 *
 * \param[in]   optiga_instance_id  Indicates the OPTIGA instance to be associated with #optiga_crypt_t. Should be defined as below:
 *                                  Use #OPTIGA_INSTANCE_ID_0.
 * \param[in]   handler             Pointer to callback function, must not be NULL.
 * \param[in]   caller_context      Pointer to upper layer context. Contains user context data.
 *
 * \retval      #optiga_crypt_t     On success function will return pointer of #optiga_crypt_t.
 * \retval      NULL                Input arguments are NULL.<br>
 *                                  Low layer function fails.<br>
 *                                  OPTIGA_CMD_MAX_REGISTRATIONS number of instances are already created.
 */
LIBRARY_EXPORTS optiga_crypt_t *
optiga_crypt_create(uint8_t optiga_instance_id, callback_handler_t handler, void *caller_context);

/**
 * \brief Destroys an instance of #optiga_crypt_t.
 *
 * \details
 * Destroys the #optiga_crypt_t instance.
 * - De-allocate the memory of the #optiga_crypt_t instance.
 *
 * \pre
 * - An instance of optiga_crypt using #optiga_crypt_create must be available.<br>
 *
 * \note
 *  - User must take care to nullify the instance pointer.
 *  - Invoke this API only after all the asynchronous process is completed otherwise the behavior of software stack is not defined.
 *
 * \param[in] me                                      Valid instance of #optiga_crypt_t.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                    Successful invocation.
 * \retval    #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval    #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_destroy(optiga_crypt_t *me);

#ifdef OPTIGA_CRYPT_RANDOM_ENABLED
/**
 * \brief Generates a random number.
 *
 * \details
 * Generates the requested random stream of data for the user provided length.
 * - Invokes #optiga_cmd_get_random API, based on the input arguments to retrieve random data .
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.<br>
 * - The maximum value of the <b>random_data_length</b> parameter is size of buffer <b>random_data</b>.
 *   In case the value is greater than buffer size, memory corruption can occur.<br>
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      rng_type                                 Type of random data generator.
 *                                                          - The input must be from #optiga_rng_type.
 *                                                          - Argument check for rng_type is not done since OPTIGA will provide an error for invalid rng_type.
 * \param[in,out]  random_data                              Pointer to the buffer into which random data is stored, must not be NULL.
 * \param[in]      random_data_length                       Length of random data to be generated.
 *                                                          - Range should be 8 - 256 bytes.
 *                                                          - Length validation is not done, since OPTIGA will provide an error for invalid random_data_length.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                    Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_random.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_random(
    optiga_crypt_t *me,
    optiga_rng_type_t rng_type,
    uint8_t *random_data,
    uint16_t random_data_length
);
#endif  // OPTIGA_CRYPT_RANDOM_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED
/**
 *
 * \brief Updates a hashing for input data and returns digest.
 *
 * \details
 * Updates hashing for the given data and returns digest.<br>
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layer will be returned as it is.<br>
 *
 *<br>
 * \param[in]      me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      hash_algorithm                            Hash algorithm of #optiga_hash_type_t.
 * \param[in]      source_of_data_to_hash                    Data from host / Data in OPTIGA. Must be one of the below
 *                                                           - #OPTIGA_CRYPT_HOST_DATA or Non-Zero value ,if source of data is from Host.
 *                                                           - #OPTIGA_CRYPT_OID_DATA, if the source of data is from OPTIGA.
 * \param[in]      data_to_hash                              Data for hashing either in #hash_data_from_host_t or in #hash_data_in_optiga_t
 * \param[inout]   hash_output                               Pointer to the valid buffer to store hash output.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                     Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT         Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                           (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hash.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hash(
    optiga_crypt_t *me,
    optiga_hash_type_t hash_algorithm,
    uint8_t source_of_data_to_hash,
    const void *data_to_hash,
    uint8_t *hash_output
);

/**
 *
 * \brief Initializes a hash context.
 *
 * \details
 * Sets up a hash context and exports it.
 * - Invokes #optiga_cmd_calc_hash API, based on the input arguments.<br>
 * - Initializes a new hash context.<br>
 * - Exports the hash context to caller.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.<br>
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layer will be returned as it is.<br>
 * - User must save the output hash context for further usage because OPTIGA does not store it internally.<br>
 *
 *<br>
 * \param[in]      me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[inout]   hash_ctx                                  Pointer to #optiga_hash_context_t to store the hash context from OPTIGA.
 *                                                           - The minimum size of the <b>context_buffer</b> must be 209 bytes in #optiga_hash_context_t for hash algo #OPTIGA_HASH_TYPE_SHA_256.
 *                                                           - The input <b>hash_algo</b> in  <b>hash_ctx</b> must be from #optiga_hash_type.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                     Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT         Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                           (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hash.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_crypt_hash_start(optiga_crypt_t *me, optiga_hash_context_t *hash_ctx);

/**
 * \brief Updates a hash context with the input data.
 *
 * \details
 * Updates hashing for the given data and hash context then export the updated hash context.<br>
 * - Invokes #optiga_cmd_calc_hash API, based on the input arguments.<br>
 * - Update the input hash context.<br>
 * - Exports the hash context to caller.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.<br>
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 * - #optiga_hash_context_t from #optiga_crypt_hash_start or #optiga_crypt_hash_update must be available.
 *
 * \note<br>
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layer will be returned as it is.<br>
 * - User must save the output hash context for further usage as OPTIGA does not store it internally.
 *
 * \param[in]   me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]   hash_ctx                                Pointer to #optiga_hash_context_t containing hash context from OPTIGA, must not be NULL
 *                                                      - The minimum size of the <b>context_buffer</b> must be 209 bytes in #optiga_hash_context_t for hash algo #OPTIGA_HASH_TYPE_SHA_256.
 * \param[in]   source_of_data_to_hash                  Data from host / Data in optiga. Must be one of the below
 *                                                      - #OPTIGA_CRYPT_HOST_DATA or Non-Zero value ,if source of data is from Host.
 *                                                      - #OPTIGA_CRYPT_OID_DATA, if the source of data is from OPITGA.
 * \param[in]   data_to_hash                            Data for hashing either in #hash_data_from_host or in #hash_data_in_optiga
 *
 * \retval      #OPTIGA_CRYPT_SUCCESS                   Successful invocation.
 * \retval      #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval      #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 * \retval      #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                      (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hash.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hash_update(
    optiga_crypt_t *me,
    optiga_hash_context_t *hash_ctx,
    uint8_t source_of_data_to_hash,
    const void *data_to_hash
);

/**
 *
 * \brief Finalizes and exports the hash output.
 *
 * \details
 * Finalizes the hash context and returns hash as output.
 * - Invokes #optiga_cmd_calc_hash API, based on the input arguments.<br>
 * - Finalize the hash from the input hash context
 * - Exports the finalized hash.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 * - #optiga_hash_context_t from #optiga_crypt_hash_start or #optiga_crypt_hash_update must be available.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layer will be returned as it is.
 * - hash context is not updated by this API. This can be used later to fulfill intermediate hash use-cases.
 * - User must save the output hash context for further usage as OPTIGA does not store it internally.
 *
 * \param[in]      me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      hash_ctx                                Pointer to #optiga_hash_context_t containing hash context from OPTIGA, must not be NULL.
 *                                                         - The minimum size of the <b>context_buffer</b> must be 209 bytes in #optiga_hash_context_t for hash algo #OPTIGA_HASH_TYPE_SHA_256.
 * \param[inout]   hash_output                             Output Hash.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                   Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                         (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hash.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hash_finalize(
    optiga_crypt_t *me,
    optiga_hash_context_t *hash_ctx,
    uint8_t *hash_output
);

#endif  // OPTIGA_CRYPT_HASH_ENABLED

#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

/**
 * \brief Generates a key pair based on ECC curves.
 *
 * \details
 * Generates an ECC key-pair based on the input curve.
 * - Invokes #optiga_cmd_gen_keypair API, based on the input arguments.<br>
 * - Generate an ECC key pair using OPTIGA.
 * - Private key is exported only if explicitly requested otherwise it is stored in the input private key OID.
 * - Public key is always exported.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 *
 * \param[in]       me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]       curve_id                                ECC curve id as #optiga_ecc_curve.
 * \param[in]       key_usage                               Key usage defined in #optiga_key_usage_t.
 *                                                          - Values from #optiga_key_usage can be logically ORed and passed.<br>
 *                                                          - It is ignored if export_private_key is TRUE (1).
 * \param[in]       export_private_key                      TRUE (1) or Non-Zero value - Exports private key to the host.<br>
 *                                                          FALSE (0) - Exports only public key to the host and writes private key to OPTIGA. The input key_usage is ignored.
 * \param[in]       private_key                             Buffer to store private key or private key OID of OPTIGA, must not be NULL.
 *                                                          - If export_private_key is TRUE, assign pointer to a buffer to store private key.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the key type and additional DER encoding formats.
 *                                                          - If export_private_key is FALSE, assign pointer to variable of type #optiga_key_id_t.
 * \param[in,out]   public_key                              Buffer to store public key, must not be NULL.
 * \param[in]       public_key_length                       Initially set as length of buffer to store public_key, later updated as actual length of public_key.
 *
 * \retval          #OPTIGA_CRYPT_SUCCESS                   Successful invocation.
 * \retval          #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval          #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 * \retval          #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecc_generate_keypair.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_ecc_generate_keypair(
    optiga_crypt_t *me,
    optiga_ecc_curve_t curve_id,
    uint8_t key_usage,
    bool_t export_private_key,
    void *private_key,
    uint8_t *public_key,
    uint16_t *public_key_length
);
#endif  // OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
/**
 * \brief Generates a signature for the given digest.
 *
 * \details
 * Generates a signature for the given digest using private key stored in OPTIGA.
 * - Invokes #optiga_cmd_calc_sign API, based on the input arguments.<br>
 * - Generates signature for the input digest.
 * - Exports the generated signature.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 * - If the private_key type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecc_generate_keypair )
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      digest                                   Digest on which signature is generated.
 * \param[in]      digest_length                            Length of the input digest.
 * \param[in]      private_key                              Private key OID to generate signature.
 * \param[in,out]  signature                                Pointer to store generated signature, must not be NULL.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the additional
 *                                                          DER encoding formatting for R and S components of signature.
 * \param[in,out]  signature_length                         Length of signature. Initial value set as length of buffer, later updated as the actual length of generated signature.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                    Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
                                                            Session is not available in instance and the private_key type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecdsa_sign.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_ecdsa_sign(
    optiga_crypt_t *me,
    const uint8_t *digest,
    uint8_t digest_length,
    optiga_key_id_t private_key,
    uint8_t *signature,
    uint16_t *signature_length
);
#endif  // OPTIGA_CRYPT_ECDSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
/**
 * \brief Verifies the signature over the given digest.
 *
 * \details
 * Verifies the signature over a given digest provided with the input data.
 * - Invokes #optiga_cmd_verify_sign API, based on the input arguments.<br>
 * - Verifies the signature over the given provided with the input data using public key.
 * - It invokes the callback handler of the instance, when it is asynchronously completed.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]   me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]   digest                                    Pointer to a given digest buffer, must not be NULL.
 * \param[in]   digest_length                             Length of digest.
 * \param[in]   signature                                 Pointer to a given signature buffer, must not be NULL.
 * \param[in]   signature_length                          Length of signature.
 * \param[in]   public_key_source_type                    Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
 *                                                        - #OPTIGA_CRYPT_OID_DATA, if the public key is to used from the certificate data object from OPTIGA.
 *                                                        - #OPTIGA_CRYPT_HOST_DATA  or Non-Zero value , if the public key is provided by host.
 * \param[in]   public_key                                Public key from host / OID of certificate object. Value must be one of the below
 *                                                        - For certificate OID, pointer OID value must be passed.
 *                                                        - For Public key from host, pointer to #public_key_from_host_t instance.
 *
 * \retval      #OPTIGA_CRYPT_SUCCESS                     Successful invocation
 * \retval      #OPTIGA_CRYPT_ERROR_INVALID_INPUT         Wrong Input arguments provided
 * \retval      #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete
 * \retval      #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecdsa_verify.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_ecdsa_verify(
    optiga_crypt_t *me,
    const uint8_t *digest,
    uint8_t digest_length,
    const uint8_t *signature,
    uint16_t signature_length,
    uint8_t public_key_source_type,
    const void *public_key
);
#endif  // OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_ECDH_ENABLED
/**
 * \brief Calculates the shared secret using ECDH algorithm.<br>
 *
 * \details
 * Calculates the shared secret using ECDH algorithm
 * - Invokes #optiga_cmd_calc_ssec API, based on the input arguments.<br>
 * - Calculates the shared secret based on input private key object ID and public key.
 * - Based on user request (export_to_host), the shared secret can either be exported to the host or be stored in the acquired session object ID.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 * - There must be a private key available in the "session context / data object OID" provided as input parameter.
 * - If the private_key type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecc_generate_keypair )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 * - The buffer size for shared secret should be appropriately provided by the user
 * - If the user provides <b>private_key</b> as session based and <b>export_to_host</b> as FALSE,<br>
 *   then the shared secret generated will overwrite the private key stored in the session object ID
 *
 * \param[in]      me                                          Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      private_key                                 Object ID of the private key stored in OPTIGA.<br>
 *                                                             - Possible values are from the #optiga_key_id_t <br>
 *                                                             - Argument check for private_key is not done since OPTIGA will provide an error for invalid private_key.
 * \param[in]      public_key                                  Pointer to the public key structure for shared secret generation with its properties, must not be NULL.<br>
 *                                                             - Provide the inputs according to the structure type #public_key_from_host_t
 * \param[in]      export_to_host                              TRUE (1) or Non-Zero value - Exports the generated shared secret to Host. <br>
 *                                                             FALSE (0) - Stores the generated shared secret into the session object ID acquired by the instance.
 * \param[in,out]  shared_secret                               Pointer to the shared secret buffer, only if <b>export_to_host</b> is TRUE. The size of the buffer shall be equal or more than the key size.<br>
 *                                                             Otherwise provide NULL as input.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                       Successful invocation
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT           Wrong Input arguments provided.<br>
                                                               Session is not available in instance and the private_key type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE         The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                        Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecdh.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_ecdh(
    optiga_crypt_t *me,
    optiga_key_id_t private_key,
    public_key_from_host_t *public_key,
    bool_t export_to_host,
    uint8_t *shared_secret
);
#endif  // OPTIGA_CRYPT_ECDH_ENABLED

#if defined(OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED) || defined(OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED) \
    || defined(OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED)
/**
 * \brief Derives a key.<br>
 *
 * \details
 * Derives a key using the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - Stores the derived key into OPTIGA session context or export it to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 * - If the secret type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecdh )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID..
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         type                                     Type of scheme to be used for TLS PRF as #optiga_tls_prf_type_t.
 * \param[in]         secret                                   Object ID where the secret is stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         label                                    Pointer to the label, can be NULL if not applicable.
 * \param[in]         label_length                             Length of the label.
 * \param[in]         seed                                     Valid pointer to the seed, must not be NULL.
 * \param[in]         seed_length                              Length of the seed.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        - Wrong Input arguments provided
 *                                                             - Attempt to use a session OID which is not acquired in #optiga_crypt_ecdh
                                                               Session is not available in instance and the secret type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_tls_prf_sha256.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_tls_prf(
    optiga_crypt_t *me,
    optiga_tls_prf_type_t type,
    uint16_t secret,
    const uint8_t *label,
    uint16_t label_length,
    const uint8_t *seed,
    uint16_t seed_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
);
#endif  // OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED || OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED || OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED

#ifdef OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
/**
 * \brief Derives a key using TLS PRF SHA256.<br>
 *
 * \details
 * Derives a key TLS PRF SHA256 using the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - Stores the derived key into OPTIGA session context or export it to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 * - Try to access session OID without acquiring session in #optiga_crypt_ecdh throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID where the secret is stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         label                                    Pointer to the label, can be NULL if not applicable.
 * \param[in]         label_length                             Length of the label.
 * \param[in]         seed                                     Valid pointer to the seed, must not be NULL.
 * \param[in]         seed_length                              Length of the seed.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided
 *                                                             - Attempt to use a session OID which is not acquired in #optiga_crypt_ecdh
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_tls_prf_sha256.c
 *
 */
_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_tls_prf_sha256(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *label,
    uint16_t label_length,
    const uint8_t *seed,
    uint16_t seed_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_tls_prf(
        me,
        OPTIGA_TLS12_PRF_SHA_256,
        secret,
        label,
        label_length,
        seed,
        seed_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}
#endif  // OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
#ifdef OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED
/**
 * \brief Derives a key using TLS PRF SHA384.<br>
 *
 * \details
 * Derives a key using the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - Stores the derived key into OPTIGA session context or export it to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID where the secret is stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         label                                    Pointer to the label, can be NULL if not applicable.
 * \param[in]         label_length                             Length of the label.
 * \param[in]         seed                                     Valid pointer to the seed, must not be NULL.
 * \param[in]         seed_length                              Length of the seed.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided
 *                                                             - Attempt to use a session OID which is not acquired in #optiga_crypt_ecdh
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * None
 *
 */
_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_tls_prf_sha384(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *label,
    uint16_t label_length,
    const uint8_t *seed,
    uint16_t seed_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_tls_prf(
        me,
        OPTIGA_TLS12_PRF_SHA_384,
        secret,
        label,
        label_length,
        seed,
        seed_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}
#endif  // OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED
#ifdef OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED
/**
 * \brief Derives a key using TLS PRF SHA512.<br>
 *
 * \details
 * Derives a key using the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - Stores the derived key into OPTIGA session context or export it to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID where the secret is stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         label                                    Pointer to the label, can be NULL if not applicable.
 * \param[in]         label_length                             Length of the label.
 * \param[in]         seed                                     Valid pointer to the seed, must not be NULL.
 * \param[in]         seed_length                              Length of the seed.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided
 *                                                             - Attempt to use a session OID which is not acquired in #optiga_crypt_ecdh
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * None
 *
 */
_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_tls_prf_sha512(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *label,
    uint16_t label_length,
    const uint8_t *seed,
    uint16_t seed_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_tls_prf(
        me,
        OPTIGA_TLS12_PRF_SHA_512,
        secret,
        label,
        label_length,
        seed,
        seed_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}
#endif  // OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED

#ifdef OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED

/**
 * \brief Generates a key pair based on RSA key type.
 *
 * \details
 * Generates a RSA key-pair based on the type of the key.
 * - Invokes #optiga_cmd_gen_keypair API, based on the input arguments.<br>
 * - Generate an RSA key pair using OPTIGA.
 * - If export is requested, exports the private key else stores it in the input private key OID.
 * - Always exports the public keys.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened before using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 * - RSA key pair generation on OPTIGA can go beyond 50 seconds therefore use a larger value for optiga comms TL_MAX_EXIT_TIMEOUT.
 *   The default value is hence set to 180 seconds.
 *
 * \param[in]       me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]       key_type                                RSA key type defined in #optiga_rsa_key_type_t.
 * \param[in]       key_usage                               Key usage defined in #optiga_key_usage_t.
 *                                                          - Values from #optiga_key_usage can be logically ORed and passed.<br>
 *                                                          - It is ignored if export_private_key is TRUE (1) or non-zero.
 * \param[in]       export_private_key                      TRUE (1) or a non-zero value - Exports private key to the host.<br>
 *                                                          FALSE (0) - Exports only public key to the host and writes private key to OPTIGA. The input key_usage is ignored.
 * \param[in,out]       private_key                             Buffer to store private key or private key OID of OPTIGA, must not be NULL.
 *                                                          - If export_private_key is TRUE, assign a pointer to a buffer to store the generated private key.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the key type and additional DER encoding formats.
 *                                                          - If export_private_key is FALSE, assign a pointer to variable of type #optiga_key_id_t.
 * \param[in,out]   public_key                              Buffer to store public key, must not be NULL.
 * \param[in,out]       public_key_length                       Initially set as length of public_key, later updated as actual length of public_key.
 *
 * \retval          #OPTIGA_CRYPT_SUCCESS                   Successful invocation
 * \retval          #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval          #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval          #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_rsa_generate_keypair.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_generate_keypair(
    optiga_crypt_t *me,
    optiga_rsa_key_type_t key_type,
    uint8_t key_usage,
    bool_t export_private_key,
    void *private_key,
    uint8_t *public_key,
    uint16_t *public_key_length
);
#endif  // OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_RSA_SIGN_ENABLED
/**
 * \brief Generates a RSA signature for the given digest based on the input signature scheme.
 *
 * \details
 * Generates a signature for the given digest using private key stored in OPTIGA.
 * - Invokes #optiga_cmd_calc_sign API, based on the input arguments.<br>
 * - Generates signature for the input digest.
 * - Exports the generated signature.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      signature_scheme                         RSA signature scheme defined in #optiga_rsa_signature_scheme_t
 * \param[in]      digest                                   Digest on which signature is generated.
 * \param[in]      digest_length                            Length of the input digest.
 * \param[in]      private_key                              Private key OID to generate signature. This is static private key only.
 * \param[in,out]  signature                                Pointer to store generated signature, must not be NULL.
 * \param[in]      signature_length                         Length of signature. Initial value set as length of buffer, later updated as the actual length of generated signature.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the signature.
 * \param[in]      salt_length                              Reserved for future use. Parameter for RSA PSS signature scheme.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                    Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_rsa_sign.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_sign(
    optiga_crypt_t *me,
    optiga_rsa_signature_scheme_t signature_scheme,
    const uint8_t *digest,
    uint8_t digest_length,
    optiga_key_id_t private_key,
    uint8_t *signature,
    uint16_t *signature_length,
    uint16_t salt_length
);
#endif  // OPTIGA_CRYPT_RSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_RSA_VERIFY_ENABLED
/**
 * \brief Verifies the RSA signature over the given digest.
 *
 * \details
 * Verifies the signature over a given digest provided with the input data.
 * - Invokes #optiga_cmd_verify_sign API, based on the input arguments.<br>
 * - Verifies the signature over the given provided with the input data using public key.
 * - It invokes the callback handler of the instance, when it is asynchronously completed.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]   me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]   signature_scheme                          RSA signature scheme defined in #optiga_rsa_signature_scheme_t
 * \param[in]   digest                                    Pointer to a given digest buffer, must not be NULL.
 * \param[in]   digest_length                             Length of digest.
 * \param[in]   signature                                 Pointer to a given signature buffer, must not be NULL.
 * \param[in]   signature_length                          Length of signature.
 * \param[in]   public_key_source_type                    Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
 *                                                        - #OPTIGA_CRYPT_OID_DATA, if the public key is to used from the certificate data object from OPTIGA.
 *                                                        - #OPTIGA_CRYPT_HOST_DATA or Non-Zero value , if the public key is provided by host.
 * \param[in]   public_key                                Public key from host / public key of certificate OID. Value must be one of the below
 *                                                        - For certificate OID, pointer to an OID value must be passed.
 *                                                        - For Public key from host, pointer to #public_key_from_host_t instance.
 * \param[in]   salt_length                               Reserved for future use. Parameter for RSA PSS signature scheme.
 *
 * \retval      #OPTIGA_CRYPT_SUCCESS                     Successful invocation
 * \retval      #OPTIGA_CRYPT_ERROR_INVALID_INPUT         Wrong Input arguments provided
 * \retval      #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete
 * \retval      #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_rsa_verify.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_verify(
    optiga_crypt_t *me,
    optiga_rsa_signature_scheme_t signature_scheme,
    const uint8_t *digest,
    uint8_t digest_length,
    const uint8_t *signature,
    uint16_t signature_length,
    uint8_t public_key_source_type,
    const void *public_key,
    uint16_t salt_length
);
#endif  // OPTIGA_CRYPT_RSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED
/**
 * \brief Generates a pre-master secret.
 *
 * \details
 * Generates a pre-master secret for RSA key exchange and stores in the acquired session
 * - Invokes #optiga_cmd_get_random API, based on the input arguments.
 * - Generates the pre-master secret
 * - Pre-master secret is stored in session OID.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.<br>
 * - Pre Master Secret :
 *      - Minimum length of generated pre master secret is 8 bytes.
 *      - The maximum length supported by OPTIGA is 48 bytes.
 *      - It is a concatenation of optional data and random secret.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      optional_data                            Optional data that gets prepended to the generated random secret.
 *                                                          - Can be set to NULL, if not required
 * \param[in]      optional_data_length                     Length of the optional data provided. It is ignored if optional_data is NULL
 *                                                          - Value can be up to 58 bytes
 *                                                          - Difference in shared secret length and optional data length is less than 8 bytes
 * \param[in]      pre_master_secret_length                 Length of the shared secret to be generated.
 *                                                          - Length validation for more than maximum length is not done, since OPTIGA will provide an error for an invalid shared secret length.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                    Successful invocation.
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
 *                                                          - optional_data_length is more than 40 bytes
 *                                                          - Difference in shared secret length and optional data length is less than 8 bytes
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_encrypt_session.c
 *
 */

LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_generate_pre_master_secret(
    optiga_crypt_t *me,
    const uint8_t *optional_data,
    uint16_t optional_data_length,
    uint16_t pre_master_secret_length
);
#endif  // OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

/**
 * \brief Encrypts message using RSA public key.<br>
 *
 * \details
 * Encrypts message using RSA public key which is either provided by the host or stored in OPTIGA.
 * - Invokes #optiga_cmd_encrypt_asym API, based on the input arguments.<br>
 * - Encrypts the input message using a RSA public key.
 * - The RSA public key either provided by the host or refers to a OID in OPTIGA containing the public key certificate.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - <b>encrypted_message_length</b> Initially, it contains the size of buffer provided by user to store the encrypted data. On successful encryption, this value is updated with actual length of the encrypted data.<br>
 *      In case of any error, the value is set to 0.
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_scheme                     RSA encryption scheme.
 * \param[in]         message                               Pointer to message to be encrypted.
 * \param[in]         message_length                        Length of the message to be encrypted.
 * \param[in]         label                                 Pointer to a label (Reserved for future use).
 * \param[in]         label_length                          Length of the label (Reserved for future use).
 * \param[in]         public_key_source_type                Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
 *                                                              - #OPTIGA_CRYPT_OID_DATA, if the public key is to be used from the certificate data object from OPTIGA.
 *                                                              - #OPTIGA_CRYPT_HOST_DATA or Non-Zero value , if the public key is provided by host.
 * \param[in]         public_key                            Public key from host / public key of certificate OID. Value must be one of the below
 *                                                              - For certificate OID, pointer to OID value must be passed.
 *                                                              - For Public key from host, pointer to #public_key_from_host_t instance.
 * \param[in,out]     encrypted_message                     Pointer to buffer to store encrypted message.
 * \param[in,out]     encrypted_message_length              Pointer to length of the encrypted message.
                                                                - Out length is depends on the key size.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_encrypt_message.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_encrypt_message(
    optiga_crypt_t *me,
    optiga_rsa_encryption_scheme_t encryption_scheme,
    const uint8_t *message,
    uint16_t message_length,
    const uint8_t *label,
    uint16_t label_length,
    uint8_t public_key_source_type,
    const void *public_key,
    uint8_t *encrypted_message,
    uint16_t *encrypted_message_length
);

/**
 * \brief Encrypts session data using RSA public key.<br>
 *
 * \details
 * Encrypts session data using RSA public key which is either provided by the host or stored in OPTIGA.
 * - Invokes #optiga_cmd_encrypt_asym API, based on the input arguments.<br>
 * - Encrypts the data in the session OID using a RSA public key.
 * - The RSA public key either provided by the host or refers to a OID in OPTIGA containing the public key certificate.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - A session OID must be acquired using #optiga_crypt_rsa_generate_pre_master_secret otherwise #OPTIGA_CMD_ERROR_INVALID_INPUT is returned.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *     - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION.
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - <b>encrypted_message_length</b> contains the size of buffer provided by user to store the encrypted data. On successful encryption, this value is updated with actual length of the encrypted data.<br>
 *      In case of any error, the value is set to 0.
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_scheme                     RSA encryption scheme.
 * \param[in]         label                                 Pointer to a label (used only is case of RSAES_OAEP_SHA256 otherwise ignored).
 * \param[in]         label_length                          Length of the label (used only is case of RSAES_OAEP_SHA256 otherwise ignored).
 * \param[in]         public_key_source_type                Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
 *                                                              - #OPTIGA_CRYPT_OID_DATA, if the public key is to be used from the certificate data object from OPTIGA.
 *                                                              - #OPTIGA_CRYPT_HOST_DATA  or Non-Zero value, if the public key is provided by host.
 * \param[in]         public_key                            Public key from host / public key of certificate OID. Value must be one of the below
 *                                                              - For certificate OID, pointer to OID value must be passed.
 *                                                              - For Public key from host, pointer to #public_key_from_host_t instance.
 * \param[in,out]     encrypted_message                     Pointer to buffer to store encrypted message.
 * \param[in,out]     encrypted_message_length              Pointer to length of the encrypted message.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_encrypt_session.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_encrypt_session(
    optiga_crypt_t *me,
    optiga_rsa_encryption_scheme_t encryption_scheme,
    const uint8_t *label,
    uint16_t label_length,
    uint8_t public_key_source_type,
    const void *public_key,
    uint8_t *encrypted_message,
    uint16_t *encrypted_message_length
);

#endif  // OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
/**
 * \brief Decrypts input data using OPTIGA private key and export it to the host.<br>
 *
 * \details
 * Decrypts input data using RSA private key from OPTIGA and exports the decrypted data to the host.
 * - Invokes #optiga_cmd_decrypt_asym API, based on the input arguments.<br>
 * - Decrypts the input data using a RSA private key.
 * - The RSA private key from OPTIGA is referred by #optiga_key_id_t.
 * - Decrypted data is exported back to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - <b>*message_length</b> Initially , it contains the size of buffer provided by user to store the decrypted data. On successful decryption, this value is updated with actual length of the decrypted data.<br>
 *      In case of any error, the value is set to 0.
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_scheme                     RSA encryption scheme.
 * \param[in]         encrypted_message                     Pointer to the data to be decrypted.
 * \param[in]         encrypted_message_length              Length of the data to be decrypted.
 * \param[in]         label                                 Pointer to a label (Reserved for future use).
 * \param[in]         label_length                          Length of the label (Reserved for future use).
 * \param[in]         private_key                           RSA private key in OPTIGA
 * \param[in,out]     message                               Pointer to buffer to store decrypted message.
 * \param[in,out]     message_length                        Pointer to length of the decrypted message.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_decrypt_and_export(
    optiga_crypt_t *me,
    optiga_rsa_encryption_scheme_t encryption_scheme,
    const uint8_t *encrypted_message,
    uint16_t encrypted_message_length,
    const uint8_t *label,
    uint16_t label_length,
    optiga_key_id_t private_key,
    uint8_t *message,
    uint16_t *message_length
);

/**
 * \brief Decrypts input data using OPTIGA private key and stores it in a OPTIGA session.<br>
 *
 * \details
 * Decrypts input data using RSA private key from OPTIGA and stores it in a OPTIGA session.
 * - Invokes #optiga_cmd_decrypt_asym API, based on the input arguments.<br>
 * - Decrypts the input data using a RSA private key.
 * - The RSA private key from OPTIGA is referred by #optiga_key_id_t.
 * - Stores the decrypted data in the acquired OPTIGA session.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_scheme                     RSA encryption scheme.
 * \param[in]         encrypted_message                     Pointer to the data to be decrypted.
 * \param[in]         encrypted_message_length              Length of the data to be decrypted.
 * \param[in]         label                                 Pointer to a label (Reserved for future use).
 * \param[in]         label_length                          Length of the label (Reserved for future use).
 * \param[in]         private_key                           RSA private key in OPTIGA.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_rsa_decrypt_and_store(
    optiga_crypt_t *me,
    optiga_rsa_encryption_scheme_t encryption_scheme,
    const uint8_t *encrypted_message,
    uint16_t encrypted_message_length,
    const uint8_t *label,
    uint16_t label_length,
    optiga_key_id_t private_key
);

#endif  // OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

#ifdef OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED
/**
 * \brief Encrypt the data using symmetric encryption mode and export encrypted message to host.<br>
 *
 * \details
 * Encrypt the input message using symmetric key from OPTIGA and export the encrypted message to host.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Encrypts the input message based on the encryption mode.
 * - Use the symmetric key referred by symmetric key OID.
 * - Exports the encrypted message back to host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_COMMAND_PROTECTION.
 * - No internal padding is performed by OPTIGA, hence plain data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_mode                       Symmetric encryption mode
 * \param[in]         symmetric_key_oid                     OPTIGA symmetric key OID
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         plain_data                            Pointer to the data to be encrypted.
 * \param[in]         plain_data_length                     Length of the data to be encrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in]         iv                                    Pointer to an IV(initialization vector) or nonce.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         iv_length                             Length of the IV
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         associated_data                       Pointer to associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         associated_data_length                Length of associated data
 *                                                          - Only supported for CCM mode.
 * \param[in,out]     encrypted_data                        Pointer to buffer to store encrypted data
 * \param[in,out]     encrypted_data_length                 Pointer to length of the <b>encrypted_data</b>. Initial value set as length of buffer, later updated as the actual length of encrypted data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the encrypted data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_encrypt(
    optiga_crypt_t *me,
    optiga_symmetric_encryption_mode_t encryption_mode,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *plain_data,
    uint32_t plain_data_length,
    const uint8_t *iv,
    uint16_t iv_length,
    const uint8_t *associated_data,
    uint16_t associated_data_length,
    uint8_t *encrypted_data,
    uint32_t *encrypted_data_length
);

/**
 * \brief Encrypt the data using symmetric encryption scheme using ECB mode of operation.<br>
 *
 * \details
 * Encrypt the data using symmetric encryption scheme using ECB mode of operation.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.<br>
 * - Encrypts the input message in ecb mode.
 * - Use the symmetric key referred by symmetric key OID.
 * - Exports the encrypted message back to host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_COMMAND_PROTECTION.
 * - No internal padding is performed by OPTIGA hence plain data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         symmetric_key_oid                     OID of the symmetric key object to be used to encrypt the data.
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         plain_data                            Pointer to the data to be encrypted.
 * \param[in]         plain_data_length                     Length of the data to be encrypted.
 * \param[in,out]     encrypted_data                        Pointer to buffer to store encrypted data.
 * \param[in,out]     encrypted_data_length                 Pointer to length of the <b>encrypted_data</b>. Initial value set as length of buffer, later updated as the actual length of encrypted data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the encrypted data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt_ecb.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_encrypt_ecb(
    optiga_crypt_t *me,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *plain_data,
    uint32_t plain_data_length,
    uint8_t *encrypted_data,
    uint32_t *encrypted_data_length
);

/**
 * \brief Initiates a symmetric encryption sequence for input data using symmetric key from OPTIGA.<br>
 *
 * \details
 * Initiates a symmetric encryption sequence for input data using symmetric key from OPTIGA.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.<br>
 * - It marks the beginning of a strict sequence. None of the service layer API will be processed until the strict sequence is terminated.
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments to generate and export the encrypted message to host.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_COMMAND_PROTECTION.
 * - No internal padding is performed by OPTIGA, hence plain data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated
 *   - In case of an error from lower layer.<br>
 *   - Same instance is used for other service layer APIs (except #optiga_crypt_symmetric_encrypt_continue and #optiga_crypt_symmetric_encrypt_final).<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_mode                       Symmetric encryption mode
 * \param[in]         symmetric_key_oid                     OPTIGA symmetric key OID
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         plain_data                            Pointer to the data to be encrypted.
 * \param[in]         plain_data_length                     Length of the data to be encrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in]         iv                                    Pointer to an IV(initialization vector) or nonce.
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         iv_length                             Length of the IV
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         associated_data                       Pointer to associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         associated_data_length                Length of associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         total_plain_data_length               Length of total data to be encrypted until #optiga_crypt_symmetric_encrypt_final.
 *                                                          - Only supported for CCM mode.
 * \param[in,out]     encrypted_data                        Pointer to buffer to store encrypted data. Can be NULL for MAC based operations.
 * \param[in,out]     encrypted_data_length                 Pointer to length of the <b>encrypted_data</b>. Initial value set as length of buffer, later updated as the actual length of encrypted data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the encrypted data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_encrypt_start(
    optiga_crypt_t *me,
    optiga_symmetric_encryption_mode_t encryption_mode,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *plain_data,
    uint32_t plain_data_length,
    const uint8_t *iv,
    uint16_t iv_length,
    const uint8_t *associated_data,
    uint16_t associated_data_length,
    uint16_t total_plain_data_length,
    uint8_t *encrypted_data,
    uint32_t *encrypted_data_length
);

/**
 * \brief Encrypts input data using symmetric key from OPTIGA and exports block aligned encrypted data.<br>
 *
 * \details
 * Encrypts input data using symmetric key from OPTIGA and exports block aligned encrypted data.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.<br>
 * - Encrypts the input message and exports the encrypted message.<br>
 * - Encryption mode and Symmetric key OID specified in #optiga_crypt_symmetric_encrypt_start is used to encrypt the data by OPTIGA.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.<br>
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The encryption sequence must have been initiated, by invoking #optiga_crypt_symmetric_encrypt_start, before calling this API.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_symmetric_encrypt_start is used in this API.<br>
 * - No internal padding is performed by OPTIGA, hence plain data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_symmetric_encrypt_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         plain_data                            Pointer to the data to be encrypted.
 * \param[in]         plain_data_length                     Length of the data to be encrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in,out]     encrypted_data                        Pointer to buffer to store encrypted data. Can be NULL for MAC based operations.
 * \param[in,out]     encrypted_data_length                 Pointer to length of the <b>encrypted_data</b>. Initial value set as length of buffer, later updated as the actual length of encrypted data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the encrypted data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_encrypt_continue(
    optiga_crypt_t *me,
    const uint8_t *plain_data,
    uint32_t plain_data_length,
    uint8_t *encrypted_data,
    uint32_t *encrypted_data_length
);

/**
 * \brief Encrypts input data using symmetric key from OPTIGA, exports block aligned encrypted data and completes the encryption sequence.<br>
 *
 * \details
 * Encrypts input data using symmetric key from OPTIGA, exports block aligned encrypted data and completes the encryption sequence.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Encrypts the input message and exports the encrypted message.<br>
 * - Encryption mode and Symmetric key OID specified in #optiga_crypt_symmetric_encrypt_start is used to encrypt the data by OPTIGA.<br>
 * - It terminates the strict sequence.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The encryption sequence must have been initiated, by invoking #optiga_crypt_symmetric_encrypt_start, before calling this API.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_symmetric_encrypt_start is used in this API.<br>
 * - No internal padding is performed by OPTIGA, hence plain data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_symmetric_encrypt_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         plain_data                            Pointer to the data to be encrypted.
 * \param[in]         plain_data_length                     Length of the data to be encrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in,out]     encrypted_data                        Pointer to buffer to store encrypted data.
 * \param[in,out]     encrypted_data_length                 Pointer to length of the <b>encrypted_data</b>. Initial value set as length of buffer, later updated as the actual length of encrypted data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the encrypted data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_encrypt_final(
    optiga_crypt_t *me,
    const uint8_t *plain_data,
    uint32_t plain_data_length,
    uint8_t *encrypted_data,
    uint32_t *encrypted_data_length
);
#endif  // OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED

#ifdef OPTIGA_CRYPT_SYM_DECRYPT_ENABLED
/**
 * \brief Decrypt the encrypted data using symmetric encryption mode and export plain message to host.<br>
 *
 * \details
 * Decrypt the encrypted data using symmetric key from OPTIGA and export the plain data to host.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - Decrypts the input message based on the encryption mode.
 * - Use the symmetric key referred by symmetric key OID.
 * - Exports the decrypted message back to host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - The API does not support MAC based encryption modes.<br>
 * - No internal padding is performed by OPTIGA, hence encrypted data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_mode                       Symmetric encryption mode
 * \param[in]         symmetric_key_oid                     OPTIGA symmetric key OID
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         encrypted_data                        Pointer to the data to be decrypted.
 * \param[in]         encrypted_data_length                 Length of the data to be decrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in]         iv                                    Pointer to an IV(initialization vector) or nonce.
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         iv_length                             Length of the IV
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         associated_data                       Pointer to associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         associated_data_length                Length of associated data
 *                                                          - Only supported for CBC mode.
 * \param[in,out]     plain_data                            Pointer to buffer to store plain data.
 * \param[in,out]     plain_data_length                     Pointer to length of the <b>plain_data</b>. Initial value set as length of buffer, later updated as the actual length of plain data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the plain data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_decrypt(
    optiga_crypt_t *me,
    optiga_symmetric_encryption_mode_t encryption_mode,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *encrypted_data,
    uint32_t encrypted_data_length,
    const uint8_t *iv,
    uint16_t iv_length,
    const uint8_t *associated_data,
    uint16_t associated_data_length,
    uint8_t *plain_data,
    uint32_t *plain_data_length
);
/**
 * \brief Decrypt the data using symmetric encryption scheme using ECB mode of operation.<br>
 *
 * \details
 * Decrypt the data using symmetric encryption scheme using ECB mode of operation.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - Decrypts the input message based on ecb mode.
 * - Use the symmetric key referred by symmetric key OID.
 * - Exports the decrypted message back to host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - No internal padding is performed by OPTIGA, hence encrypted data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         symmetric_key_oid                     OID of the symmetric key object to be used to decrypt the data.
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         encrypted_data                        Pointer to the encrypted data to be decrypted.
 * \param[in]         encrypted_data_length                 Length of the encrypted data to be decrypted.
 * \param[in,out]     plain_data                            Pointer to buffer to store decrypted data.
 * \param[in,out]     plain_data_length                     Pointer to length of the <b>plain_data</b>. Initial value set as length of buffer, later updated as the actual length of plain data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the plain data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt_ecb.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_decrypt_ecb(
    optiga_crypt_t *me,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *encrypted_data,
    uint32_t encrypted_data_length,
    uint8_t *plain_data,
    uint32_t *plain_data_length
);

/**
 * \brief Initiate symmetric decryption sequence for input data using symmetric key from OPTIGA.<br>
 *
 * \details
 * Initiate symmetric decryption sequence for input data using symmetric key from OPTIGA.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - It marks the beginning of a strict sequence. None of the service layer API will be processed until the strict sequence is terminated.
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments to generate and export the plain message to host.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - The API does not support MAC based encryption modes.<br>
 * - No internal padding is performed by OPTIGA, hence encrypted data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated
 *   - In case of an error from lower layer.<br>
 *   - Same instance is used for other service layer APIs (except #optiga_crypt_symmetric_decrypt_continue and #optiga_crypt_symmetric_decrypt_final).<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encryption_mode                       Symmetric encryption mode
 * \param[in]         symmetric_key_oid                     OPTIGA symmetric key OID
 *                                                          - Symmetric key must be available at the specified OID.<br>
 * \param[in]         encrypted_data                        Pointer to the data to be decrypted.
 * \param[in]         encrypted_data_length                 Length of the data to be decrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in]         iv                                    Pointer to an IV(initialization vector) or nonce.
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         iv_length                             Length of the IV
 *                                                          - Only supported for CBC and CCM mode.
 * \param[in]         associated_data                       Pointer to associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         associated_data_length                Length of associated data
 *                                                          - Only supported for CCM mode.
 * \param[in]         total_encrypted_data_length           Length of total data to be decrypted until #optiga_crypt_symmetric_decrypt_final.
 *                                                          - Only supported for CCM mode.
 * \param[in,out]     plain_data                            Pointer to buffer to store plain data.
 * \param[in,out]     plain_data_length                     Pointer to length of the <b>plain_data</b>. Initial value set as length of buffer, later updated as the actual length of plain data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the plain data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_decrypt_start(
    optiga_crypt_t *me,
    optiga_symmetric_encryption_mode_t encryption_mode,
    optiga_key_id_t symmetric_key_oid,
    const uint8_t *encrypted_data,
    uint32_t encrypted_data_length,
    const uint8_t *iv,
    uint16_t iv_length,
    const uint8_t *associated_data,
    uint16_t associated_data_length,
    uint16_t total_encrypted_data_length,
    uint8_t *plain_data,
    uint32_t *plain_data_length
);

/**
 * \brief Decrypts input encrypted data using symmetric key from OPTIGA and exports block aligned plain data.<br>
 *
 * \details
 * Decrypts input encrypted data using symmetric key from OPTIGA and exports block aligned plain data.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - Decrypts the input message and exports the decrypted message.<br>
 * - Encryption mode and the Symmetric key OID specified in #optiga_crypt_symmetric_decrypt_start is used to decrypt the data by OPTIGA.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The decryption sequence must have been initiated, by invoking #optiga_crypt_symmetric_decrypt_start, before calling this API.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_symmetric_decrypt_start is used in this API.<br>
 * - No internal padding is performed by OPTIGA, hence encrypted data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_symmetric_decrypt_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encrypted_data                        Pointer to the data to be decrypted.
 * \param[in]         encrypted_data_length                 Length of the data to be decrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in,out]     plain_data                            Pointer to buffer to store plain data.
 * \param[in,out]     plain_data_length                     Pointer to length of the <b>plain_data</b>. Initial value set as length of buffer, later updated as the actual length of plain data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the plain data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_decrypt_continue(
    optiga_crypt_t *me,
    const uint8_t *encrypted_data,
    uint32_t encrypted_data_length,
    uint8_t *plain_data,
    uint32_t *plain_data_length
);

/**
 * \brief Decrypts input data using symmetric key from OPTIGA, exports block aligned plain data and completes the decryption sequence.<br>
 *
 * \details
 * Decrypts input data using symmetric key from OPTIGA, exports block aligned plain data and completes the decryption sequence.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments to generate and export the plain message to host.<br>
 * - Decrypts the input message and exports the decrypted message.<br>
 * - Encryption mode and the symmetric key OID specified in #optiga_crypt_symmetric_decrypt_start is used to decrypt the data by OPTIGA.<br>
 * - It terminates the strict sequence.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The decryption sequence must have been initiated, by invoking #optiga_crypt_symmetric_decrypt_start, before calling this API.<br>
 * - Symmetric key must be available at symmetric key OID in OPTIGA.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_symmetric_decrypt_start is used in this API.<br>
 * - No internal padding is performed by OPTIGA, hence encrypted data length must be block aligned (minimum 1 block).<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_symmetric_decrypt_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         encrypted_data                        Pointer to the data to be decrypted.
 * \param[in]         encrypted_data_length                 Length of the data to be decrypted.
 *                                                          - It must be block aligned, otherwise OPTIGA returns an error.<br>
 * \param[in,out]     plain_data                            Pointer to buffer to store plain data.
 * \param[in,out]     plain_data_length                     Pointer to length of the <b>plain_data</b>. Initial value set as length of buffer, later updated as the actual length of plain data.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the plain data.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_encrypt_decrypt.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_decrypt_final(
    optiga_crypt_t *me,
    const uint8_t *encrypted_data,
    uint32_t encrypted_data_length,
    uint8_t *plain_data,
    uint32_t *plain_data_length
);
#endif  // OPTIGA_CRYPT_SYM_DECRYPT_ENABLED

#ifdef OPTIGA_CRYPT_HMAC_ENABLED
/**
 * \brief Generates HMAC on the input message using input secret from OPTIGA and exports the generated HMAC to the host.<br>
 *
 * \details
 * Generates HMAC on the input message using input secret from OPTIGA and exports the generated HMAC to host.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Generates HMAC on the input data and exports it to host.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_COMMAND_PROTECTION.
 * - Error codes from lower layers is returned as it is to the application.<br>

 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         type                                  HMAC type
 * \param[in]         secret                                OPTIGA OID with input secret
 *                                                          - Input secret must be available at the specified OID.<br>
 *                                                          - To indicate session OID (already acquired by instance), specify #OPTIGA_KEY_ID_SESSION_BASED
 * \param[in]         input_data                            Pointer to input data for HMAC generation.
 * \param[in]         input_data_length                     Length of input data for HMAC generation.
 *                                                          - Must be greater than 0.<br>
 * \param[in,out]     mac                                   Pointer to buffer to store generated HMAC
 * \param[in,out]     mac_length                            Pointer to length of the <b>mac</b>. Initial value set as length of buffer, later updated as the actual length of generated HMAC.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the HMAC.
 *                                                          - If the <b>mac_length</b> is lesser than the length of MAC received from OPTIGA, then first <b>mac_length</b> bytes are only returned.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hmac.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hmac(
    optiga_crypt_t *me,
    optiga_hmac_type_t type,
    uint16_t secret,
    const uint8_t *input_data,
    uint32_t input_data_length,
    uint8_t *mac,
    uint32_t *mac_length
);

/**
 * \brief Initiates a HMAC generation sequence for the input data using input secret from OPTIGA.<br>
 *
 * \details
 * Initiates a HMAC generation sequence for the input data.<br>
 * - It marks the beginning of a strict sequence. None of the service layer API will be processed until the strict sequence is terminated.
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Generates HMAC on the input data but does not export it to host.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_COMMAND_PROTECTION.<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated
 *   - In case of an error from lower layer.<br>
 *   - Same instance is used for other service layer APIs (except #optiga_crypt_hmac_update and #optiga_crypt_hmac_finalize).<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         type                                  HMAC type
 * \param[in]         secret                                OPTIGA OID with input secret
 *                                                          - Input secret must be available at the specified OID.<br>
 *                                                          - To indicate session OID (already acquired by instance), specify #OPTIGA_KEY_ID_SESSION_BASED
 * \param[in]         input_data                            Pointer to input data for HMAC generation.
 * \param[in]         input_data_length                     Length of input data for HMAC generation.
 *                                                          - Must be greater than 0.<br>
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hmac.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hmac_start(
    optiga_crypt_t *me,
    optiga_hmac_type_t type,
    uint16_t secret,
    const uint8_t *input_data,
    uint32_t input_data_length
);

/**
 * \brief Generates HMAC on the input message using input secret from OPTIGA, update the previously generated HMAC value internally.<br>
 *
 * \details
 * Generates HMAC on the input message using input secret from OPTIGA and updates the previously generated HMAC value internally.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Generates HMAC on the input data but does not export it to host.
 * - Input secret specified in #optiga_crypt_hmac_start is used to generate HMAC.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The HMAC sequence must have been initiated, by invoking #optiga_crypt_hmac_start, before calling this API.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_hmac_start is used in this API.<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_hmac_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         input_data                            Pointer to input data for HMAC generation.
 * \param[in]         input_data_length                     Length of input data for HMAC generation.
 *                                                          - Must be greater than 0.<br>
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hmac.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_crypt_hmac_update(optiga_crypt_t *me, const uint8_t *input_data, uint32_t input_data_length);

/**
 * \brief Generates HMAC on the input message using input secret from OPTIGA and exports the finalized HMAC to host.<br>
 *
 * \details
 * Generates HMAC on the input message using input secret from OPTIGA and exports the finalized HMAC to host.<br>
 * - Invokes #optiga_cmd_encrypt_sym API, based on the input arguments.
 * - Generates HMAC on the input data and exports it to host.
 * - Input secret specified in #optiga_crypt_hmac_start is used to generate HMAC.<br>
 * - It terminates the strict sequence.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The HMAC sequence must have been initiated, by invoking #optiga_crypt_hmac_start, before calling this API.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, value set in #optiga_crypt_hmac_start is used in this API.<br>
 * - Error codes from lower layers is returned as it is to the application.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 * - Invoking this API without successful completion of #optiga_crypt_hmac_start throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         input_data                            Pointer to input data for HMAC generation.
 * \param[in]         input_data_length                     Length of input data for HMAC generation.
 *                                                          - Must be greater than 0.<br>
 * \param[in,out]     mac                                   Pointer to buffer to store generated HMAC
 * \param[in,out]     mac_length                            Pointer to length of the <b>mac</b>. Initial value set as length of buffer, later updated as the actual length of generated HMAC.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the HMAC.
 *                                                          - If the <b>mac_length</b> is lesser than the length of MAC received from OPTIGA, then first <b>mac_length</b> bytes are only returned.
 *                                                          - In case of any error, the value is set to 0.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hmac.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hmac_finalize(
    optiga_crypt_t *me,
    const uint8_t *input_data,
    uint32_t input_data_length,
    uint8_t *mac,
    uint32_t *mac_length
);
#endif  // OPTIGA_CRYPT_HMAC_ENABLED

#ifdef OPTIGA_CRYPT_HKDF_ENABLED
/**
 * \brief Derives a key or shared secret using HKDF operation from the secret stored in OPTIGA.<br>
 *
 * \details
 * Derives a key or shared secret using HKDF operation from the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - The derived key is either stored in a OPTIGA session context or exported back to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 * - If the secret type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecdh )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         type                                     HKDF scheme as #optiga_hkdf_type_t.
 * \param[in]         secret                                   Object ID of the secret stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         salt                                     Pointer to buffer containing salt value, can be NULL if not applicable.
 * \param[in]         salt_length                              Length of salt.
 * \param[in]         info                                     Pointer to buffer containing application specific information, can be NULL if not applicable.
 * \param[in]         info_length                              Length of info.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
                                                               Session is not available in instance and the secret type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hkdf.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hkdf(
    optiga_crypt_t *me,
    optiga_hkdf_type_t type,
    uint16_t secret,
    const uint8_t *salt,
    uint16_t salt_length,
    const uint8_t *info,
    uint16_t info_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
);

/**
 * \brief Derives a key or shared secret using HKDF-SHA256 operation from the secret stored in OPTIGA.<br>
 *
 * \details
 * Derives a key or shared secret using HKDF-SHA256 operation from the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - The derived key is either stored in a OPTIGA session context or exported back to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 * - If the secret type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecdh )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID of the secret stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         salt                                     Pointer to buffer containing salt value, can be NULL if not applicable.
 * \param[in]         salt_length                              Length of salt.
 * \param[in]         info                                     Pointer to buffer containing application specific information, can be NULL if not applicable.
 * \param[in]         info_length                              Length of info.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
                                                               Session is not available in instance and the secret type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hkdf.c
 *
 */
_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hkdf_sha256(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *salt,
    uint16_t salt_length,
    const uint8_t *info,
    uint16_t info_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_hkdf(
        me,
        OPTIGA_HKDF_SHA_256,
        secret,
        salt,
        salt_length,
        info,
        info_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}

/**
 * \brief Derives a key or shared secret using HKDF-SHA384 operation from the secret stored in OPTIGA.<br>
 *
 * \details
 * Derives a key or shared secret using HKDF-SHA384 operation from the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - The derived key is either stored in a OPTIGA session context or exported back to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 * - If the secret type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecdh )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID of the secret stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         salt                                     Pointer to buffer containing salt value, can be NULL if not applicable.
 * \param[in]         salt_length                              Length of salt.
 * \param[in]         info                                     Pointer to buffer containing application specific information, can be NULL if not applicable.
 * \param[in]         info_length                              Length of info.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
                                                               Session is not available in instance and the secret type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * None
 *
 */

_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hkdf_sha384(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *salt,
    uint16_t salt_length,
    const uint8_t *info,
    uint16_t info_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_hkdf(
        me,
        OPTIGA_HKDF_SHA_384,
        secret,
        salt,
        salt_length,
        info,
        info_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}

/**
 * \brief Derives a key or shared secret using HKDF-SHA512 operation from the secret stored in OPTIGA.<br>
 *
 * \details
 * Derives a key or shared secret using HKDF-SHA384 operation from the secret stored in OPTIGA.
 * - Invokes #optiga_cmd_derive_key API, based on the input arguments.<br>
 * - The derived key is either stored in a OPTIGA session context or exported back to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 * - If the secret type is #OPTIGA_KEY_ID_SESSION_BASED then session must be already available in the instance. (For .e.g Using #optiga_crypt_ecdh )
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION if secret is in session OID.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID of the secret stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         salt                                     Pointer to buffer containing salt value, can be NULL if not applicable.
 * \param[in]         salt_length                              Length of salt.
 * \param[in]         info                                     Pointer to buffer containing application specific information, can be NULL if not applicable.
 * \param[in]         info_length                              Length of info.
 * \param[in]         derived_key_length                       Length of derived key.
 *                                                             - Export to Host : The minimum length of the derived key can be 1 byte.
 *                                                             - Save in Sesssion OID : The minimum length of the derived key should be 16 bytes. <br>
 *                                                             For derived key lengths from 1 to 15, OPTIGA derives 16 bytes key in the session OID.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.<br>
                                                               Session is not available in instance and the secret type is #OPTIGA_KEY_ID_SESSION_BASED
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * None
 *
 */

_STATIC_INLINE LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hkdf_sha512(
    optiga_crypt_t *me,
    uint16_t secret,
    const uint8_t *salt,
    uint16_t salt_length,
    const uint8_t *info,
    uint16_t info_length,
    uint16_t derived_key_length,
    bool_t export_to_host,
    uint8_t *derived_key
) {
    return (optiga_crypt_hkdf(
        me,
        OPTIGA_HKDF_SHA_512,
        secret,
        salt,
        salt_length,
        info,
        info_length,
        derived_key_length,
        export_to_host,
        derived_key
    ));
}
#endif  // OPTIGA_CRYPT_HKDF_ENABLED

#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED

/**
 * \brief Generates a symmetric key using OPTIGA.
 *
 * \details
 * Generates a symmetric key using OPTIGA for different key sizes.
 * - Invokes #optiga_cmd_gen_symkey API, based on input arguments.
 * - Symmetric key is generated based on the key type.<br>
 * - If export to host is requested, exports the symmetric key back to host otherwise stores it in the provided symmetric key OID.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 *
 * \param[in]       me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]       key_type                                Key type of #optiga_symmetric_key_type_t.
 * \param[in]       key_usage                               Key usage defined in #optiga_key_usage_t.
 *                                                          - It is ignored if export_symmetric_key is TRUE (1) or Non-Zero.
 * \param[in]       export_symmetric_key                    TRUE (1) or Non-Zero value - Exports symmetric key to the host.<br>
 *                                                          FALSE (0) - Stores symmetric key in OPTIGA.
 * \param[in,out]   symmetric_key                           Pointer to buffer of symmetric key.
 *                                                          - If export_symmetric_key is TRUE or non-zero, assign pointer to a buffer to store symmetric key.
 *                                                          - If export_symmetric_key is FALSE, assign pointer to variable of type #optiga_key_id_t.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the key.
 *
 * \retval          #OPTIGA_CRYPT_SUCCESS                   Successful invocation.
 * \retval          #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval          #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 * \retval          #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_symmetric_generate_key.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_symmetric_generate_key(
    optiga_crypt_t *me,
    optiga_symmetric_key_type_t key_type,
    uint8_t key_usage,
    bool_t export_symmetric_key,
    void *symmetric_key
);
#endif  // OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED

#ifdef OPTIGA_CRYPT_GENERATE_AUTH_CODE_ENABLED

/**
 * \brief Generates a random number using OPTIGA and stores the same in acquired session context at OPTIGA.
 *
 * \details
 * Generates random stream code for user provided length.
 * - Invokes #optiga_cmd_get_random API, based on the input arguments to retrieve random data <br>
 * - Generated random is returned to host .
 * - Generated random and optional data is also stored in the session context acquired at OPTIGA.
 * - Data stored in the acquired session context acquired at OPTIGA is used as an auth code.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers is returned as it is.
 * - Minimum length of generated random data is 8 bytes.
 * - Auth Code is a concatenation of optional data and random data.
 * - Maximum length of auth code is 48 bytes.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      rng_type                                 Type of random data generator.
 *                                                          - The input must be from #optiga_rng_type.
 *                                                          - Argument check for rng_type is not done since OPTIGA will provide an error for invalid rng_type.
 * \param[in]      optional_data                            Optional data that gets prepended to the generated random secret.
 *                                                          - Can be set to NULL, if not required
 * \param[in]      optional_data_length                     Length of the optional data provided. It is ignored if optional_data is NULL
 *                                                          - Value can be up to 58 bytes
 *                                                          - Difference in random data length and optional data length is less than 8 bytes
 * \param[in,out]  random_data                              Pointer to the buffer into which random data is stored, must not be NULL.
 * \param[in]      random_data_length                       Length of random data to be generated.
 *                                                          - Minimum range is 8 bytes.
 *
 * \retval          #OPTIGA_CRYPT_SUCCESS                   Successful invocation.
 * \retval          #OPTIGA_CRYPT_ERROR_INVALID_INPUT       Wrong Input arguments provided.
 * \retval          #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete.
 * \retval          #OPTIGA_DEVICE_ERROR                    Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_generate_auth_code(
    optiga_crypt_t *me,
    optiga_rng_type_t rng_type,
    const uint8_t *optional_data,
    uint16_t optional_data_length,
    uint8_t *random_data,
    uint16_t random_data_length
);
#endif  // OPTIGA_CRYPT_GENERATE_AUTH_CODE_ENABLED

#ifdef OPTIGA_CRYPT_HMAC_VERIFY_ENABLED
/**
 * \brief Performs the HMAC verification for the provided authorization value using OPTIGA.<br>
 *
 * \details
 * Verifies HMAC generated at host with HMAC generated from OPTIGA for the provided authorization value.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - Acquired session gets released after completion of hmac verify operation between host and OPTIGA.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - The data object specified by input <b>secret</b> must have a secret written into it.<br>
 * - Session has already been acquired by #optiga_crypt_generate_auth_code.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - Error codes from lower layers is returned as it is to the application.<br>
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         type                                  HMAC type as #optiga_hmac_type_t
 * \param[in]         secret                                OPTIGA OID with input secret
 *                                                          - Input secret must be available at the specified OID.<br>
 *                                                          - Data object type must be AUTOREF.
 * \param[in]         input_data                            Pointer to input data for HMAC generation.
 *                                                          - Input data is concatenation of optional data, random and arbitrary data.
 *                                                          - Optional data is input provided to #optiga_crypt_generate_auth_code
 *                                                            and random data is returned by #optiga_crypt_generate_auth_code.
 * \param[in]         input_data_length                     Length of input data.
 *                                                          - Must be greater than 0.<br>
 * \param[in]         hmac                                  Pointer to buffer of HMAC generated at host which gets verified at OPTIGA.
 * \param[in]         hmac_length                           Length of the generated <b>hmac</b> at host.
 *                                                          - Host generated hmac length must be the size of output defined in respective hash algorithm used in hmac scheme.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_hmac_verify.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_crypt_hmac_verify(
    optiga_crypt_t *me,
    optiga_hmac_type_t type,
    uint16_t secret,
    const uint8_t *input_data,
    uint32_t input_data_length,
    const uint8_t *hmac,
    uint32_t hmac_length
);
#endif  // OPTIGA_CRYPT_HMAC_VERIFY_ENABLED
#ifdef OPTIGA_CRYPT_CLEAR_AUTO_STATE_ENABLED
/**
 * \brief This operation clears the AUTO state at OPTIGA for input secret OID.<br>
 *
 * \details
 * Clears the AUTO state and session if it's already acquired.<br>
 * - Invokes #optiga_cmd_decrypt_sym API, based on the input arguments.<br>
 * - Acquired session is released after completion of clear auto state operation.<br>
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is #OPTIGA_COMMS_RESPONSE_PROTECTION.
 *
 * \param[in]         me                                    Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                OPTIGA OID with input secret. It should be of AUTOREF type

 * \retval            #OPTIGA_CRYPT_SUCCESS                 Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT     Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE   The previous operation with the same instance is not complete
 *
 * <b>Example</b><br>
 * example_optiga_crypt_clear_auto_state.c
 *
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_crypt_clear_auto_state(optiga_crypt_t *me, uint16_t secret);

#endif  // OPTIGA_CRYPT_CLEAR_AUTO_STATE_ENABLED
/**
 * \brief Enables the protected I2C communication with OPTIGA for CRYPT instances
 *
 * \details
 * Enables the protected I2C communication with OPTIGA
 * - Sets the protection mode for the supplied instance.<br>
 * - Call this function before calling the service layer API which requires protection.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - #OPTIGA_COMMS_SHIELDED_CONNECTION macro must be defined.<br>
 * - #OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION function must be called once to set the required protocol version
 *   - Currently only the Pre-shared Secret based version is supported.
 * - The host and OPTIGA must be paired and Pre-Shared secret is available.<br>
 *
 * \note
 * - The protection mode for the instance is reset to #OPTIGA_COMMS_NO_PROTECTION once the service layer API returns synchronously.
 *
 * \param[in]      p_instance           Valid pointer to an instance
 * \param[in]      protection_level     Required protection mode
 *                                      - #OPTIGA_COMMS_NO_PROTECTION : Command and response is unprotected
 *                                      - #OPTIGA_COMMS_COMMAND_PROTECTION : Command is protected and response is unprotected
 *                                      - #OPTIGA_COMMS_RESPONSE_PROTECTION : Command is unprotected and response is protected
 *                                      - #OPTIGA_COMMS_FULL_PROTECTION : Both command and response is protected
 *                                      - To re-establish secure channel, bitwise-OR protection_level with #OPTIGA_COMMS_RE_ESTABLISH
 */
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
#define OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    { optiga_crypt_set_comms_params(p_instance, OPTIGA_COMMS_PROTECTION_LEVEL, protection_level); }
#else
#define OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    {}
#endif

/**
 * \brief Select the protocol version required for the I2C protected communication for CRYPT instances
 *
 * \details
 * Select the protocol version required for the I2C protected communication
 * - Select the protocol version for the supplied instance.<br>
 *
 * \pre
 * - #OPTIGA_COMMS_SHIELDED_CONNECTION macro must be defined.<br>
 *
 * \note
 * - None
 *
 * \param[in]      p_instance    Valid pointer to an instance
 * \param[in]      version       Required protocol version
 *                               - #OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET : Pre-shared Secret based protocol version
 */
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
#define OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    { optiga_crypt_set_comms_params(p_instance, OPTIGA_COMMS_PROTOCOL_VERSION, version); }
#else
#define OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    {}
#endif

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_CRYPT_H_*/

/**
 * @}
 */
