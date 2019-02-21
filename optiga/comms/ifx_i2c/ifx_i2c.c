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
* \file ifx_i2c.c
*
* \brief   This file implements the wrapper API Layer for IFX I2C protocol v2.00.
*
* \ingroup  grIFXI2C
*
* @{
*/

#include "optiga/ifx_i2c/ifx_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c_presentation_layer.h"
#include "optiga/pal/pal_os_event.h"

#ifndef OPTIGA_COMMS_SHIELDED_CONNECTION
#include "optiga/ifx_i2c/ifx_i2c_transport_layer.h"
#endif

/// @cond hidden

// IFX I2C states
#define IFX_I2C_STATE_UNINIT         (0x01)
#define IFX_I2C_STATE_IDLE           (0x02)
#define IFX_I2C_STATUS_BUSY          (0x03)
#define IFX_I2C_STATUS_NOT_BUSY      (0x04)

/// IFX I2C Reset states
#define IFX_I2C_STATE_RESET_PIN_LOW        (0xB1)
#define IFX_I2C_STATE_RESET_PIN_HIGH       (0xB2)
#define IFX_I2C_STATE_RESET_INIT           (0xB3)

void ifx_i2c_tl_event_handler(ifx_i2c_context_t * p_ctx,
                              optiga_lib_status_t event,
                              const uint8_t * p_data,
                              uint16_t data_len);

void ifx_i2c_prl_close_event_handler(ifx_i2c_context_t * p_ctx,
                                     optiga_lib_status_t event,
                                     const uint8_t * p_data,
                                     uint16_t data_len);

_STATIC_H optiga_lib_status_t ifx_i2c_init(ifx_i2c_context_t * p_ifx_i2c_context);

//lint --e{526} suppress "This API is defined in ifx_i2c_physical_layer.c file. As it is a low level API, it is not exposed in header file"
extern optiga_lib_status_t ifx_i2c_pl_write_slave_address(ifx_i2c_context_t * p_ctx,
                                                          uint8_t slave_address,
                                                          uint8_t storage_type);
/// @endcond

optiga_lib_status_t ifx_i2c_open(ifx_i2c_context_t * p_ctx)
{
    optiga_lib_status_t api_status = (int32_t)IFX_I2C_STACK_ERROR;

    //If api status is not busy, proceed
    if ((IFX_I2C_STATUS_BUSY != p_ctx->status))
    {
        p_ctx->p_pal_i2c_ctx->p_upper_layer_ctx = p_ctx;
        p_ctx->reset_type = (uint8_t)IFX_I2C_COLD_RESET;
        p_ctx->reset_state = IFX_I2C_STATE_RESET_PIN_LOW;
        p_ctx->do_pal_init = TRUE;
        p_ctx->state = IFX_I2C_STATE_UNINIT;

        api_status = ifx_i2c_init(p_ctx);
        if (IFX_I2C_STACK_SUCCESS == api_status)
        {
            p_ctx->status = IFX_I2C_STATUS_BUSY;
        }
    }

    return (api_status);
}


optiga_lib_status_t ifx_i2c_reset(ifx_i2c_context_t * p_ctx,
                                  ifx_i2c_reset_type_t reset_type)
{
    optiga_lib_status_t api_status = (int32_t)IFX_I2C_STACK_ERROR;

    // Proceed, if not busy and in idle state
    if ((IFX_I2C_STATE_IDLE == p_ctx->state) && (IFX_I2C_STATUS_BUSY != p_ctx->status))
    {
        p_ctx->reset_type = (uint8_t)reset_type;
        p_ctx->reset_state = IFX_I2C_STATE_RESET_PIN_LOW;
        p_ctx->do_pal_init = FALSE;

        api_status = ifx_i2c_init(p_ctx);
        if (IFX_I2C_STACK_SUCCESS == api_status)
        {
            p_ctx->status = IFX_I2C_STATUS_BUSY;
        }
    }
    return (api_status);
}


optiga_lib_status_t ifx_i2c_transceive(ifx_i2c_context_t * p_ctx,
                                       const uint8_t * p_tx_data,
                                       uint16_t tx_data_length,
                                       uint8_t * p_rx_buffer,
                                       uint16_t * p_rx_buffer_len)
{
    optiga_lib_status_t api_status = (int32_t)IFX_I2C_STACK_ERROR;
    // Proceed, if not busy and in idle state
    if ((IFX_I2C_STATE_IDLE == p_ctx->state) && (IFX_I2C_STATUS_BUSY != p_ctx->status))
    {
        p_ctx->p_upper_layer_rx_buffer = p_rx_buffer;
        p_ctx->p_upper_layer_rx_buffer_len = p_rx_buffer_len;
#ifndef OPTIGA_COMMS_SHIELDED_CONNECTION
        api_status = ifx_i2c_tl_transceive(p_ctx,
                                           (uint8_t * )p_tx_data,
                                           (tx_data_length),
                                           (uint8_t * )p_rx_buffer,
                                           p_rx_buffer_len);
        if (IFX_I2C_STACK_SUCCESS == api_status)
        {
            p_ctx->status = IFX_I2C_STATUS_BUSY;
        }
#else
        api_status = ifx_i2c_prl_transceive(p_ctx,
                                         (uint8_t * )p_tx_data,
                                         tx_data_length,
                                         (uint8_t * )p_rx_buffer,
                                         p_rx_buffer_len);
#endif
        if ((IFX_I2C_STACK_SUCCESS == api_status) && (IFX_I2C_STACK_SUCCESS == p_ctx->close_state))
        {
            p_ctx->status = IFX_I2C_STATUS_BUSY;
        }
    }
    return (api_status);
}


optiga_lib_status_t ifx_i2c_close(ifx_i2c_context_t * p_ctx)
{
    optiga_lib_status_t api_status = (int32_t)IFX_I2C_STACK_ERROR;
    // Proceed, if not busy and in idle state
    if (IFX_I2C_STATUS_BUSY != p_ctx->status)
    {
        api_status = IFX_I2C_STACK_SUCCESS;

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        p_ctx->close_state = IFX_I2C_STACK_ERROR;
        api_status = ifx_i2c_prl_close(p_ctx, ifx_i2c_prl_close_event_handler);
        p_ctx->state = IFX_I2C_STATE_UNINIT;
        if (IFX_I2C_STACK_ERROR == api_status)
        {
            //lint --e{534} suppress "Error handling is not required so return value is not checked"
            pal_i2c_deinit(p_ctx->p_pal_i2c_ctx);
            // Also power off the device
            pal_gpio_set_low(p_ctx->p_slave_vdd_pin);
            pal_gpio_set_low(p_ctx->p_slave_reset_pin);
            p_ctx->state = IFX_I2C_STATE_UNINIT;
            p_ctx->status = IFX_I2C_STATUS_NOT_BUSY;
        }
#else
        ifx_i2c_tl_event_handler(p_ctx, IFX_I2C_STACK_SUCCESS, NULL, 0);
        //lint --e{534} suppress "Error handling is not required so return value is not checked"
        // Close I2C master
        pal_i2c_deinit(p_ctx->p_pal_i2c_ctx);
        // Also power off the device
        pal_gpio_set_low(p_ctx->p_slave_vdd_pin);
        pal_gpio_set_low(p_ctx->p_slave_reset_pin);
        p_ctx->state = IFX_I2C_STATE_UNINIT;
        p_ctx->status = IFX_I2C_STATUS_NOT_BUSY;
#endif

    }
    return (api_status);
}


optiga_lib_status_t ifx_i2c_set_slave_address(ifx_i2c_context_t * p_ctx,
                                              uint8_t slave_address,
                                              uint8_t persistent)
{
    optiga_lib_status_t api_status = (int32_t)IFX_I2C_STACK_ERROR;

    if ((IFX_I2C_STATE_IDLE == p_ctx->state))
    {
        p_ctx->p_pal_i2c_ctx->p_upper_layer_ctx = p_ctx;

        api_status = ifx_i2c_pl_write_slave_address(p_ctx, slave_address, persistent);
    }

    return (api_status);
}

/// @cond hidden
//lint --e{715} suppress "The arguments p_data and data_len is not used in this function 
//                        but as per the function signature those 2 parameter should be passed"
void ifx_i2c_tl_event_handler(ifx_i2c_context_t * p_ctx,
                              optiga_lib_status_t event,
                              const uint8_t * p_data,
                              uint16_t data_len)
{
    // If there is no upper layer handler, don't do anything and return
    if (NULL != p_ctx->upper_layer_event_handler)
    {
        p_ctx->upper_layer_event_handler(p_ctx->p_upper_layer_ctx, event);
    }
    p_ctx->close_state = event;
    p_ctx->status = IFX_I2C_STATUS_NOT_BUSY;
    switch (p_ctx->state)
    {
        case IFX_I2C_STATE_UNINIT:
        {
            if (IFX_I2C_STACK_SUCCESS == event)
            {
                p_ctx->state = IFX_I2C_STATE_IDLE;
            }
            break;
        }
        default:
            break;
    }
}

/// @cond hidden
//lint --e{715} suppress "The arguments p_data and data_len is not used in this function 
//                        but as per the function signature those 2 parameter should be passed"
void ifx_i2c_prl_close_event_handler(ifx_i2c_context_t * p_ctx,
                                     optiga_lib_status_t event,
                                     const uint8_t * p_data,
                                     uint16_t data_len)
{
    // If there is no upper layer handler, don't do anything and return
    if (NULL != p_ctx->upper_layer_event_handler)
    {
        p_ctx->upper_layer_event_handler(p_ctx->p_upper_layer_ctx, event);
    }
    p_ctx->status = IFX_I2C_STATUS_NOT_BUSY;
    switch (p_ctx->state)
    {
        case IFX_I2C_STATE_UNINIT:
        {
            //lint --e{534} suppress "Error handling is not required so return value is not checked"
            pal_i2c_deinit(p_ctx->p_pal_i2c_ctx);
            // Also power off the device
            pal_gpio_set_low(p_ctx->p_slave_vdd_pin);
            pal_gpio_set_low(p_ctx->p_slave_reset_pin);
            p_ctx->state = IFX_I2C_STATE_UNINIT;
            p_ctx->status = IFX_I2C_STATUS_NOT_BUSY;
            break;
        }
        default:
            break;
    }
}

_STATIC_H optiga_lib_status_t ifx_i2c_init(ifx_i2c_context_t * p_ifx_i2c_context)
{
    optiga_lib_status_t api_status = IFX_I2C_STACK_ERROR;

    if (((uint8_t)IFX_I2C_WARM_RESET == p_ifx_i2c_context->reset_type) ||
        ((uint8_t)IFX_I2C_COLD_RESET == p_ifx_i2c_context->reset_type))
    {
        switch (p_ifx_i2c_context->reset_state)
        {
            case IFX_I2C_STATE_RESET_PIN_LOW:
            {
                // Setting the Vdd & Reset pin to low
                if ((uint8_t)IFX_I2C_COLD_RESET == p_ifx_i2c_context->reset_type)
                {
                    pal_gpio_set_low(p_ifx_i2c_context->p_slave_vdd_pin);
                }
                pal_gpio_set_low(p_ifx_i2c_context->p_slave_reset_pin);
                p_ifx_i2c_context->reset_state = IFX_I2C_STATE_RESET_PIN_HIGH;
                pal_os_event_register_callback_oneshot(p_ifx_i2c_context->pal_os_event_ctx,
                                                       (register_callback)ifx_i2c_init,
                                                       (void * )p_ifx_i2c_context,
                                                       RESET_LOW_TIME_MSEC);
                api_status = IFX_I2C_STACK_SUCCESS;
                break;
            }
            case IFX_I2C_STATE_RESET_PIN_HIGH:
            {
                // Setting the Vdd & Reset pin to high
                if ((uint8_t)IFX_I2C_COLD_RESET == p_ifx_i2c_context->reset_type)
                {
                    pal_gpio_set_high(p_ifx_i2c_context->p_slave_vdd_pin);
                }
                pal_gpio_set_high(p_ifx_i2c_context->p_slave_reset_pin);
                p_ifx_i2c_context->reset_state = IFX_I2C_STATE_RESET_INIT;
                pal_os_event_register_callback_oneshot(p_ifx_i2c_context->pal_os_event_ctx,
                                                       (register_callback)ifx_i2c_init,
                                                       (void * )p_ifx_i2c_context,
                                                       STARTUP_TIME_MSEC);
                api_status = IFX_I2C_STACK_SUCCESS;
                break;
            }
            case IFX_I2C_STATE_RESET_INIT:
            {
                //Frequency and frame size negotiation
#ifndef OPTIGA_COMMS_SHIELDED_CONNECTION
                api_status = ifx_i2c_tl_init(p_ifx_i2c_context, ifx_i2c_tl_event_handler);
#else
                api_status = ifx_i2c_prl_init(p_ifx_i2c_context, ifx_i2c_tl_event_handler);
#endif
                break;
            }
            default:
                break;
        }
    }
    //soft reset
    else
    {
        p_ifx_i2c_context->pl.request_soft_reset = (uint8_t)TRUE;    //Soft reset
#ifndef OPTIGA_COMMS_SHIELDED_CONNECTION
        api_status = ifx_i2c_tl_init(p_ifx_i2c_context, ifx_i2c_tl_event_handler);
#else
        api_status = ifx_i2c_prl_init(p_ifx_i2c_context, ifx_i2c_tl_event_handler);
#endif
    }

    return (api_status);
}
/// @endcond
/**
* @}
*/
