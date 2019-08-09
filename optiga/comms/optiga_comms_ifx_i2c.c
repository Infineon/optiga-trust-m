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
* \file optiga_comms_ifx_i2c.c
*
* \brief   This file implements optiga comms abstraction layer for IFX I2C Protocol.
*
* \ingroup  grOptigaComms
*
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/comms/optiga_comms.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/ifx_i2c/ifx_i2c.h"
#include "optiga/pal/pal_os_event.h"

/// @cond hidden

 /// Optiga comms is in use
 #define OPTIGA_COMMS_INUSE     (0x01)
 /// Optiga comms is free
 #define OPTIGA_COMMS_FREE      (0x00)

optiga_comms_t optiga_comms = {
                               (void *)&ifx_i2c_context_0,
                               NULL,
                               NULL,
                               0,
                               0,
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
                               0,
                               0,
                               0,
#endif
                               NULL};

_STATIC_H optiga_lib_status_t check_optiga_comms_state(optiga_comms_t *p_ctx);
_STATIC_H void ifx_i2c_event_handler(void* p_ctx, optiga_lib_status_t event);

optiga_comms_t * optiga_comms_create(callback_handler_t callback, void * context)
{
    optiga_comms_t * p_optiga_comms = NULL;

    do
    {
        p_optiga_comms = &optiga_comms;

        if (FALSE == p_optiga_comms->instance_init_state)
        {
            p_optiga_comms->upper_layer_handler = callback;
            p_optiga_comms->p_upper_layer_ctx = context;
            p_optiga_comms->instance_init_state = TRUE;
        }
    } while (FALSE);
    return (p_optiga_comms);
}

//lint --e{715} suppress "p_optiga_cmd is not used here as it is placeholder for future."
//lint --e{818} suppress "Not declared as pointer as nothing needs to be updated in the pointer."
void optiga_comms_destroy(optiga_comms_t * p_optiga_cmd)
{
}

optiga_lib_status_t optiga_comms_set_callback_handler(optiga_comms_t * p_optiga_comms, callback_handler_t handler)
{
    p_optiga_comms->upper_layer_handler = handler;
    return (0);
}

optiga_lib_status_t optiga_comms_set_callback_context(optiga_comms_t * p_optiga_comms, void * context)
{
    p_optiga_comms->p_upper_layer_ctx = context;
    return (0);
}


/// @endcond
/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/


optiga_lib_status_t optiga_comms_open(optiga_comms_t * p_ctx)
{
    optiga_lib_status_t status = OPTIGA_COMMS_ERROR;
    if (OPTIGA_COMMS_SUCCESS == check_optiga_comms_state(p_ctx))
    {
        if (PAL_STATUS_SUCCESS == pal_init())
        {
            ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->p_upper_layer_ctx = (void * )p_ctx;
            ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->upper_layer_event_handler = ifx_i2c_event_handler;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
            ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->manage_context_operation = p_ctx->manage_context_operation;
#endif
            ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->pal_os_event_ctx = p_ctx->p_pal_os_event_ctx;

            status = ifx_i2c_open((ifx_i2c_context_t * )(p_ctx->p_comms_ctx));
            if (IFX_I2C_STACK_SUCCESS != status)
            {
                p_ctx->state = OPTIGA_COMMS_FREE;
            }
        }
    }
    return (status);
}

optiga_lib_status_t optiga_comms_reset(optiga_comms_t * p_ctx, uint8_t reset_type)
{
    optiga_lib_status_t status = OPTIGA_COMMS_ERROR;
    if (OPTIGA_COMMS_SUCCESS == check_optiga_comms_state(p_ctx))
    {
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->p_upper_layer_ctx = (void * )p_ctx;
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->upper_layer_event_handler = ifx_i2c_event_handler;
        status = ifx_i2c_reset((ifx_i2c_context_t * )(p_ctx->p_comms_ctx), (ifx_i2c_reset_type_t)reset_type);
        if (IFX_I2C_STACK_SUCCESS != status)
        {
            p_ctx->state = OPTIGA_COMMS_FREE;
        }
    }
    return (status);
}


optiga_lib_status_t optiga_comms_transceive(optiga_comms_t * p_ctx,
                                            const uint8_t * p_tx_data,
                                            uint16_t tx_data_length,
                                            uint8_t * p_rx_data,
                                            uint16_t * p_rx_data_len)
{
    optiga_lib_status_t status = OPTIGA_COMMS_ERROR;
    if (OPTIGA_COMMS_SUCCESS == check_optiga_comms_state(p_ctx))
    {
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->p_upper_layer_ctx = (void * )p_ctx;
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->upper_layer_event_handler = ifx_i2c_event_handler;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->protection_level = p_ctx->protection_level;
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->protocol_version = p_ctx->protocol_version;
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->manage_context_operation = p_ctx->manage_context_operation;
#endif
        status = (ifx_i2c_transceive((ifx_i2c_context_t * )(p_ctx->p_comms_ctx),
                                     p_tx_data,
                                     tx_data_length,
                                     p_rx_data,
                                     p_rx_data_len));
        if (IFX_I2C_STACK_SUCCESS != status)
        {
            p_ctx->state = OPTIGA_COMMS_FREE;
        }
    }
    return (status);
}


optiga_lib_status_t optiga_comms_close(optiga_comms_t * p_ctx)
{
    optiga_lib_status_t status = OPTIGA_COMMS_ERROR;
    if (OPTIGA_COMMS_SUCCESS == check_optiga_comms_state(p_ctx))
    {
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->p_upper_layer_ctx = (void * )p_ctx;
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->upper_layer_event_handler = ifx_i2c_event_handler;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        ((ifx_i2c_context_t * )(p_ctx->p_comms_ctx))->manage_context_operation = p_ctx->manage_context_operation;
#endif
        status = ifx_i2c_close((ifx_i2c_context_t * )(p_ctx->p_comms_ctx));
        if (IFX_I2C_STACK_SUCCESS != status)
        {
            p_ctx->state = OPTIGA_COMMS_FREE;
        }
    }
    return (status);
}

/// @cond hidden
_STATIC_H optiga_lib_status_t check_optiga_comms_state(optiga_comms_t * p_ctx)
{
    optiga_lib_status_t status = OPTIGA_COMMS_ERROR;
    if ((NULL != p_ctx) && (OPTIGA_COMMS_INUSE != p_ctx->state))
    {
        p_ctx->state = OPTIGA_COMMS_INUSE;
        status = OPTIGA_COMMS_SUCCESS;
    }
    return (status);
}

//lint --e{818} suppress "This is ignored as upper layer handler function prototype requires this argument"
_STATIC_H void ifx_i2c_event_handler(void * p_upper_layer_ctx, optiga_lib_status_t event)
{
    void * ctx = ((optiga_comms_t * )p_upper_layer_ctx)->p_upper_layer_ctx;
    ((optiga_comms_t * )p_upper_layer_ctx)->upper_layer_handler(ctx, event);
    ((optiga_comms_t * )p_upper_layer_ctx)->state = OPTIGA_COMMS_FREE;
}

/// @endcond
/**
* @}
*/
