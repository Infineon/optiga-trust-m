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
* \file optiga_comms.h
*
* \brief   This file implements Optiga comms abstraction layer for IFX I2C Protocol.
*
* \ingroup  grOptigaComms
*
* @{
*/


#ifndef _OPTIGA_COMMS_H_
#define _OPTIGA_COMMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/common/optiga_lib_types.h"
#include "optiga/common/optiga_lib_return_codes.h"
#include "optiga/common/optiga_lib_common.h"

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
// Offset for data
#define OPTIGA_COMMS_DATA_OFFSET         (0x05)
// Overhead buffer size for user buffer
#define OPTIGA_COMMS_PRL_OVERHEAD        (0x0D)
#else
#define OPTIGA_COMMS_DATA_OFFSET         (0x00)
#define OPTIGA_COMMS_PRL_OVERHEAD        (0x00)
#endif

/** @brief Optiga comms structure */
typedef struct optiga_comms
{
    /// Comms structure pointer
    void * p_comms_ctx;
    /// Upper layer context
    void * p_upper_layer_ctx;
    /// Upper layer handler
    upper_layer_callback_t upper_layer_handler;
    /// Holds the instance initialization state
    uint8_t instance_init_state;
    /// OPTIGA comms state
    uint8_t state;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    /// To provide the encryption and decryption need for command and response
    uint8_t protection_level;
    /// To provide the presentation layer protocol version to be used
    uint8_t protocol_version;
    /// To provide the option to save and restore the optiga comms presentation layer context
    uint8_t manage_context_operation;
#endif
    /// Pointer to the pal os event instance/context
    void * p_pal_os_event_ctx;
} optiga_comms_t;

/** @brief optiga communication structure */
extern optiga_comms_t optiga_comms;

/**
 * \brief Provides the singleton OPTIGA instance.
 *
 * \details
 * Creates an instance of #optiga_comms_t.
 * - Stores the callers context and callback handler.
 * - Allocate memory for #optiga_comms_t.
 * - Assigns OPTIGA structure based on the Optiga instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] callback            Pointer to callback function, must not be NULL
 * \param[in] context             Pointer to upper layer context.
 *
 * \retval    #optiga_comms_t *   On successful instance creation
 * \retval    NULL                Memory allocation failure
 */
optiga_comms_t * optiga_comms_create(callback_handler_t callback,
                                     void * context);

/**
 * \brief Deinitializes the OPTIGA comms instance
 *
 * \details
 * Destroys the instance of #optiga_comms_t.
 * - Releases any OPTIGA cmd module lock utilized by the instance.
 * - Releases any OPTIGA session acquired by the instance.
 * - Deallocates the memory of the #optiga_comms_t instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] optiga_comms         Valid instance of #optiga_comms_t created using #optiga_comms_create
 *
 */
void optiga_comms_destroy(optiga_comms_t * optiga_comms);

/**
 * \brief Sets the callers context to OPTIGA comms instance
 *
 * \details
 * Sets the callers context to #optiga_comms_t.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_optiga_comms         Valid instance of #optiga_comms_t created using #optiga_comms_create
 * \param[in] context                Pointer to callers context
 *
 */
optiga_lib_status_t optiga_comms_set_callback_context(optiga_comms_t * p_optiga_comms,
                                                      void *context);

/**
 * \brief Sets the callback handler to OPTIGA comms instance
 *
 * \details
 * Sets the callback handler to #optiga_comms_t instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_optiga_comms         Valid instance of #optiga_comms_t created using #optiga_comms_create
 * \param[in] handler                Pointer to callback handler
 */
optiga_lib_status_t optiga_comms_set_callback_handler(optiga_comms_t * p_optiga_comms,
                                                      callback_handler_t handler);
/**
 * \brief Opens the communication channel with OPTIGA.
 *
 * \details
 * Initializes the communication with OPTIGA
 * - Initializes OPTIGA and establishes the communication channel.
 * - Initializes the IFX I2C protocol stack and registers the event callbacks.
 * - Negotiates the frame size and bit rate with the OPTIGA.
 *
 * \pre
 * - None
 *
 * \note
 * - The following parameters in #ifx_i2c_context_t must be initialized with appropriate values <br>
 *   - <b>slave address</b> : Address of I2C slave
 *   - <b>frame_size</b> : Frame size in bytes. Minimum supported value is 16 bytes.<br>
 *              - It is recommended not to use a value greater than the slave's frame size.
 *              - The user specified frame size is written to I2C slave's frame size register.
 *                The frame size register is read back from I2C slave.
 *                This frame value is used by the IFX-I2C protocol even if it is not equal to the user specified value.
 *
 *   - <b>frequency</b> : Frequency/speed of I2C master in KHz.
 *              - This must be lowest of the maximum frequency supported by the devices (master/slave) connected on the bus.
 *              - Initial negotiation starts with a frequency of 100KHz.
 *              - If the user specified frequency is more than 400 KHz, the I2C slave is configured to operate in "Fm+" mode,
 *                otherwise the I2C slave is configured for "SM & Fm" mode. <br>
 *              - If the user specified frequency negotiation fails, the I2C master frequency remains at 100KHz<br>
 *
 *   - <b>upper_layer_event_handler</b> : Upper layer event handler. This is invoked when #ifx_i2c_open() is asynchronously completed.
 *   - <b>upper_layer_ctx</b> : Context of upper layer.
 *   - <b>p_slave_vdd_pin</b> : GPIO pin for VDD. If not set, cold reset is not done.
 *   - <b>p_slave_reset_pin</b> : GPIO pin for Reset. If not set, warm reset is not done.
 *   - <b>manage_contex_operation</b> : Used for managing session context and the values must be as defined below. The value of the parameter is not modified by the IFX-I2C protocol stack.
 *       - #OPTIGA_COMMS_SESSION_CONTEXT_RESTORE : Restore any stored session context.
 *       - #OPTIGA_COMMS_SESSION_CONTEXT_NONE : No manage context operation.
 * - The values of registers MAX_SCL_FREQU and DATA_REG_LEN, read from slave are not validated.
 * - At present, only single instance of #ifx_i2c_context_t is supported.
 * - Manage context operations will start in next transceive.
 *
 * \param[in,out] p_ctx                     Valid instance of #optiga_comms_t created using #optiga_comms_create
 *
 * \retval        #OPTIGA_COMMS_SUCCESS
 * \retval        #OPTIGA_COMMS_ERROR
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_comms_open(optiga_comms_t * p_ctx);


/**
 * \brief Resets the OPTIGA
 *
 * \details
 * Resets the communication channel with OPTIGA.
 * - Resets the OPTIGA device.
 * - Initializes the IFX I2C protocol stack.
 * - Re-Initializes and negotiates the frame size and bit rate with the OPTIGA.
 *   The values remain same as that in previous #optiga_comms_open().
 *
 * \pre
 * - Communication channel must be established with OPTIGA.
 *
 * \note
 * - This function clears the saved context.
 *
 * \param[in,out] p_ctx                     Valid instance of #optiga_comms_t created using #optiga_comms_create
 * \param[in,out] reset_type                Type of reset
 *                                          - For COLD and WARM reset type: If the GPIO(VDD and/or reset) pins are not configured,<br>
 *                                            the API continues without returning error status<br>
 *
 * \retval        #OPTIGA_COMMS_SUCCESS
 * \retval        #OPTIGA_COMMS_ERROR
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_comms_reset(optiga_comms_t * p_ctx, uint8_t reset_type);

/**
 * \brief Sends and receives the APDU.
 *
 * \details
 * Sends a command to OPTIGA and receives a response.
 * - Transmit data(Command) to OPTIGA.
 * - Receive data(Response) from OPTIGA.
 *
 * \pre
 * - Communication channel must be established with OPTIGA
 *
 * \note
 * - The following parameters in #optiga_comms_t must be initialized with appropriate values <br>
 *      - The <b>comms_ctx</b> must be initialized with a valid #ifx_i2c_context_t<br>
 *      - The <b>upper_layer_event_handler</b> parameter must be properly initialized,
 *          if it is different from that in #optiga_comms_open().
 *          This is invoked when optiga_comms_transceive is asynchronously completed.<br>
 *      - The <b>upper_layer_ctx</b> must be properly initialized,
 *          if it is different from that in #optiga_comms_open().
 *      - <b>protection_level</b> : Used for secure communication.The value of the parameter is not modified by the IFX-I2C protocol stack.
 *      - The values for <b>protection_level</b> must be one of the below
 *        - #OPTIGA_COMMS_NO_PROTECTION : Command and response is unprotected
 *        - #OPTIGA_COMMS_COMMAND_PROTECTION : Command is protected and response is unprotected
 *        - #OPTIGA_COMMS_RESPONSE_PROTECTION : Command is unprotected and response is protected
 *        - #OPTIGA_COMMS_FULL_PROTECTION : Both command and response is protected
 *        - To re-establish secure channel, bitwise-OR protection_level with #OPTIGA_COMMS_RE_ESTABLISH
 *   - If Presentation Layer is enabled,
 *       - Additional buffer size is required to handle encryption and decryption functionality.
 *   - The total buffer size with the additional presentation layer overhead is explained in the figure below. Please provide the lengths and data buffers as specified below.
 *       - The transmit and receive data buffers need additional overhead of #OPTIGA_COMMS_PRL_OVERHEAD bytes along with Tx/Rx data(payload) size.
 *       - Provide the command data, starting from the #OPTIGA_COMMS_DATA_OFFSET location in the transmit buffer.
 *       - In the receive buffer, the response (received data) from the OPTIGA will be stored from the #OPTIGA_COMMS_DATA_OFFSET location. Read the response from this offset.
 *       - Provide the length of command data (tx_data_length) with the payload length excluding the additional overhead.
 *       - Provide the length of response data (p_rx_data_length) in order to copy the response to the buffer provided.
 *
 *
 * \image html optiga_comms_trancv_buffer.png "optiga_comms_transceive()" width=0cm
 *
 * - The actual number of bytes received is stored in p_rx_buffer_len. In case of error,p_rx_buffer_len is set to 0.<br>
 * - If the size of p_rx_data is zero or insufficient to copy the response bytes, #OPTIGA_COMMS_ERROR_STACK_MEMORY error is returned.
 * - If establishing a secure channel fails, #OPTIGA_COMMS_ERROR_HANDSHAKE is returned.
 * - If #OPTIGA_COMMS_ERROR_SESSION is returned, a new session must be established.
 * - If presentation layer is enabled and the command data protection is selected,
 *   the input data provided will be modified because the data will be encrypted in the same input buffer.
 *   Therefore, if required by the user a copy of the input data must be preserved by the caller of this API.
 *
 *
 *
 * \param[in,out] p_ctx                              Valid instance of #optiga_comms_t created using #optiga_comms_create
 * \param[in]     p_tx_data                          Pointer to the transmit data buffer
 * \param[in]     tx_data_length                     Length of the transmit data buffer
 * \param[in,out] p_rx_data                          Pointer to the receive data buffer
 * \param[in,out] p_rx_data_len                      Pointer to the length of the receive data buffer
 *
 * \retval        #OPTIGA_COMMS_SUCCESS
 * \retval        #OPTIGA_COMMS_ERROR
 * \retval        #OPTIGA_COMMS_ERROR_STACK_MEMORY
 * \retval        #OPTIGA_COMMS_ERROR_HANDSHAKE
 * \retval        #OPTIGA_COMMS_ERROR_SESSION
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_comms_transceive(optiga_comms_t * p_ctx,
                                                            const uint8_t * p_tx_data,
                                                            uint16_t tx_data_length,
                                                            uint8_t * p_rx_data,
                                                            uint16_t * p_rx_data_len);

/**
 * \brief Closes the communication channel with OPTIGA.
 *
 * \details
 * Closes the communication with OPTIGA.
 * - De-Initializes the OPTIGA and closes the communication channel.
 * - Power downs the OPTIGA.
 *
 * \pre
 * - None
 *
 * \note
 * - The #optiga_comms_t comms_ctx must be initialized with a valid #ifx_i2c_context<br>
 * - <b>manage_contex_operation</b> : Used for managing session context and the values must be as defined below. The value of the parameter is not modified by the IFX-I2C protocol stack.
 *   - #OPTIGA_COMMS_SESSION_CONTEXT_SAVE : save active secure session.
 *   - #OPTIGA_COMMS_SESSION_CONTEXT_NONE : for no manage context operation.
 *
 * \param[in,out] p_ctx                   Valid instance of #optiga_comms_t created using #optiga_comms_create
 *
 * \retval        #OPTIGA_COMMS_SUCCESS
 * \retval        #OPTIGA_COMMS_ERROR
 */
LIBRARY_EXPORTS optiga_lib_status_t optiga_comms_close(optiga_comms_t * p_ctx);



#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_COMMS_H_*/

/**
* @}
*/

