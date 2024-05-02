/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_transport_layer.h
 *
 * \brief   This file defines the API prototype for transport layer of the Infineon I2C Protocol Stack library.
 *
 * \ingroup  grIFXI2C
 *
 * @{
 */

#ifndef _IFX_I2C_TRANSPORT_LAYER_H_
#define _IFX_I2C_TRANSPORT_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ifx_i2c_config.h"
#include "pal_os_timer.h"

/**
 * \brief Function for initializing the module.
 *
 * \details
 * Function initializes and enables the module and registers an event handler to receive events from this module.
 *
 * \pre
 * - This function must be called before using the module.
 *
 * \note
 * - None
 *
 * \param[in,out] p_ctx                   Pointer to ifx i2c context.
 * \param[in]     handler                 Function pointer to the event handler of the upper layer.
 *
 * \retval        IFX_I2C_STACK_SUCCESS   If initialization was successful.
 * \retval        IFX_I2C_STACK_ERROR     If the module is already initialized.
 */
optiga_lib_status_t ifx_i2c_tl_init(ifx_i2c_context_t *p_ctx, ifx_i2c_event_handler_t handler);

/**
 * \brief Function to transmit and receive a packet.
 *
 * \details
 * - The function returns immediately.
 * - One of the following events is propagated to the event handler registered with #ifx_i2c_tl_init
 *
 * \pre
 * - This function must be called before using the module.
 *
 * \note
 * - None
 *
 * \param[in,out] p_ctx                   Pointer to ifx i2c context.
 * \param[in]     p_packet                Buffer containing the packet header.
 * \param[in]     packet_len              Packet header length.
 * \param[in]     p_recv_packet           Buffer containing the packet payload.
 * \param[in]     p_recv_packet_len       Packet payload length.
 *
 * \retval        IFX_I2C_STACK_SUCCESS   If function was successful.
 * \retval        IFX_I2C_STACK_ERROR     If the module is busy.
 */
optiga_lib_status_t ifx_i2c_tl_transceive(
    ifx_i2c_context_t *p_ctx,
    uint8_t *p_packet,
    uint16_t packet_len,
    uint8_t *p_recv_packet,
    uint16_t *p_recv_packet_len
);

#ifdef __cplusplus
}
#endif
#endif /* _IFX_I2C_TRANSPORT_LAYER_H_ */

/**
 * @}
 */
