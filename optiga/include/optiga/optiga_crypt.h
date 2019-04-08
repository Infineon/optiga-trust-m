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

#include "optiga/cmd/optiga_cmd.h"

/** \brief OPTIGA crypt instance structure */
struct optiga_crypt
{
    /// Extra buffer to hold the details/references (pointers) to the Application Inputs
    uint8_t params [80];
    /// Command module instance
    optiga_cmd_t * my_cmd;
    /// Caller context
    void * caller_context;
    /// Callback handler
    callback_handler_t handler;
    ///To provide the busy/free status of the crypt instance
    uint16_t instance_state;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide the encryption and decryption need for command and response
    uint8_t protection_level;
    /// To provide the presentation layer protocol version to be used
    uint8_t protocol_version;
#endif //OPTIGA_COMMS_SHIELDED_CONNECTION

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
 * - The #OPTIGA_COMMS_PROTECTION_LEVEL configuration settings using this API, will be used in the next immedidate usage of the instance.
 * - Once the API is invoked, this level gets reset to the default protection level #OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL
 *
 *\pre
 * - None
 *
 *\note
 * - None
 *
 * \param[in,out]  me                     Valid instance of #optiga_util_t
 * \param[in]      configuration_type     Configuration Type
 *                                        Possible Types are
 *                                        #OPTIGA_COMMS_PROTECTION_LEVEL
 *                                        #OPTIGA_COMMS_PROTOCOL_VERSION
 *
 * \param[in]      value                  Value part for the respective configuration
 *
 * <b>Example</b><br>
 *
 */
void optiga_crypt_set_comms_params(optiga_crypt_t * me,
                                   uint8_t configuration_type,
                                   uint8_t value);
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
 *  - This API is implemented in synchronous mode.
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
optiga_crypt_t * optiga_crypt_create(uint8_t optiga_instance_id,
                                     callback_handler_t handler,
                                     void * caller_context);

/**
 * \brief Destroys an instance of #optiga_crypt_t.
 *
 * \details
 * Destroys the #optiga_crypt_t instance.
 * - De-allocate the memory of the #optiga_crypt_t instance.
 *
 * \pre
 * - An instance of optiga_crypt using #optiga_crypt_create must be available.
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
optiga_lib_status_t optiga_crypt_destroy(optiga_crypt_t * me);

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
 * - Error codes from lower layers will be returned as it is.<br>
 * - The maximum value of the <b>random_data_length</b> parameter is size of buffer <b>random_data</b>.
 *   In case the value is greater than buffer size, memory corruption can occur.
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
optiga_lib_status_t optiga_crypt_random(optiga_crypt_t * me,
                                        optiga_rng_type_t rng_type,
                                        uint8_t * random_data,
                                        uint16_t random_data_length);
#endif //OPTIGA_CRYPT_RANDOM_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED

 /**
 *
 * \brief Initializes a hash context.
 *
 * \details
 * Sets up a hash context and exports it.
 * - Initializes a new hash context.<br>
 * - Exports the hash context to caller.<br>
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layer will be returned as it is.<br>
 * - User must save the output hash context for further usage as OPTIGA does not store it internally.<br>
 *
 *<br>
 * \param[in]      me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[inout]   hash_ctx                                  Pointer to #optiga_hash_context_t to store the hash context from OPTIGA.
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
optiga_lib_status_t optiga_crypt_hash_start(optiga_crypt_t * me,
                                            optiga_hash_context_t * hash_ctx);


 /**
 * \brief Updates a hash context with the input data.
 *
 * \details
 * Updates hashing for the given data and hash context then export the updated hash context.<br>
 * - Update the input hash context.
 * - Exports the hash context to caller.
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
optiga_lib_status_t optiga_crypt_hash_update(optiga_crypt_t * me,
                                             optiga_hash_context_t * hash_ctx,
                                             uint8_t source_of_data_to_hash,
                                             const void * data_to_hash);

 /**
 *
 * \brief Finalizes and exports the hash output.
 *
 * \details
 * Finalizes the hash context and returns hash as output.
 * - Finalize the hash from the input hash context
 * - Exports the finalized hash.
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
optiga_lib_status_t optiga_crypt_hash_finalize(optiga_crypt_t * me,
                                               optiga_hash_context_t * hash_ctx,
                                               uint8_t * hash_output);
#endif //OPTIGA_CRYPT_HASH_ENABLED


#ifdef OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

/**
 * \brief Generates an key pair based on ECC curves.
 *
 * \details
 * Generates an ECC key-pair based on the type of the key.
 * - Generate an ECC key pair using OPTIGA.
 * - If export is requested, Exports the private key else stores it in the input private key OID.
 * - Always exports the public key.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 *
 * \param[in]       me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]       curve_id                                ECC curve id.
 * \param[in]       key_usage                               Key usage defined by #optiga_key_usage_t.
 *                                                          - Values from #optiga_key_usage can be logically ORed and passed.<br>
 *                                                          - It is ignored if export_private_key is TRUE (1).
 * \param[in]       export_private_key                      TRUE (1) or Non-Zero value - Exports private key to the host.<br>
 *                                                          FALSE (0) - Exports only public key to the host and writes private key to OPTIGA. The input key_usage is ignored.
 * \param[in]       private_key                             Buffer to store private key or private key OID of OPTIGA, must not be NULL.
 *                                                          - If export_private_key is TRUE, assign pointer to a buffer to store private key.
 *                                                          - The size of the buffer must be sufficient enough to accommodate the key type and additional DER encoding formats.
 *                                                          - If export_private_key is FALSE, assign pointer to variable of type #optiga_key_id_t.
 * \param[in,out]   public_key                              Buffer to store public key, must not be NULL.
 * \param[in]       public_key_length                       Initially set as length of public_key, later updated as actual length of public_key.
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
optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_crypt_t * me,
                                                      optiga_ecc_curve_t curve_id,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length);
#endif //OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_SIGN_ENABLED
 /**
 * \brief Generates a signature for the given digest.
 *
 * \details
 * Generates a signature for the given digest using private key stored in OPTIGA.
 * - Generates signature for the input digest.
 * - Exports the generated signature.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 * - Try to access session OID without acquiring session in #optiga_crypt_ecc_generate_keypair throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.
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
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided.
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete.
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecdsa_sign.c
 *
 */
optiga_lib_status_t optiga_crypt_ecdsa_sign(optiga_crypt_t * me,
                                            const uint8_t * digest,
                                            uint8_t digest_length,
                                            optiga_key_id_t private_key,
                                            uint8_t * signature,
                                            uint16_t * signature_length);
#endif //OPTIGA_CRYPT_ECDSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED
/**
 * \brief Verifies the signature over the given digest.
 *
 * \details
 * Verifies the signature over a given digest provided with the input data.
 * - Verifies the signature over the given provided with the input data using public key.
 * - It invokes the callback handler of the instance, when it is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layers will be returned as it is to the application.<br>
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
optiga_lib_status_t optiga_crypt_ecdsa_verify(optiga_crypt_t * me,
                                              const uint8_t * digest,
                                              uint8_t digest_length,
                                              const uint8_t * signature,
                                              uint16_t signature_length,
                                              uint8_t public_key_source_type,
                                              const void * public_key);
#endif //OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_ECDH_ENABLED
 /**
 * \brief Calculates the shared secret using ECDH algorithm.<br>
 *
 * \details
 * Calculates the shared secret using ECDH algorithm
 * - Calculates the shared secret based on input private key object ID and public key.
 * - Based on user request (export_to_host), the shared secret can either be exported to the host or be stored in the acquired session object ID.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 * - There must be a private key available in the "session context / data object OID" provided as input parameter.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 * - The buffer size for shared secret should be appropriately provided by the user
 * - If the user provides <b>private_key</b> as session based and <b>export_to_host</b> as FALSE,<br>
 *   then the shared secret generated will overwrite the private key stored in the session object ID
 * - Try to access session OID without acquiring session in #optiga_crypt_ecc_generate_keypair throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.
 *
 * \param[in]      me                                          Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      private_key                                 Object ID of the private key stored in OPTIGA.<br>
 *                                                             - Possible values are from the #optiga_key_id_t <br>
 *                                                             - Argument check for private_key is not done since OPTIGA will provide an error for invalid private_key.
 * \param[in]      public_key                                  Pointer to the public key structure for shared secret generation with its properties, must not be NULL.<br>
 *                                                             - Provide the inputs according to the structure type #public_key_from_host_t
 * \param[in]      export_to_host                              TRUE (1) or Non-Zero value - Exports the generated shared secret to Host. <br>
 *                                                             FALSE (0) - Stores the generated shared secret into the session object ID acquired by the instance.
 * \param[in,out]  shared_secret                               Pointer to the shared secret buffer, only if <b>export_to_host</b> is TRUE. <br>
 *                                                             Otherwise provide NULL as input.
 *
 * \retval         #OPTIGA_CRYPT_SUCCESS                       Successful invocation
 * \retval         #OPTIGA_CRYPT_ERROR_INVALID_INPUT           Wrong Input arguments provided
 * \retval         #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE         The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                        Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_ecdh.c
 */
optiga_lib_status_t optiga_crypt_ecdh(optiga_crypt_t * me,
                                      optiga_key_id_t private_key,
                                      public_key_from_host_t * public_key,
                                      bool_t export_to_host,
                                      uint8_t * shared_secret);
#endif //OPTIGA_CRYPT_ECDH_ENABLED

#ifdef OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED
/**
 * \brief Derives a key.<br>
 *
 * \details
 * Derives a key using the secret stored in OPTIGA.
 * - Provides the options to store the derived key into OPTIGA session context or export to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 * - Try to access session OID without acquiring session in #optiga_crypt_ecdh throws #OPTIGA_CMD_ERROR_INVALID_INPUT error.
 *
 * \note
 * - Exporting the derived key:
 *      - The minimum length of the output derived key can be 1 byte.
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION.
 * - Error codes from lower layers will be returned as it is to the application.<br>
 *
 * \param[in]         me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]         secret                                   Object ID of the secret stored in OPTIGA.
 *                                                             - #OPTIGA_KEY_ID_SESSION_BASED from #optiga_key_id_t, indicates the secret is available
 *                                                               in the session context acquired by the instance.
 *                                                             - or any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]         label                                    Pointer to the label, can be NULL if not applicable.
 * \param[in]         label_length                             Length of the label.
 * \param[in]         seed                                     Valid pointer to the seed, must not be NULL.
 * \param[in]         seed_length                              Length of the seed.
 * \param[in]         derived_key_length                       Length of derived key.
 * \param[in]         export_to_host                           TRUE (1) or Non-Zero value - Exports the derived key to Host. <br>
 *                                                             FALSE (0) - Stores the derived key into the session object ID acquired by the instance.
 * \param[in,out]     derived_key                              Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                                             in case of exporting the key to host(<b>export_to_host is non-zero value</b>). Otherwise set to NULL.
 *
 * \retval            #OPTIGA_CRYPT_SUCCESS                    Successful invocation
 * \retval            #OPTIGA_CRYPT_ERROR_INVALID_INPUT        Wrong Input arguments provided
 * \retval            #OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE      The previous operation with the same instance is not complete
 * \retval            #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                             (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_crypt_tls_prf_sha256.c
 *
 */
optiga_lib_status_t optiga_crypt_tls_prf_sha256(optiga_crypt_t * me,
                                                uint16_t secret,
                                                const uint8_t * label,
                                                uint16_t label_length,
                                                const uint8_t * seed,
                                                uint16_t seed_length,
                                                uint16_t derived_key_length,
                                                bool_t export_to_host,
                                                uint8_t * derived_key);
#endif //OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED

#ifdef OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED

/**
 * \brief Generates an key pair based on RSA key type.
 *
 * \details
 * Generates an RSA key-pair based on the type of the key.
 * - Generate an RSA key pair using OPTIGA.
 * - If export is requested, exports the private key else stores it in the input private key OID.
 * - Always exports the public keys.
 *
 * \pre
 * - The application on OPTIGA must be opened before using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 * - RSA key pair generation on OPTIGA can go beyond 50 seconds therefore use a larger value for optiga comms TL_MAX_EXIT_TIMEOUT.
 *   The default value is hence set to 180 seconds.
 *
 * \param[in]       me                                      Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]       key_type                                RSA key type defined by #optiga_rsa_key_type_t.
 * \param[in]       key_usage                               Key usage defined by #optiga_key_usage_t.
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
optiga_lib_status_t optiga_crypt_rsa_generate_keypair(optiga_crypt_t * me,
                                                      optiga_rsa_key_type_t key_type,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length);
#endif //OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED


#ifdef OPTIGA_CRYPT_RSA_SIGN_ENABLED
 /**
 * \brief Generates a RSA signature for the given digest based on the input signature scheme.
 *
 * \details
 * Generates a signature for the given digest using private key stored in OPTIGA.
 * - Generates signature for the input digest.
 * - Exports the generated signature.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      signature_scheme                         RSA signature scheme defined by #optiga_rsa_signature_scheme_t
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
optiga_lib_status_t optiga_crypt_rsa_sign(optiga_crypt_t * me,
                                          optiga_rsa_signature_scheme_t signature_scheme,
                                          const uint8_t * digest,
                                          uint8_t digest_length,
                                          optiga_key_id_t private_key,
                                          uint8_t * signature,
                                          uint16_t * signature_length,
                                          uint16_t salt_length);
#endif //OPTIGA_CRYPT_RSA_SIGN_ENABLED

#ifdef OPTIGA_CRYPT_RSA_VERIFY_ENABLED
/**
 * \brief Verifies the RSA signature over the given digest.
 *
 * \details
 * Verifies the signature over a given digest provided with the input data.
 * - Verifies the signature over the given provided with the input data using public key.
 * - It invokes the callback handler of the instance, when it is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 * - Error codes from lower layers will be returned as it is to the application.<br>
 *
 * \param[in]   me                                        Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]   signature_scheme                          RSA signature scheme defined by #optiga_rsa_signature_scheme_t
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
optiga_lib_status_t optiga_crypt_rsa_verify(optiga_crypt_t * me,
                                            optiga_rsa_signature_scheme_t signature_scheme,
                                            const uint8_t * digest,
                                            uint8_t digest_length,
                                            const uint8_t * signature,
                                            uint16_t signature_length,
                                            uint8_t public_key_source_type,
                                            const void * public_key,
                                            uint16_t salt_length);
#endif //OPTIGA_CRYPT_RSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED
/**
 * \brief Generates a pre-master secret.
 *
 * \details
 * Generates a pre-master secret for RSA key exchange and stores in the acquired session
 * - Invokes #optiga_cmd_get_random API, based on the input arguments.
 * - Generates the pre-master secret
 * - Pre-master secret is stored in session OID.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.<br>
 * - Pre Master Secret :
 *      - Minimum length of generated pre master secret is 8 bytes.
 *      - The maximum length supported by OPTIGA is 48 bytes.
 *      - It is a concatenation of optional data and random secret.
 *
 * \param[in]      me                                       Valid instance of #optiga_crypt_t created using #optiga_crypt_create.
 * \param[in]      optional_data                            Optional data that gets prepended to the generated random secret.
 *                                                          - Can be set to NULL, if not required
 * \param[in]      optional_data_length                     Length of the optional data provided. It is ignored if optional_data is NULL
 *                                                          - Value should be less than 40 bytes
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

optiga_lib_status_t optiga_crypt_rsa_generate_pre_master_secret(optiga_crypt_t * me,
                                                                const uint8_t * optional_data,
                                                                uint16_t optional_data_length,
                                                                uint16_t pre_master_secret_length);
#endif //OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

/**
 * \brief Encrypts message using RSA public key.<br>
 *
 * \details
 * Encrypts message using RSA public key which is either provided by the host or stored in OPTIGA.
 * - Provides the input message to OPTIGA for encryption.
 * - Uses the RSA public key either provided by the host or the OID referring to public key certificate in OPTIGA.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is to the application.<br>
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
optiga_lib_status_t optiga_crypt_rsa_encrypt_message(optiga_crypt_t * me,
                                                     optiga_rsa_encryption_scheme_t encryption_scheme,
                                                     const uint8_t * message,
                                                     uint16_t message_length,
                                                     const uint8_t * label,
                                                     uint16_t label_length,
                                                     uint8_t public_key_source_type,
                                                     const void * public_key,
                                                     uint8_t * encrypted_message,
                                                     uint16_t * encrypted_message_length);

/**
 * \brief Encrypts session data using RSA public key.<br>
 *
 * \details
 * Encrypts session data using RSA public key which is either provided by the host or stored in OPTIGA.
 * - Provides the session OID to OPTIGA for encrypting the session data.
 * - Uses the RSA public key either provided by host or the OID referring to public key certificate in OPTIGA.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - A session OID must be acquired using #optiga_crypt_rsa_generate_pre_master_secret otherwise #OPTIGA_CMD_ERROR_INVALID_INPUT is returned.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *     - Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION.
 * - Error codes from lower layers will be returned as it is to the application.<br>
 * - <b>encrypted_message_length</b> Initially , it contains the size of buffer provided by user to store the encrypted data. On successful encryption, this value is updated with actual length of the encrypted data.<br>
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
optiga_lib_status_t optiga_crypt_rsa_encrypt_session(optiga_crypt_t * me,
                                                     optiga_rsa_encryption_scheme_t encryption_scheme,
                                                     const uint8_t * label,
                                                     uint16_t label_length,
                                                     uint8_t public_key_source_type,
                                                     const void * public_key,
                                                     uint8_t * encrypted_message,
                                                     uint16_t * encrypted_message_length);

#endif //OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
/**
 * \brief Decrypts input data using OPTIGA private key and export it to the host.<br>
 *
 * \details
 * Decrypts input data using RSA private key from OPTIGA and exports the decrypted data to the host.
 * - Provides the input data from decryption.
 * - Uses the RSA private key from OPTIGA that is referred by #optiga_key_id_t.
 * - Decrypted data is exported to the host.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL.
 *      - Default protection level for this API is OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - Error codes from lower layers will be returned as it is to the application.<br>
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
 optiga_lib_status_t optiga_crypt_rsa_decrypt_and_export(optiga_crypt_t * me,
                                                         optiga_rsa_encryption_scheme_t encryption_scheme,
                                                         const uint8_t * encrypted_message,
                                                         uint16_t encrypted_message_length,
                                                         const uint8_t * label,
                                                         uint16_t label_length,
                                                         optiga_key_id_t private_key,
                                                         uint8_t * message,
                                                         uint16_t * message_length);

 /**
 * \brief Decrypts input data using OPTIGA private key and stores it in a OPTIGA session.<br>
 *
 * \details
 * Decrypts input data using RSA private key from OPTIGA and stores it in a OPTIGA session.
 * - Provides the input data for decryption.
 * - Uses the RSA private key from OPTIGA that is referred by #optiga_key_id_t.
 * - Stores the decrypted data in the acquired OPTIGA session.
 * - The callback registered with instance (#optiga_crypt_create) gets invoked, when the operation is asynchronously completed.
 *
 * \pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 * \note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL
 *      - Default protection level for this API is OPTIGA_COMMS_RESPONSE_PROTECTION.
 * - Error codes from lower layers will be returned as it is to the application.<br>
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
 optiga_lib_status_t optiga_crypt_rsa_decrypt_and_store(optiga_crypt_t * me,
                                                        optiga_rsa_encryption_scheme_t encryption_scheme,
                                                        const uint8_t * encrypted_message,
                                                        uint16_t encrypted_message_length,
                                                        const uint8_t * label,
                                                        uint16_t label_length,
                                                        optiga_key_id_t private_key);

#endif //OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

/**
 * \brief Enables the protected I2C communication with OPTIGA for CRYPT instances
 *
 * \details
 * Enables the protected I2C communication with OPTIGA
 * - Sets the protection mode for the supplied instance.<br>
 * - Call this function before calling the service layer API which requires protection.
 *
 * \pre
 * - #OPTIGA_COMMS_SHIELDED_CONNECTION macro must be defined.<br>
 * - #OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION function must be called once to set the required protocol version
 *   - Currently only Pre-shared Secret based version is supported.
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
{ \
    optiga_crypt_set_comms_params(p_instance, \
                                         OPTIGA_COMMS_PROTECTION_LEVEL, \
                                         protection_level);\
}
#else
#define OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) {}
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
{ \
    optiga_crypt_set_comms_params(p_instance, \
                                         OPTIGA_COMMS_PROTOCOL_VERSION, \
                                         version);\
}
#else
#define OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(p_instance, version) {}
#endif

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_CRYPT_H_*/

/**
* @}
*/

