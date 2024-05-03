/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_util.h
 *
 * \brief   This file defines APIs, types and data structures used in the OPTIGA utility module.
 *
 * \ingroup  grOptigaUtil
 *
 * @{
 */

#ifndef _OPTIGA_UTIL_H_
#define _OPTIGA_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_cmd.h"

/// Option to only write the data object
#define OPTIGA_UTIL_WRITE_ONLY (0x00)
/// Option to erase and write the data object
#define OPTIGA_UTIL_ERASE_AND_WRITE (0x40)

/// To Initialize a clean application context
#define OPTIGA_UTIL_CONTEXT_NONE (0x00)

/** \brief union for OPTIGA util parameters */
typedef union optiga_util_params {
    // set data object params
    optiga_set_data_object_params_t optiga_set_data_object_params;
    // get data object params
    optiga_get_data_object_params_t optiga_get_data_object_params;
    // set object protected params
    optiga_set_object_protected_params_t optiga_set_object_protected_params;
} optiga_util_params_t;

/** \brief OPTIGA util instance structure */
struct optiga_util {
    /// Details/references (pointers) to the Application Inputs
    optiga_util_params_t params;
    /// pointer to optiga command instance
    optiga_cmd_t *my_cmd;
    /// Callback context
    void *caller_context;
    /// Callback handler
    callback_handler_t handler;
    /// To provide the busy/free status of the util instance
    uint16_t instance_state;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide the encryption and decryption need for command and response
    uint8_t protection_level;
    /// To provide the presentation layer protocol version to be used
    uint8_t protocol_version;
#endif  // OPTIGA_COMMS_SHIELDED_CONNECTION
};
/** \brief OPTIGA util instance structure type*/
typedef struct optiga_util optiga_util_t;

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
void optiga_util_set_comms_params(optiga_util_t *me, uint8_t parameter_type, uint8_t value);
#endif
/**
 * \brief Create an instance of #optiga_util_t.
 *
 *\details
 * Create an instance of #optiga_util_t.
 * - Stores the callers context and callback handler.
 * - Allocate memory for #optiga_util_t.
 *
 *\pre
 * - None
 *
 *\note
 * - This API is implemented in synchronous mode.
 * - For <b>protected I2C communication</b>,
 *      - Default protection level for this API is #OPTIGA_COMMS_DEFAULT_PROTECTION_LEVEL.
 *      - Default protocol version for this API is #OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET.
 *
 * \param[in]   optiga_instance_id    Indicates the OPTIGA instance to be associated with #optiga_util_t. Value should be defined as below
 *                                    - #OPTIGA_INSTANCE_ID_0 : Indicate created instance will be part of OPTIGA with slave address 0x30.
 * \param[in]   handler               Valid pointer to callback function
 * \param[in]   caller_context        Pointer to upper layer context, contains user context data
 *
 * \retval      #optiga_util_t        On success function will return pointer of #optiga_util_t
 * \retval      NULL                Input arguments are NULL.<br>
 *                                  Low layer function fails.<br>
 *                                  OPTIGA_CMD_MAX_REGISTRATIONS number of instances are already created.
 *
 * <b>Example</b><br>
 * main_xmc4800_sample.c
 */
LIBRARY_EXPORTS optiga_util_t *
optiga_util_create(uint8_t optiga_instance_id, callback_handler_t handler, void *caller_context);

/**
 * \brief De-Initializes the OPTIGA util instance.
 *
 *\details
 * De-Initializes the #optiga_util_t instance.
 * - De-allocate the memory of the #optiga_util_t instance.
 *
 *\pre
 * - None
 *
 *\note
 *  - User must take care to nullify the instance pointer.
 *  - Invoke this API only after all the asynchronous process is completed otherwise the behavior of software stack is not defined.
 *
 * \param[in]  me                                    Valid instance of #optiga_util_t
 *
 * \retval     #OPTIGA_LIB_SUCCESS                   Successful invocation
 * \retval     #OPTIGA_UTIL_ERROR_INVALID_INPUT      Wrong Input arguments provided
 * \retval     #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE    The previous operation with the same instance is not complete
 *
 * <b>Example</b><br>
 * main_xmc4800_sample.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_destroy(optiga_util_t *me);

/**
 * \brief Initializes the communication with optiga and open the application on OPTIGA.
 *
 *\details
 * Initializes the communication with OPTIGA for the given instance.
 * - Sets up optiga comms channel.<br>
 * - Initiate open application command to optiga.<br>
 * - Perform manage context , session restore operations and application restore.<br>
 *
 *\pre
 * - For restoring OPTIGA application, the application must be in hibernate state using #optiga_util_close_application.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layer will be returned as it is.
 * - If error in lower layer occurs while restoring OPTIGA application, then initialize a clean OPTIGA application context.
 *
 * \param[in]   me                                    Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]   perform_restore                       Restore application on OPTIGA from a previous hibernate state. The values must be as defined below
 *                                                    - TRUE (1) - Performs application restore. <br>
 *                                                    - FALSE (0) - Initialize a clean application context.
 *
 * \retval      #OPTIGA_UTIL_SUCCESS                  Successful invocation
 * \retval      #OPTIGA_UTIL_ERROR_INVALID_INPUT      Wrong Input arguments provided
 * \retval      #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE    The previous operation with the same instance is not complete
 * \retval      #OPTIGA_DEVICE_ERROR                  Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                    (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * main_xmc4800_sample.c
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_util_open_application(optiga_util_t *me, bool_t perform_restore);

/**
 *\brief Closes the application on OPTIGA and closes the communication with optiga.
 *
 *\details
 * Closes the communication with OPTIGA for the given instance.
 * - Initiate close application command to optiga.
 * - Perform manage context secure session save operations.
 * - De-Initializes the OPTIGA and closes the communication channel.
 * - Power downs the OPTIGA after closing the application on optiga.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layer will be returned as it is.
 * - Close application with hibernate and shielded connection establishment(negotiation/re-negotiation) will fail at OPTIGA because hibernation is not allowed when SEC value is greater than zero.
 *   The SEC value increments internally on every shielded connection negotiation process.

 *
 *<br>
 * \param[in]   me                                       Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]   perform_hibernate                        Hibernate the application on OPTIGA. The values must be as defined below.
 *                                                        - TRUE (1) - Performs application hibernate. <br>
 *                                                        - FALSE (0) - De-Initializes application context.
 *
 * \retval      #OPTIGA_UTIL_SUCCESS                     Successful invocation
 * \retval      #OPTIGA_UTIL_ERROR_INVALID_INPUT         Input arguments are NULL
 * \retval      #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete
 * \retval      #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                       (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * main_xmc4800_sample.c
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_util_close_application(optiga_util_t *me, bool_t perform_hibernate);

/**
 * \brief Reads data from optiga.
 *
 *\details
 * Retrieves the requested data that is stored in the user provided data object.<br>
 * - Invokes #optiga_cmd_get_data_object API and based on the input arguments, read the data from the data object.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 * - The maximum value of the <b>*length</b> parameter must be the size of buffer <b>buffer</b>. In case the value is greater than buffer size, memory corruption can occur.<br>
 * - If any errors occur while retrieving the data, <b>*length</b> parameter is set to 0.
 *
 * \param[in]      me                                     Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      optiga_oid                             OID of data object
 *                                                        - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      offset                                 Offset from within data object
 *                                                        - It must be valid offset from within data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer                                 Valid pointer to the buffer to which data is read
 * \param[in,out]  length                                 Valid pointer to the length of data to be read from data object
 *                                                        - When the data is successfully retrieved, it is updated with actual data length retrieved
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                   Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                   Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_read_data.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_read_data(
    optiga_util_t *me,
    uint16_t optiga_oid,
    uint16_t offset,
    uint8_t *buffer,
    uint16_t *length
);

/**
 * \brief Reads metadata of the specified data object from optiga.
 *
 *\details
 * Reads the metadata of the user provided data object.
 * - Invokes #optiga_cmd_get_data_object API, based on the input arguments to read the metadata from the data object.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.
 * - The metadata returned will be in TLV format.
 * - The maximum value of the <b>*length</b> parameter  must be the size of <b>buffer</b>. In case the value is greater than buffer size, memory corruption can occur.<br>
 * - If any errors occur while retrieving the data, <b>*length</b> parameter is set to 0.
 *
 * \param[in]      me                                       Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      optiga_oid                               OID of data object
 *                                                          - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer                                   Valid pointer to the buffer to which metadata is read
 * \param[in,out]  length                                   Valid pointer to the length of metadata to be read from data object
 *                                                          - When the metadata is successfully retrieved, it is updated with actual metadata length retrieved
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                     Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT         Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE       The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                     Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                          (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_read_data.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_read_metadata(
    optiga_util_t *me,
    uint16_t optiga_oid,
    uint8_t *buffer,
    uint16_t *length
);

/**
 * \brief Writes data to optiga.
 *
 *\details
 * Writes the data provided by the user into the specified data object.<br>
 * - Invokes #optiga_cmd_set_data_object API, based on the input arguments to write the data to the data object.<br>
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.<br>
 * - The maximum value of the <b>length parameter</b> is size of <b>buffer</b>. In case the value is greater than buffer size, incorrect values can get written into the data object in OPTIGA.<br>
 * - In case the write_type provided is other than <b>erase and write(0x00)</b> or <b>write only(0x40)</b>, the function returns #OPTIGA_UTIL_ERROR_INVALID_INPUT.<br>
 *
 * \param[in]      me                                        Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      optiga_oid                                OID of data object
 *                                                           - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      write_type                                Type of write must be either #OPTIGA_UTIL_WRITE_ONLY or #OPTIGA_UTIL_ERASE_AND_WRITE.<br>
 * \param[in]      offset                                    Offset from within data object
 *                                                           - It must be valid offset from within data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      buffer                                    Valid pointer to the buffer with user data to write
 * \param[in]      length                                    Length of data to be written
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                      Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT          Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE        The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                           (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_write_data.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_write_data(
    optiga_util_t *me,
    uint16_t optiga_oid,
    uint8_t write_type,
    uint16_t offset,
    const uint8_t *buffer,
    uint16_t length
);

/**
 * \brief Writes metadata for the user provided data object.
 *
 *\details
 * Writes metadata for the specified data object.
 * - Invokes #optiga_cmd_set_data_object API, based on the input arguments to write metadata to the data object.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.<br>
 * - The maximum value of the <b>length</b> parameter is size of <b>buffer</b>.
 *   In case the value is greater than buffer size, incorrect values can get written into the meta data of the data object in OPTIGA.<br>
 * - The metadata to be written must be in TLV format.
 *
 * \param[in]      me                                     Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      optiga_oid                             OID of data object
 *                                                        - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      buffer                                 Valid pointer to the buffer with metadata to write
 * \param[in]      length                                 Length of metadata to be written
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                   Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                   Command execution failure in OPTIGA and the LSB indicates the error code.
 *
 * <b>Example</b><br>
 * example_optiga_util_write_data.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_write_metadata(
    optiga_util_t *me,
    uint16_t optiga_oid,
    const uint8_t *buffer,
    uint8_t length
);

/**
 * \brief Initiates the start of protected update of object by writing manifest into OPTIGA object.
 *
 *\details
 * Initiates the start of protected update of object.
 * - It marks the beginning of a strict sequence. None of the service layer API will be processed until the strict sequence is terminated.
 * - Invokes #optiga_cmd_set_object_protected API, based on the input arguments to send manifest to OPTIGA.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 *      - The protection level set in this API will be used for #optiga_util_protected_update_continue and #optiga_util_protected_update_final.<br>
 * - Error codes from lower layers will be returned as it is.<br>
 * - The strict sequence is terminated
 *   - In case of an error from lower layer.<br>
 *   - Same instance is used for other service layer APIs (except #optiga_util_protected_update_continue).<br>
 *
 * \param[in]      me                                     Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      manifest_version                       Version of manifest to be written
 * \param[in]      manifest                               Valid pointer to the buffer which contains manifest
 *                                                        - It should be a valid manifest, otherwise OPTIGA returns an error.<br>
 * \param[in]      manifest_length                        Length of manifest to be written
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                   Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                   Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_protected_update.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_protected_update_start(
    optiga_util_t *me,
    uint8_t manifest_version,
    const uint8_t *manifest,
    uint16_t manifest_length
);

/**
 * \brief Sends fragment(s) of data to be written to OPTIGA.
 *
 *\details
 * Sends a fragment of data to be written to OPTIGA.
 * - Invokes #optiga_cmd_set_object_protected API, to send the fragment of data to write to OPTIGA.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 * - The manifest must be written to OPTIGA using #optiga_util_protected_update_start, otherwise lower layer returns an error.
 *\note
 * - The <b>protected I2C communication</b> value set in #optiga_util_protected_update_start will be used in this API.<br>
 * - Error codes from lower layers will be returned as it is.<br>
 * - For writing 'n' fragment (s) ,
 *   - if n > 1 : Send 1 to 'n - 1' fragments using #optiga_util_protected_update_continue and and the final fragment using #optiga_util_protected_update_final.<br>
 *   - if n == 1 : Send the fragment using #optiga_util_protected_update_final.<br>
 * - Chaining of fragments and the sequence of sending fragments must be handled externally to this API.<br>
 * - The strict sequence is terminated in case of an error from lower layer<br>
 *
 * \param[in]      me                                     Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      fragment                               Valid pointer to the buffer which contains fragment
 *                                                        - It must be a valid fragment, otherwise OPTIGA returns an error.<br>
 * \param[in]      fragment_length                        Length of fragment to be written
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                   Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                   Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_protected_update.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_protected_update_continue(
    optiga_util_t *me,
    const uint8_t *fragment,
    uint16_t fragment_length
);

/**
 * \brief Sends the last fragment to finalize the protected update of data object.
 *
 *\details
 * Sends the last fragment to finalize the protected update of data object.
 * - Invokes #optiga_cmd_set_object_protected API, based on the input arguments to write the final fragment to data object.
 * - It terminates the strict sequence.
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application.
 * - The manifest must be written to OPTIGA using #optiga_util_protected_update_start, otherwise lower layer returns an error.
 *
 *\note
 * - The <b>protected I2C communication</b> value set in #optiga_util_protected_update_start will be used in this API.<br>
 * - Error codes from lower layers will be returned as it is.<br>
 * - The strict sequence is terminated in case of an error from lower layer.<br>
 *
 * \param[in]      me                                     Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      fragment                               Valid pointer to the buffer which contains the last fragment.
 *                                                        - If NULL, only strict sequence is released and no APDU is sent to OPTIGA
 * \param[in]      fragment_length                        Length of fragment to be written
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                   Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT       Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE     The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                   Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                        (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_protected_update.c
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_util_protected_update_final(
    optiga_util_t *me,
    const uint8_t *fragment,
    uint16_t fragment_length
);

/**
 * \brief Increments the counter object by a value specified by user.
 *
 *\details
 * Increments the counter object by a value specified by user.<br>
 * - Invokes #optiga_cmd_set_data_object API, based on the input arguments to update the count to the specified data object.<br>
 *
 *\pre
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 * - Initial count and threshold value must be set using #optiga_util_write_data.<br>
 *
 *\note
 * - For <b>protected I2C communication</b>, Refer #OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL
 * - Error codes from lower layers will be returned as it is.<br>
 *
 * \param[in]      me                                        Valid instance of #optiga_util_t created using #optiga_util_create.
 * \param[in]      optiga_counter_oid                        OID of counter data object
 *                                                           - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      count                                     Counter value to be updated
 *
 * \retval         #OPTIGA_UTIL_SUCCESS                      Successful invocation
 * \retval         #OPTIGA_UTIL_ERROR_INVALID_INPUT          Wrong Input arguments provided
 * \retval         #OPTIGA_UTIL_ERROR_INSTANCE_IN_USE        The previous operation with the same instance is not complete
 * \retval         #OPTIGA_DEVICE_ERROR                      Command execution failure in OPTIGA and the LSB indicates the error code.
 *                                                           (Refer Solution Reference Manual)
 *
 * <b>Example</b><br>
 * example_optiga_util_update_count.c
 */
LIBRARY_EXPORTS optiga_lib_status_t
optiga_util_update_count(optiga_util_t *me, uint16_t optiga_counter_oid, uint8_t count);

/**
 * \brief Enables the protected I2C communication with OPTIGA for UTIL instances
 *
 * \details
 * Enables the protected I2C communication with OPTIGA
 * - Sets the protection mode for the supplied instance.<br>
 * - Call this function before calling the service layer API which requires protection.
 *
 * \pre
 * - #OPTIGA_COMMS_SHIELDED_CONNECTION macro must be defined.<br>
 * - #OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION function must be called once to set the required protocol version
 *   - Currently only Pre-shared Secret based version is supported.
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
#define OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    { optiga_util_set_comms_params(p_instance, OPTIGA_COMMS_PROTECTION_LEVEL, protection_level); }
#else
#define OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(p_instance, protection_level) \
    {}
#endif
/**
 * \brief Select the protocol version required for the I2C protected communication for UTIL instances
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
#define OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    { optiga_util_set_comms_params(p_instance, OPTIGA_COMMS_PROTOCOL_VERSION, version); }
#else
#define OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(p_instance, version) \
    {}
#endif

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_UTIL_H_ */

/**
 * @}
 */
