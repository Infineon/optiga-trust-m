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
* \file pal_i2c.c
*
* \brief   This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_i2c.h"
#include "pal_psoc_i2c_mapping.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_scb_common.h"

#define PAL_I2C_MASTER_MAX_BITRATE  (400U)
#define PAL_I2C_MASTER_INTR_PRIO    (3U)
/// @cond hidden

_STATIC_H volatile uint32_t g_entry_count = 0;
_STATIC_H const pal_i2c_t * gp_pal_i2c_current_ctx;
_STATIC_H uint8_t g_pal_i2c_init_flag = 0;

//lint --e{715} suppress "This is implemented for overall completion of API"
_STATIC_H pal_status_t pal_i2c_acquire(const void * p_i2c_context)
{
    if (0 == g_entry_count)
    {
        g_entry_count++;
        if (1 == g_entry_count)
        {
            return PAL_STATUS_SUCCESS;
        }
    }
    return PAL_STATUS_FAILURE;
}

//lint --e{715} suppress "The unused p_i2c_context variable is kept for future enhancements"
_STATIC_H void pal_i2c_release(const void * p_i2c_context)
{
    g_entry_count = 0;
}

_STATIC_H void invoke_upper_layer_callback (const pal_i2c_t * p_pal_i2c_ctx,
                                        optiga_lib_status_t event)
{
    upper_layer_callback_t upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
    upper_layer_handler = (upper_layer_callback_t)p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx, event);

    //Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->p_upper_layer_ctx);
}

_STATIC_H void i2c_master_error_detected_callback(const pal_i2c_t * p_pal_i2c_ctx)
{
    cyhal_i2c_abort_async(((pal_psoc_i2c_t *)(p_pal_i2c_ctx->p_i2c_hw_config))->i2c_master_channel);
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_ERROR);
}

/* Defining master callback handler */
void i2c_master_event_handler(void *callback_arg, cyhal_i2c_event_t event)
{
    if (0UL != (CYHAL_I2C_MASTER_ERR_EVENT & event))
    {
        /* In case of error abort transfer */
        i2c_master_error_detected_callback(gp_pal_i2c_current_ctx);
    }
    /* Check write complete event */
    else if (0UL != (CYHAL_I2C_MASTER_WR_CMPLT_EVENT & event))
    {
        /* Perform the required functions */
        invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
    }
    /* Check read complete event */
    else if (0UL != (CYHAL_I2C_MASTER_RD_CMPLT_EVENT & event))
    {
        /* Perform the required functions */
        invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
    }
}

pal_status_t pal_i2c_init(const pal_i2c_t * p_i2c_context)
{
    cy_rslt_t cy_hal_status;
    /* Define the I2C master configuration structure */
    cyhal_i2c_cfg_t i2c_master_config = {false,    // is slave?
                                         0,        // address of this resource; set to 0 for master
                                         400000    // bus frequency in Hz
                                        };
    
    do
    {
        if (g_pal_i2c_init_flag == 0)
        {
            // Init I2C driver
            cy_hal_status = cyhal_i2c_init(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                        ((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->sda_port_num,
                        ((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->sca_port_num,
                        NULL);

            if (CY_RSLT_SUCCESS != cy_hal_status)
            {
                break;
            }

            //Configure the I2C resource to be master
            cy_hal_status = cyhal_i2c_configure(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                                                            &i2c_master_config);
            if (CY_RSLT_SUCCESS != cy_hal_status)
            {
                break;
            }

            // Register i2c master callback
            cyhal_i2c_register_callback(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                          (cyhal_i2c_event_callback_t) i2c_master_event_handler,
                          NULL);

            // Enable interrupts for I2C master
            cyhal_i2c_enable_event(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                         (cyhal_i2c_event_t)(CYHAL_I2C_MASTER_WR_CMPLT_EVENT \
                         | CYHAL_I2C_MASTER_RD_CMPLT_EVENT \
                         | CYHAL_I2C_MASTER_ERR_EVENT),    \
                         PAL_I2C_MASTER_INTR_PRIO ,
                         true);
            g_pal_i2c_init_flag = 1;
        }
        else
        {
            cy_hal_status = (pal_status_t)PAL_STATUS_SUCCESS;
        }
    } while (FALSE);

    return (pal_status_t)cy_hal_status;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t * p_i2c_context)
{
    g_pal_i2c_init_flag = 0;
    cyhal_i2c_free(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel);
    return (PAL_STATUS_SUCCESS);
}

pal_status_t pal_i2c_write(const pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;

    //Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;
        //Invoke the low level i2c master driver API to write to the bus
        if (CY_RSLT_SUCCESS != cyhal_i2c_master_transfer_async(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                                                                p_i2c_context->slave_address,
                                                                p_data,
                                                                length,
                                                                NULL,
                                                                0))
        {
            //If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.
            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
            
            //Release I2C Bus
            pal_i2c_release((void * )p_i2c_context);
        }
        else
        {
            status = PAL_STATUS_SUCCESS;
        }
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return (status);
}

pal_status_t pal_i2c_read(const pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;

    //Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;

        //Invoke the low level i2c master driver API to read from the bus
        if (CY_RSLT_SUCCESS != cyhal_i2c_master_transfer_async(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel,
                                                                p_i2c_context->slave_address,
                                                                NULL,
                                                                0,
                                                                p_data,
                                                                length))
        {
            //If I2C Master fails to invoke the read operation, invoke upper layer event handler with error.
            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);

            //Release I2C Bus
            pal_i2c_release((void * )p_i2c_context);
        }
        else
        {
            status = PAL_STATUS_SUCCESS;
        }
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return (status);
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t * p_i2c_context, uint16_t bitrate)
{

    cyhal_i2c_t * i2cObj = (cyhal_i2c_t * )(((pal_psoc_i2c_t *)(p_i2c_context->p_i2c_hw_config))->i2c_master_channel);
    pal_status_t return_status = PAL_STATUS_FAILURE;
    optiga_lib_status_t event = PAL_I2C_EVENT_ERROR;
    uint32_t setDataRate;

    //Acquire the I2C bus before setting the bitrate
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
        // set the I2C master to its maximum supported value.
        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)
        {
            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
        }

        setDataRate = _cyhal_i2c_set_peri_divider(i2cObj->base,
                                                 i2cObj->resource.block_num,
                                                 &(i2cObj->clock),
                                                 (bitrate * 1000),
                                                 false);
        if (0 == setDataRate)
        {

             return_status = PAL_STATUS_FAILURE;
        }
        else
        {
             return_status = PAL_STATUS_SUCCESS;
             event = PAL_I2C_EVENT_SUCCESS;
        }
    }
    else
    {
        return_status = PAL_STATUS_I2C_BUSY;
        event = PAL_I2C_EVENT_BUSY;
    }

    if (0 != p_i2c_context->upper_layer_event_handler)
    {
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((callback_handler_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , event);
    }
    //Release I2C Bus if its acquired 
    if (PAL_STATUS_I2C_BUSY != return_status)
    {
        pal_i2c_release((void * )p_i2c_context);
    }

    return (return_status);
}

/**
* @}
*/
