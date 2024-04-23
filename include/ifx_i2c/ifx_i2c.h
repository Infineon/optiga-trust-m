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
* \file ifx_i2c.h
*
* \brief   This file defines the API prototype for IFX I2C protocol v2.00 wrapper.
*
* \ingroup  grIFXI2C
*
* @{
*/


#ifndef _IFXI2C_H_
#define _IFXI2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_lib_types.h"
#include "ifx_i2c_config.h"

/** @brief IFX I2C Reset types */
typedef enum ifx_i2c_reset_type
{
    /// Cold reset. Both reset pin and VDD pin are toggled low and then high
    IFX_I2C_COLD_RESET = 0U,
    /// Soft reset. 0x0000 is written to IFX-I2C Soft reset register
    IFX_I2C_SOFT_RESET = 1U,
    /// Warm reset. Only reset pin is toggled low and then high
    IFX_I2C_WARM_RESET = 2U
} ifx_i2c_reset_type_t;

/**
 * \brief Initializes the IFX I2C protocol stack for a given context.
 *
 * <br>
 * <br>
 * \image html ifx_i2c_open.png "ifx_i2c_open()" width=20cm
 *
 * \details
 * Initializes the IFX I2C protocol stack for the given context.
 * - Performs a reset sequence.<br>
 * - Initializes the I2C slave device.<br>
 * - Initializes the IFX I2C protocol stack and registers the event callbacks.
 * - Negotiates the frame size and bit rate with the I2C slave.
 *
 * \pre
 * - None
 *
 * \note
 * - The values of registers MAX_SCL_FREQU and DATA_REG_LEN, read from slave are not validated.
 * - At present, only single instance of #ifx_i2c_context_t is supported.
 * - The following parameters in #ifx_i2c_context_t must be initialized with appropriate values <br>
 *   - <b>slave address</b> : Address of I2C slave
 *   - <b>frame_size</b> : Frame size in bytes. Refer #IFX_I2C_FRAME_SIZE for more details.<br>
 *              - It is recommended not to use a value greater than the slave's frame size.
 *              - The user specified frame size is written to I2C slave's frame size register.
 *                The frame size register is read back from I2C slave.
 *                This frame value is used by the IFX I2C protocol even if it is not equal to the user specified value.
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
 *   - <b>manage_contex_operation</b> : Used for manage context.The value of the parameter is not modified by the IFX I2C protocol stack.
 *     - The values for <b>manage_contex_operation</b> must be one of the below.
 *       - #IFX_I2C_SESSION_CONTEXT_RESTORE : restore the saved secure session.
 *       - #IFX_I2C_SESSION_CONTEXT_NONE : for no manage context operation. 
 *
 * \param[in,out]  p_ctx                    Pointer to #ifx_i2c_context_t, must not be NULL
 *
 * \retval         #IFX_I2C_STACK_SUCCESS
 * \retval         #IFX_I2C_STACK_ERROR
 */optiga_lib_status_t ifx_i2c_open(ifx_i2c_context_t * p_ctx);

/**
 * \brief   Resets the I2C slave.
 *
 * <br>
 * <br>
 * \image html ifx_i2c_reset.png "ifx_i2c_reset()" width=20cm
 *
 * \details
 * Resets the I2C slave and initializes the IFX I2C protocol stack for the given context.
 * - Resets the I2C slave.
 * - Initializes the IFX I2C protocol stack.
 * - Re-Initializes and negotiates the frame size and bit rate with the I2C slave.
 *   The values remain same as that in previous #ifx_i2c_open().
 *
 * \pre
 * - IFX I2C protocol stack must be initialized.
 *
 * \note
 * - For COLD and WARM reset type: If the GPIO(VDD and/or reset) pins are not configured,<br>
 *   the API continues without any failure return status<br>
 *
 * \param[in,out] p_ctx                  Pointer to #ifx_i2c_context_t, must not be NULL
 * \param[in,out] reset_type             type of reset
 *
 * \retval        #IFX_I2C_STACK_SUCCESS
 * \retval        #IFX_I2C_STACK_ERROR
 */
optiga_lib_status_t ifx_i2c_reset(ifx_i2c_context_t * p_ctx,
                                  ifx_i2c_reset_type_t reset_type);

/**
 * \brief   Sends a command and receives a response for the command.
 *
 * <br>
 * \image html ifx_i2c_transceive.png "ifx_i2c_transceive()" width=20cm
 *
 * \details
 * Sends a command and receives a response for the command.<br>
 * - Transmit data(Command) to I2C slave.
 * - Receive data(Response) from I2C slave.
 *
 * \pre
 * - IFX I2C protocol stack must be initialized.
 *
 * \note
 * - The actual number of bytes received is stored in p_rx_buffer_len. In case of error, p_rx_buffer_len is set to 0.<br>
 * - The p_rx_buffer must be large enough to accommodate the response data from the slave.
 *   The p_rx_buffer_len parameter must specify the length of the receive buffer correctly.
 *   If the size of p_rx_data is zero or insufficient to copy the response bytes then #IFX_I2C_STACK_MEM_ERROR error is returned.
 * - If establishing a secure channel fails, #IFX_I2C_HANDSHAKE_ERROR is returned.
 * - If #IFX_I2C_SESSION_ERROR is returned, a new session must be established.
 * - If presentation layer is enabled and the command data protection is selected,
 *   the input data provided will be modified because the data will be encrypted in the same input buffer.
 *   Hence a copy of the input data must be preserved by the caller of this API, if needed.
 * - The following parameters in #ifx_i2c_context_t must be initialized with appropriate values <br>
 *   - <b>upper_layer_event_handler</b> : Upper layer event handler, if it is different from that in #ifx_i2c_open().
 *     This is invoked when #ifx_i2c_transceive is asynchronously completed.
 *   - <b>upper_layer_ctx</b> : Context of upper layer, if it is different from that in #ifx_i2c_open.
 *   - <b>protection_level</b> : Used for secure communication.The value of the parameter is not modified by the IFX I2C protocol stack.
 *     - The values for <b>protection_level</b> must be one of the below
 *       - #NO_PROTECTION : data from master and slave is unprotected
 *       - #MASTER_PROTECTION : data from master is protected and data from slave is unprotected
 *       - #SLAVE_PROTECTION : data from master is unprotected and data from is slave protected
 *       - #FULL_PROTECTION : data from master and slave protected
 *       - To re-establish secure channel, bitwise-OR protection_level with #RE_ESTABLISH
 *   - If Presentation Layer is enabled,
 *       - Additional buffer size is required to handle encryption and decryption functionality.
 *   - The total buffer size with the additional presentation layer overhead is explained in the figure below. Please provide the lengths and data buffers as specified below.
 *       - The transmit and receive data buffers need additional overhead of #IFX_I2C_PRL_OVERHEAD_SIZE bytes along with Tx/Rx data(payload) size.
 *       - Provide the command data, starting from the #IFX_I2C_DATA_OFFSET location in the transmit buffer.
 *       - In the receive buffer, the response (received data) from the OPTIGA will be stored from the #IFX_I2C_DATA_OFFSET location. Read the response from this offset.
 *       - Provide the length of command data (tx_data_length) with the payload length excluding the additional overhead.
 *       - Provide the length of response data (p_rx_buffer_len) in-order to copy the response to the buffer provided.
 *   - If the master sequence number or expected slave sequence for next payload transmission crosses 0xFFFFFFF0, 
 *     master renegotiate a new secure channel and then sends the payload to slave.
 * \image html ifx_i2c_trancv_buffer.png "ifx_i2c_transceive()" width=0cm
 *
 * \param[in,out] p_ctx                     Pointer to #ifx_i2c_context_t, must not be NULL
 * \param[in]     p_tx_data                 Pointer to the write transmit buffer
 * \param[in]     tx_data_length            Length of the write data buffer
 * \param[in,out] p_rx_buffer               Pointer to the receive data buffer
 * \param[in,out] p_rx_buffer_len           Pointer to the length of the receive data buffer
 *
 * \retval        #IFX_I2C_STACK_SUCCESS
 * \retval        #IFX_I2C_STACK_ERROR
 * \retval        #IFX_I2C_STACK_MEM_ERROR
 * \retval        #IFX_I2C_HANDSHAKE_ERROR
 * \retval        #IFX_I2C_SESSION_ERROR
 */
optiga_lib_status_t ifx_i2c_transceive(ifx_i2c_context_t * p_ctx,
                                     const uint8_t * p_tx_data,
                                     uint16_t tx_data_length,
                                     uint8_t * p_rx_buffer,
                                     uint16_t* p_rx_buffer_len);

/**
 * \brief   Closes the IFX I2C protocol stack for a given context.
 *
 * <br>
 * <br>
 * \image html ifx_i2c_close.png "ifx_i2c_close()" width=20cm
 *
 * \details
 * Closes the IFX I2C protocol stack for a given context.
 * - De-Initializes the I2C slave device.
 * - Power downs the I2C slave.
 *
 * \pre
 * - None
 *
 * \note
 * - <b>manage_contex_operation</b> : Used for manage context.The value of the parameter is not modified by the IFX I2C protocol stack.
 *   - The values for <b>manage_contex_operation</b> must be one of the below
 *     - #IFX_I2C_SESSION_CONTEXT_SAVE : save active secure session.
 *     - #IFX_I2C_SESSION_CONTEXT_NONE : for no manage context operation.
 *
 * \param[in,out] p_ctx                Pointer to #ifx_i2c_context_t, must not be NULL
 *
 * \retval        #IFX_I2C_STACK_SUCCESS
 */
optiga_lib_status_t ifx_i2c_close(ifx_i2c_context_t * p_ctx);

/**
 * \brief   Sets the slave address of the target device.
 *
 * \details
 * Writes new I2C slave Address to the target device.
 *  - This API is implemented in synchronous mode.
 *  - If the write fails due to the following reasons, this API repeats the write for PL_POLLING_MAX_CNT times
 *    with a fixed interval of #PL_POLLING_INVERVAL_US microseconds and exits with respective return status.
 *    - I2C bus is in busy state, returns #IFX_I2C_STACK_BUSY
 *    - No-acknowledge(NACK) received from slave, returns #IFX_I2C_STACK_ERROR
 *    - I2C errors, returns #IFX_I2C_STACK_ERROR
 *  - Only bits [6:0] from  parameter "slave_address" are considered as slave address. Hence the bit 7 is ignored.
 *  - Slave address validation is not done in the implementation. Provide a valid slave address as input.
 *
 * \pre
 * - IFX I2C protocol stack must be initialized.
 *
 * \note
 * - If persistent mode is selected, the IFX I2C context slave address will be over-written with the new slave address.
 *   Even after IFX I2C open/reset, all future executions will use the new slave address.<br>
 * - If volatile mode is selected, the pal_i2c_context slave address will be over-written with the new slave address.
 *   This persists only till the next ifx_i2c open/reset is called.
 *
 * \param[in,out] p_ctx                    Pointer to #ifx_i2c_context_t
 * \param[in]     slave_address            Holds new slave address[7 Bit] to be set.
 * \param[in]     persistent               0 - To set the Slave address until next reset.<br>
 *                                         Non-zero - To set the slave address to persistent memory.
 *
 * \retval        #IFX_I2C_STACK_SUCCESS
 * \retval        #IFX_I2C_STACK_ERROR
 */
optiga_lib_status_t ifx_i2c_set_slave_address(ifx_i2c_context_t * p_ctx,
                                              uint8_t slave_address,
                                              uint8_t persistent);

#ifdef __cplusplus
}
#endif
#endif /* _IFXI2C_H_ */
/**
 * @}
 **/

