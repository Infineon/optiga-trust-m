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
* \file pal_i2c.c
*
* \brief   This file implements the platform abstraction layer(pal) APIs for I2C.
*
* \ingroup  grPAL
*
* @{
*/

/* OPTIGA includes */
#include "pal_zephyr.h"
#include "optiga/pal/pal_i2c.h"

/* Zephyr OS includes */
#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <logging/log.h>

LOG_MODULE_DECLARE(TRUST_M, CONFIG_LOG_DEFAULT_LEVEL);

/* Binary semaphore for lock/unlocking access to i2c bus, replaces pal_i2c_acquire/release */
K_SEM_DEFINE(i2c_bus_lock, I2C_BUS_ENTRY_INIT_VALUE , I2C_BUS_ENTRY_MAX_VALUE);

/// @cond hidden

_STATIC_H volatile uint32_t g_entry_count = 0;
_STATIC_H pal_i2c_t * gp_pal_i2c_current_ctx;

//lint --e{715} suppress "This is implemented for overall completion of API"
_STATIC_H pal_status_t __attribute__((unused)) pal_i2c_acquire(const void * p_i2c_context)
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
/// @endcond

void invoke_upper_layer_callback (const pal_i2c_t * p_pal_i2c_ctx, optiga_lib_status_t event)
{
    upper_layer_callback_t upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
    upper_layer_handler = (upper_layer_callback_t)p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx, event);

    //Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->p_upper_layer_ctx);
}

/// @cond hidden

// !!!OPTIGA_LIB_PORTING_REQUIRED
// The next 5 functions are required only in case you have interrupt based i2c implementation
void i2c_master_end_of_transmit_callback(void)
{
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

void i2c_master_end_of_receive_callback(void)
{
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

void i2c_master_error_detected_callback(void)
{
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_ERROR);
}

void i2c_master_nack_received_callback(void)
{
    i2c_master_error_detected_callback();
}

void i2c_master_arbitration_lost_callback(void)
{
    i2c_master_error_detected_callback();
}
/// @endcond

pal_status_t pal_i2c_init(const pal_i2c_t * p_i2c_context)
{
    pal_zephyr_i2c_t *p_i2c_ctx = (pal_zephyr_i2c_t*)(p_i2c_context->p_i2c_hw_config);
    p_i2c_ctx->p_i2c_dev = device_get_binding(I2C_PORT_NAME);
    
    if (p_i2c_ctx->p_i2c_dev == 0)
    {
        LOG_ERR("could not bind I2C-device");
        return PAL_STATUS_FAILURE;
    }

    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t * p_i2c_context)
{
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    int lock_result;
    int i2c_write_result;
    pal_status_t status = PAL_STATUS_FAILURE;

    pal_zephyr_i2c_t *p_i2c_ctx = (pal_zephyr_i2c_t*)(p_i2c_context->p_i2c_hw_config);

	if((p_i2c_context != NULL) && (p_i2c_ctx->p_i2c_dev == NULL))
	{
		pal_i2c_init(p_i2c_context);
	}

    // Acquire the I2C bus before accessing
    lock_result = k_sem_take(&i2c_bus_lock, K_NO_WAIT);

    if (lock_result == I2C_BUS_ENTRY_SUCCESS)
    {
        gp_pal_i2c_current_ctx = p_i2c_context;

        // Invoke the low level i2c master driver API to write to the bus

        i2c_write_result = i2c_write(p_i2c_ctx->p_i2c_dev, p_data, (u32_t)length, p_i2c_context->slave_address);
        //printk("[PAL]: I2C write status %d\n", i2c_write_result);
        
        if(i2c_write_result != 0)
        {
            //If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.
            
            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"

            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
        }
        else
        {
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
            status = PAL_STATUS_SUCCESS;
        }

        // Release I2C Bus
        k_sem_give(&i2c_bus_lock);
    }
    else
    {
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
        status = PAL_STATUS_I2C_BUSY;
    }
    return status;
}

pal_status_t pal_i2c_read(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    int lock_result;
    pal_status_t status = PAL_STATUS_FAILURE;
    
    pal_zephyr_i2c_t *p_i2c_ctx = (pal_zephyr_i2c_t*)(p_i2c_context->p_i2c_hw_config);

    // Acquire the I2C bus before accessing
    lock_result = k_sem_take(&i2c_bus_lock, K_NO_WAIT);

    if (lock_result == I2C_BUS_ENTRY_SUCCESS)
    {
        gp_pal_i2c_current_ctx = p_i2c_context;

        // Invoke the low level i2c master driver API to read from the bus
        if (i2c_read(p_i2c_ctx->p_i2c_dev, p_data, (u32_t)length, (u16_t)(p_i2c_context->slave_address)))
        {
            //If I2C Master fails to invoke the read operation, invoke upper layer event handler with error.

            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);

        }
        else
        {
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);

            status = PAL_STATUS_SUCCESS;
        }

        // Release I2C Bus
        k_sem_give(&i2c_bus_lock);
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return status;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t * p_i2c_context, uint16_t bitrate)
{
    int i2c_config_result=1;
    int lock_result;
    pal_status_t status = PAL_STATUS_FAILURE;
    optiga_lib_status_t event = PAL_I2C_EVENT_ERROR;

    pal_zephyr_i2c_t *p_i2c_ctx = (pal_zephyr_i2c_t*)(p_i2c_context->p_i2c_hw_config);

    // Acquire the I2C bus before setting the bitrate
    lock_result = k_sem_take(&i2c_bus_lock, K_NO_WAIT);

    if (lock_result == I2C_BUS_ENTRY_SUCCESS)
    {
        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
        // set the I2C master to its maximum supported value.
        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)
        {
            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
        }

        // Logic to convert bitrate to correct I2C_SPEED_VALUES from Zephyr
        switch (bitrate)
        {
        case PAL_I2C_MASTER_DEFAULT_BITRATE:
            i2c_config_result = i2c_configure(p_i2c_ctx->p_i2c_dev, I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_MASTER);
            break;

        case PAL_I2C_MASTER_FAST_BITRATE:
            i2c_config_result = i2c_configure(p_i2c_ctx->p_i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER);
            break;

        case PAL_I2C_MASTER_MAX_BITRATE:
            i2c_config_result = i2c_configure(p_i2c_ctx->p_i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST_PLUS) | I2C_MODE_MASTER);
            break;
        
        default:
            LOG_ERR("requested I2C-Device bitrate not supported");
            status = PAL_STATUS_FAILURE;
            break;
        }

        if (i2c_config_result == 0)
        {
            // I2C speed set correctly
            status = PAL_STATUS_SUCCESS;
            event = PAL_I2C_EVENT_SUCCESS;
        }
        else
        {
            LOG_ERR("could not set I2C-device bitrate");
            status = PAL_STATUS_FAILURE;
        }

        // Release I2C Bus
        k_sem_give(&i2c_bus_lock);
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        event = PAL_I2C_EVENT_BUSY;
    }
    if (0 != p_i2c_context->upper_layer_event_handler)
    {
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t type"
        ((callback_handler_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx, event);
    }
    // Release I2C Bus if it was acquired 
    if (PAL_STATUS_I2C_BUSY != status)
    {
        k_sem_give(&i2c_bus_lock);
    }
    return status;
}

/**
* @}
*/
