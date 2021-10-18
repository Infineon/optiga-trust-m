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
* \file optiga_cmd.h
*
* \brief   This file defines APIs, types and data structures used in the Command (cmd) module implementation.
*
* \ingroup  grOptigaCmd
*
* @{
*/

#ifndef _OPTIGA_CMD_H_
#define _OPTIGA_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/common/optiga_lib_common.h"

/** \brief OPTIGA command instance structure type*/
typedef struct optiga_cmd optiga_cmd_t;

/** \brief OPTIGA context instance structure type*/
typedef struct optiga_context optiga_context_t;

/**
 * \brief Creates an instance of #optiga_cmd_t.
 *
 * \details
 * Creates an instance of #optiga_cmd_t.
 * - Stores the callers context and callback handler.<br>
 * - Allocate memory for #optiga_cmd_t.<br>
 * - Assigns OPTIGA structure based on the optiga instance.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] optiga_instance_id  Indicates the OPTIGA configuration to associate with instance.
 * \param[in] handler             Pointer to callback function, must not be NULL.
 * \param[in] caller_context      Pointer to upper layer context.
 *
 * \retval    #optiga_cmd_t *     On successful instance creation.
 * \retval    NULL                Memory allocation failure.
 *                                Already, OPTIGA_CMD_MAX_REGISTRATIONS number of instances is created.
 */
optiga_cmd_t * optiga_cmd_create(uint8_t optiga_instance_id,
                                 callback_handler_t handler,
                                 void * caller_context);

/**
 * \brief Destroys the instance of #optiga_cmd_t.
 *
 * \details
 * Destroys the instance of #optiga_cmd_t.
 * - Releases any OPTIGA cmd module lock utilized by the instance.<br>
 * - Releases any OPTIGA session acquired by the instance.<br>
 * - Destroys optiga_comms and pal_os_event instances.<br>
 * - De-allocate the memory of the #optiga_cmd_t instance.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] me                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *
 * \retval    #OPTIGA_LIB_SUCCESS     Successful destruction of instance.
 */
optiga_lib_status_t optiga_cmd_destroy(optiga_cmd_t * me);


/**
 * \brief Releases the OPTIGA cmd lock.
 *
 * \details
 * Releases the OPTIGA cmd lock.
 * - Release the OPTIGA lock acquired by the instance.<br>
 * - If OPTIGA cmd lock request register in not empty, invoke the next registered caller handler with corresponding context.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] me                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *
 * \retval    #OPTIGA_CMD_SUCCESS     Successful release of the access layer lock.
 */
optiga_lib_status_t optiga_cmd_release_lock(const optiga_cmd_t * me);


/**
 * \brief Opens the OPTIGA Application
 *
 * \details
 * Opens the OPTIGA Application by issuing the OpenApplication command.
 * - Acquires the OPTIGA lock for #optiga_util_open_application.<br>
 * - Invokes #optiga_comms_open API to initialize OPTIGA and to establish the communication channel.<br>
 * - Forms the OpenApplication commands based on inputs.<br>
 * - Issues the OpenApplication command using #optiga_comms_transceive.<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - This function must be mandatorily invoked before starting any interactions with OPTIGA after the reset.
 * - Error codes from lower layers will be returned as it is.<br>
 *
 * \param[in] me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 * \param[in] cmd_param                                   Param of Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 * \param[in] params                                      Secure connection related information, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking OpenApplication command.
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_open_application(optiga_cmd_t * me,
                                                uint8_t cmd_param,
                                                void * params);

/**
 * \brief Closes the OPTIGA Application
 *
 * \details
 * Closes the OPTIGA Application by issuing the CloseApplication command.
 * - Acquires the OPTIGA lock for #optiga_util_close_application.<br>
 * - Forms the CloseApplication commands based on inputs.<br>
 * - Issues the CloseApplication command through #optiga_comms_transceive.<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - This function must be mandatorily invoked before issuing a reset to OPTIGA, in case the context needs to be saved.
 * - Error codes from lower layers will be returned as it is.<br>
 *
 * \param[in] me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 * \param[in] cmd_param                                   Param of Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 * \param[in] params                                      Secure connection related information, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking CloseApplication command.
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_close_application(optiga_cmd_t * me,
                                                 uint8_t cmd_param,
                                                 void * params);

/**
 * \brief Reads data or metadata of the specified data object
 *
 * \details
 * Reads data or metadata of the specified data object, by issuing the GetDataObject command, based on input parameters.
 * - Acquires the OPTIGA lock for #optiga_util_read_data/#optiga_util_read_metadata.<br>
 * - Forms the ReadData/ReadMetaData commands based on inputs.<br>
 * - Issues the ReadData/ReadMetaData command through #optiga_comms_transceive.<br>
 * - In case the data to be read by the user is greater than the <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>,
 *   the data is retrieved by internal chaining (by calling GetDataObject command multiple times).
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in]  me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                                   Param of Get Data Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in]  params                                      InData of Get Data Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking GetDataObject command.
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       Error due to insufficient buffer size.
 *                                                        - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                        - Data received from OPTIGA is greater than the buffer size provided by user.
 */
optiga_lib_status_t optiga_cmd_get_data_object(optiga_cmd_t * me,
                                               uint8_t cmd_param,
                                               optiga_get_data_object_params_t * params);

/**
 * \brief Writes data or metadata to the specified data object
 *
 * \details
 * Writes data or metadata to the specified data object, by issuing the SetDataObject command, based on input parameters.
 * - Acquires the OPTIGA lock for #optiga_util_write_data/#optiga_util_write_metadata.<br>
 * - Forms the SetData/SetMetaData commands based on inputs.<br>
 * - Issues the SetData/SetMetaData command through #optiga_comms_transceive.<br>
 * - In case the data to be written by the user is greater than the <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>,
 *   the data is written to the OPTIGA by internal chaining (by calling SetDataObject command multiple times).<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - In case of failure, it is possible that partial data is written into the data object.<br>
 *   In such a case, the user should decide if the data has to be re-written.
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in]  me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                                   Param of Set Data Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in]  params                                      InData of Set Data Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking SetDataObject command.
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_set_data_object(optiga_cmd_t * me,
                                               uint8_t cmd_param,
                                               optiga_set_data_object_params_t * params);

#if defined (OPTIGA_CRYPT_RANDOM_ENABLED) || defined (OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED)
/**
 * \brief Generates random data or pre-master secret for RSA key exchange.
 *
 * \details
 * Generates random data or pre-master secret for RSA key exchange, by issuing the GetRandom command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_rsa_generate_pre_master_secret/#optiga_crypt_random.<br>
 * - For pre-master secret, it acquires a session, else acquires the lock.
 * - Forms the GetRandom command based on inputs.<br>
 * - Issues the GetRandom command through #optiga_comms_transceive.<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in] me                                           Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in] cmd_param                                    Param of Get Random Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in] params                                       InData of Get Random Command APDU, must not be NULL.
 *
 * \retval   #OPTIGA_CMD_SUCCESS                          Successful invocation.
 * \retval   #OPTIGA_CMD_ERROR                            Error occurred before invoking GetRandom command.
 *                                                        Error in the asynchronous state machine.
 * \retval   #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT        APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_get_random(optiga_cmd_t * me,
                                          uint8_t cmd_param,
                                          optiga_get_random_params_t * params);
#endif //OPTIGA_CRYPT_RANDOM_ENABLED || OPTIGA_CRYPT_RSA_PRE_MASTER_SECRET_ENABLED

#ifdef OPTIGA_CRYPT_HASH_ENABLED
/**
 * \brief Generates hash on external data or data in OID.
 *
 * \details
 * Generates hash on external data or data in OID by issuing Calc Hash command to OPTIGA.
 * - Acquires the OPTIGA lock for #optiga_crypt_hash_start/#optiga_crypt_hash_update/#optiga_crypt_hash_finalize.<br>
 * - Forms the CalcHash command based on inputs.<br>
 * - Issues the CalcHash command through #optiga_comms_transceive.<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in] me                                           Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in] cmd_param                                    Param of CalcHash Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in] params                                       InData of CalcHash Command APDU, must not be NULL.
 *
 * \retval   #OPTIGA_CMD_SUCCESS                          Successful invocation.
 * \retval   #OPTIGA_CMD_ERROR                            Error occurred before invoking CalcHash command.<br>
 *                                                        Error in the asynchronous state machine.
 * \retval   #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT        Error due to insufficient buffer size.
 *                                                        - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                        - Data received from OPTIGA is greater than the buffer size provided by user.
 */
optiga_lib_status_t optiga_cmd_calc_hash(optiga_cmd_t * me,
                                         uint8_t cmd_param,
                                         optiga_calc_hash_params_t * params);
#endif //OPTIGA_CRYPT_HASH_ENABLED

#if defined (OPTIGA_CRYPT_ECDSA_SIGN_ENABLED) || defined (OPTIGA_CRYPT_RSA_SIGN_ENABLED)
/**
 * \brief Calculate signature on digest.
 *
 * \details
 * Calculate signature on digest by issuing Calc Sign command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdsa_sign/#optiga_crypt_rsa_sign.<br>
 * - Forms the Generate KeyPair command based on inputs.<br>
 * - Issues the Generate KeyPair command through #optiga_comms_transceive.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Calc Sign Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in]  params                                  InData of Calc Sign Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Calc Sign command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT         Instance invoked for session oid, without acquiring the session (from #optiga_cmd_gen_keypair).
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                    - Length of the buffer to copy the signature is less than actual length of signature.
 */
optiga_lib_status_t optiga_cmd_calc_sign(optiga_cmd_t * me,
                                         uint8_t cmd_param,
                                         optiga_calc_sign_params_t * params);
#endif //OPTIGA_CRYPT_ECDSA_SIGN_ENABLED || OPTIGA_CRYPT_RSA_SIGN_ENABLED

#if defined (OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED) || defined (OPTIGA_CRYPT_RSA_VERIFY_ENABLED)
/**
 * \brief Verifies the signature over the given digest.
 *
 * \details
 * Verifies the signature over the given digest by issuing VerifySign command.
 * - Acquires the OPTIGA lock for #optiga_crypt_ecdsa_verify/#optiga_crypt_rsa_verify.<br>
 * - Forms the VerifySign command based on inputs and invokes the optiga_comms_transceive to send the same to OPTIGA.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in]  me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                                   Param of VerifySign Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in]  params                                      Parameters for InData of VerifySign Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking VerifySign command.<br>
 *                                                        Error in the asynchronous state machine.
 * \retval   #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT        APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_verify_sign(optiga_cmd_t * me,
                                           uint8_t cmd_param,
                                           optiga_verify_sign_params_t * params);
#endif //OPTIGA_CRYPT_ECDSA_VERIFY_ENABLED || OPTIGA_CRYPT_RSA_VERIFY_ENABLED

#ifdef OPTIGA_CRYPT_ECDH_ENABLED
/**
 * \brief Calculates shared secret.
 *
 * \details
 * Calculates shared secret by issuing CalcSSec command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdh.<br>
 * - Forms the command apdu based on inputs.<br>
 * - Issues the command through #optiga_comms_transceive.<br>
 * - If the private key is session based or shared secret is to be stored in OPTIGA, it acquires a session if not already available.<br>
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application.
 * - A key pair must be generated/available in the given private key ID.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in] me                                           Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in] cmd_param                                    Param of Calc SSec Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in] params                                       Pointer to input parameters, must not be NULL.
 *
 * \retval   #OPTIGA_LIB_SUCCESS                          Successful invocation of optiga_comms module.
 * \retval   #OPTIGA_CMD_ERROR                            Error occurred before invoking CalcSSec command <br>
 *                                                        #optiga_comms_transceive returned a failure <br>
 *                                                        Error in the asynchronous state machine.
 * \retval   #OPTIGA_CMD_ERROR_INVALID_INPUT              Instance invoked for session oid, without acquiring the session (from #optiga_cmd_gen_keypair).
 * \retval   #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT        APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_calc_ssec(optiga_cmd_t * me,
                                         uint8_t cmd_param,
                                         optiga_calc_ssec_params_t * params);
#endif //OPTIGA_CRYPT_ECDH_ENABLED

#if defined (OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED) || defined (OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED) || defined (OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED) || defined (OPTIGA_CRYPT_HKDF_ENABLED)
/**
 * \brief Derives a key.
 *
 * \details
 * Derives a key by issuing DeriveKey command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_ecdh.<br>
 * - Issues the calculate shared secret command through #optiga_comms_transceive.
 * - Based on the shared secret location in OPTIGA and target storage, it requests to acquire a session.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 * - A key pair should be generated for the input object ID.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                                   Param of Derive Key Command APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                      InData of derive key Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking derive key command <br>
 *                                                        optiga comms transceive returned a failure <br>
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT             Instance invoked for session oid, without acquiring the session(from #optiga_cmd_calc_ssec).
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_derive_key(optiga_cmd_t * me,
                                          uint8_t cmd_param,
                                          optiga_derive_key_params_t * params);
#endif //OPTIGA_CRYPT_TLS_PRF_SHA256_ENABLED || OPTIGA_CRYPT_TLS_PRF_SHA384_ENABLED || OPTIGA_CRYPT_TLS_PRF_SHA512_ENABLED || OPTIGA_CRYPT_HKDF_ENABLED

#if defined (OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) || defined (OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED)
/**
 * \brief Generates ECC or RSA key-pair.
 *
 * \details
 * Generate ECC/RSA key-pair by issuing Generate KeyPair command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_ecc_generate_keypair/#optiga_crypt_rsa_generate_keypair.<br>
 * - Forms the Generate KeyPair command based on inputs.
 * - Issues the Generate KeyPair command through #optiga_comms_transceive.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Generate KeyPair Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Generate KeyPair Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Generate KeyPair command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                    - Length of the buffer to copy the key is less than actual length of key.
 */
optiga_lib_status_t optiga_cmd_gen_keypair(optiga_cmd_t * me,
                                           uint8_t cmd_param,
                                           optiga_gen_keypair_params_t * params);
#endif //(OPTIGA_CRYPT_ECC_GENERATE_KEYPAIR_ENABLED) || (OPTIGA_CRYPT_RSA_GENERATE_KEYPAIR_ENABLED)

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED
/**
 * \brief Encrypt data using RSA public key.
 *
 * \details
 * Encrypts data using RSA public key by issuing Encrypt Asym command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_rsa_encrypt_message/#optiga_crypt_rsa_encrypt_session.<br>
 * - Forms the Encrypt Asym command based on inputs.
 * - Issues the Encrypt Asym command through #optiga_comms_transceive.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Encrypt Asym Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Encrypt Asym Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Encrypt Asym command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT         Instance invoked for encrypting session, without acquiring the session
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                    - Length of the buffer to copy the encrypted data is less than buffer to copy it into.
 */
optiga_lib_status_t optiga_cmd_encrypt_asym(optiga_cmd_t * me,
                                            uint8_t cmd_param,
                                            optiga_encrypt_asym_params_t * params);
#endif // OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED
/**
 * \brief Decrypts data using OPTIGA RSA private key.
 *
 * \details
 * Decrypts data using OPTIGA RSA private key by issuing Decrypt Asym command to OPTIGA.
 * - Acquires the OPTIGA session/lock for #optiga_crypt_rsa_decrypt_and_export/#optiga_crypt_rsa_decrypt_and_store.<br>
 * - Forms the Decrypt Asym command based on inputs.
 * - Issues the Decrypt Asym command through #optiga_comms_transceive.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Decrypt Asym Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Decrypt Asym Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Decrypt Asym command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 *                                                    - Length of the buffer to copy the decrypted data is less than buffer to copy it into.
 */
optiga_lib_status_t optiga_cmd_decrypt_asym(optiga_cmd_t * me,
                                            uint8_t cmd_param,
                                            optiga_decrypt_asym_params_t * params);
#endif // OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

/**
 * \brief Writes protected object fragments to OPTIGA.
 *
 * \details
 * Writes manifest or data fragement(s) to the specified data object, by issuing the SetObjectProtected command, in a securely.
 * - Acquires the strict sequence for #optiga_util_protected_update_start.<br>
 * - Forms the SetObjectProtected commands based on inputs.<br>
 * - Issues the SetObjectProtected command through #optiga_comms_transceive.<br>
 * - Releases the strict sequence in case of an error or after #optiga_util_protected_update_final is successfully completed.
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.<br>
 *
 * \note
 * - Error codes from lower layers will be returned as it is.<br>
 *
 *\param[in]  me                                          Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                                   Param of Set Object Protected APDU.
 *                                                        - Must be valid argument, otherwise OPTIGA returns an error.<br>
 *\param[in]  params                                      InData of Set Object Protected APDU, must not be NULL.
 *
 * \retval    #OPTIGA_CMD_SUCCESS                         Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                           Error occurred before invoking SetObjectProtected command.
 *                                                        Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT       APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT             Continue and final APDU command invoked without strict lock acquired for the instance.
 */
optiga_lib_status_t optiga_cmd_set_object_protected(optiga_cmd_t * me,
                                                    uint8_t cmd_param,
                                                    optiga_set_object_protected_params_t * params);
                                                    
#if defined (OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) || defined (OPTIGA_CRYPT_HMAC_ENABLED)
/**
 * \brief Encrypt data using #optiga_symmetric_encryption_mode_t encryption scheme.
 *
 * \details
 * Encrypts data using selected encryption scheme by issuing Encrypt Sym command to OPTIGA.
 * - Acquires the strict sequence.
 * - Forms the Encrypt Sym command based on inputs.
 * - Issues the Encrypt Sym command through #optiga_comms_transceive.
 * - Releases the strict sequence in case of an error or after #optiga_crypt_symmetric_encrypt_final is successfully completed.
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Encrypt Sym Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Encrypt Sym Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Encrypt Sym command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT         Continue and final APDU command invoked without strict lock acquired for the instance.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - Length of the buffer to copy the encrypted data is less than buffer to copy it into.
 */
optiga_lib_status_t optiga_cmd_encrypt_sym(optiga_cmd_t * me,
                                           uint8_t cmd_param,
                                           optiga_encrypt_sym_params_t * params);
#endif

#if defined (OPTIGA_CRYPT_SYM_DECRYPT_ENABLED) || defined (OPTIGA_CRYPT_HMAC_VERIFY_ENABLED) ||\
    defined (OPTIGA_CRYPT_CLEAR_AUTO_STATE_ENABLED)
/**
 * \brief Decrypt data using #optiga_symmetric_encryption_mode_t encryption scheme.
 *
 * \details
 * Decrypts data using selected encryption scheme by issuing Decrypt Sym command to OPTIGA.
 * - Acquires the strict sequence.
 * - Forms the Decrypt Sym command based on inputs.
 * - Issues the Decrypt Sym command through #optiga_comms_transceive.
 * - Releases the strict sequence in case of an error or after #optiga_crypt_symmetric_decrypt_final is successfully completed.
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Decrypt Sym Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Decrypt Sym Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Encrypt Sym command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT         Continue and final APDU command invoked without strict lock acquired for the instance.
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - Length of the buffer to copy the encrypted data is less than buffer to copy it into.
 */
optiga_lib_status_t optiga_cmd_decrypt_sym(optiga_cmd_t * me,
                                           uint8_t cmd_param,
                                           optiga_decrypt_sym_params_t * params);
#endif

#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
/**
 * \brief Generate symmetric key using OPTIGA.
 *
 * \details
 * Generate symmetric key by issuing Generate Symmetric Key command to OPTIGA.
 * - Acquires the OPTIGA lock for #optiga_crypt_symmetric_generate_key.<br> 
 * - Forms the Generate Symmetric Key command based on inputs.
 * - Issues the Generate Symmetric Key command through #optiga_comms_transceive.
 * - Releases the OPTIGA lock on successful completion of asynchronous operation.<br>
 *
 * \pre
 * - Application on OPTIGA must be opened using #optiga_cmd_open_application before using this API.
 *
 * \note
 * - Error codes from lower layers will be returned as it is.
 *
 *\param[in]  me                                      Valid instance of #optiga_cmd_t created using #optiga_cmd_create.
 *\param[in]  cmd_param                               Param of Gen Sym Key Command APDU.
 *                                                    - Must be valid argument, otherwise OPTIGA returns an error.
 *\param[in]  params                                  InData of Gen Sym Key Command APDU, must not be NULL.
 *
 * \retval    #OPTIGA_LIB_SUCCESS                     Successful invocation.
 * \retval    #OPTIGA_CMD_ERROR                       Error occurred before invoking Gen Sym Key command.<br>
 *                                                    Error in the asynchronous state machine.
 * \retval    #OPTIGA_CMD_ERROR_INVALID_INPUT         Instance invoked for encrypting session, without acquiring the session
 * \retval    #OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT   Error due to insufficient buffer size.
 *                                                    - APDU length formed is greater than <b>OPTIGA_MAX_COMMS_BUFFER_SIZE</b>.
 */
optiga_lib_status_t optiga_cmd_gen_symkey(optiga_cmd_t * me,
                                          uint8_t cmd_param,
                                          optiga_gen_symkey_params_t * params);
#endif // OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_CMD_H_ */

/**
* @}
*/
