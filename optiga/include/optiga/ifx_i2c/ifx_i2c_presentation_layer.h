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
* \file ifx_i2c_presentation_layer.h
*
* \brief   Module for the presentation layer of the Infineon I2C Protocol Stack library.
*
* \ingroup  grIFXI2C
*
* @{
*/


#ifndef _IFX_I2C_PRESENTATION_LAYER_H_
#define _IFX_I2C_PRESENTATION_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ifx_i2c_config.h"

/**
 * \brief Function for initializing the module.
 *
 * \details
 * Function initializes and enables the module
 * - It registers an event handler to receive events from this module.
 *
 * \pre
 * - This function must be called before using the module.
 *
 * \note
 * - None
 *
 * \param[in,out] p_ctx                    Pointer to ifx i2c context.
 * \param[in]     handler                  Function pointer to the event handler of the upper layer.
 *
 * \retval        IFX_I2C_STACK_SUCCESS    If initialization was successful.
 * \retval        IFX_I2C_STACK_ERROR      If the module is already initialized.
 */

optiga_lib_status_t ifx_i2c_prl_init(ifx_i2c_context_t * p_ctx,
                                     ifx_i2c_event_handler_t handler);

/**
 * \brief Function to transmit and receive a packet.
 *
 * \details
 * Asynchronous function to send and receive protected/unprotected packet
 * - The function returns immediately.
 * - One of the following events is propagated to the event handler registered with #ifx_i2c_prl_init
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in,out] p_ctx                      Pointer to ifx i2c context.
 * \param[in]     p_tx_data                  Pointer to the buffer containing the data to be transmitted.
 * \param[in]     tx_data_len                Transmit data length.
 * \param[in]     p_rx_data                  Pointer to the buffer to store the data received.
 * \param[in]     p_rx_data_len              Pointer to a variable to store the received data length
 *
 * \retval        IFX_I2C_STACK_SUCCESS      If function was successful.
 * \retval        IFX_I2C_HANDSHAKE_ERROR    If establishing a secure channel fails.
 * \retval        IFX_I2C_SESSION_ERROR      If an established secure channel is closed.
 * \retval        IFX_I2C_STACK_ERROR        If the module is busy.
 */
optiga_lib_status_t ifx_i2c_prl_transceive(ifx_i2c_context_t * p_ctx,
                                           uint8_t * p_tx_data,
                                           uint16_t tx_data_len,
                                           uint8_t * p_rx_data,
                                           uint16_t * p_rx_data_len);



/**
 * \brief Function for closing the module.
 *
 * \details
 * Function closes the module and registers an event handler to receive events from this module.
 *
 * \pre
 * - This function must be called before using the module.
 *
 * \note
 * - None
 *
 * \param[in,out] p_ctx                  Pointer to ifx i2c context.
 * \param[in]     handler                Function pointer to the event handler of the upper layer.
 *
 * \retval        IFX_I2C_STACK_SUCCESS  If initialization was successful.
 * \retval        IFX_I2C_STACK_ERROR    If the module is already initialized.
 */
optiga_lib_status_t ifx_i2c_prl_close(ifx_i2c_context_t * p_ctx,
                                      ifx_i2c_event_handler_t handler);
 
#ifdef __cplusplus
}
#endif
#endif /* _IFX_I2C_TRANSPORT_LAYER_H_ */

/**
 * @}
 */
