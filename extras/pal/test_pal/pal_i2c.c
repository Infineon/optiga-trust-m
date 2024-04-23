/**
* \copyright
* MIT License
*
* Copyright (c) 2024 Infineon Technologies AG
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
* @{
*/
#include <unistd.h>
#include <stdio.h>

#include "pal_i2c.h"
#include "pal_linux.h"

/* Define LOG_HAL as normal stdio printf */
#define LOG_HAL                     printf
/* Max Bitrate Definition */
#define PAL_I2C_MASTER_MAX_BITRATE  100

/* I2C Master end of transmit Callback */
void i2c_master_end_of_transmit_callback(void);
/* I2C Master end of receive Callback */
void i2c_master_end_of_receive_callback(void);
/* Upper Layer Callback */
void invoke_upper_layer_callback (const pal_i2c_t* p_pal_i2c_ctx, optiga_lib_status_t event);

/* Varibale to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;

/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;

/* Slave address Event handler */
_STATIC_H void ut_pal_slave_addr_event_handler(void * p_ctx, optiga_lib_status_t event)
{
    (void)(p_ctx);
    (void)(event);
}

/* I2C acquire lock */
_STATIC_H pal_status_t pal_i2c_acquire(void)
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

/* I2C release lock */
_STATIC_H void pal_i2c_release(void)
{
    g_entry_count = 0;
}

/* Upper Layer Callback */
void invoke_upper_layer_callback (const pal_i2c_t * p_pal_i2c_ctx, optiga_lib_status_t event)
{
    upper_layer_callback_t  upper_layer_handler;
    
    upper_layer_handler = (upper_layer_callback_t )p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx , event);

    //Release I2C Bus
    pal_i2c_release();
}

/* I2C Master end of transmit Callback */
void i2c_master_end_of_transmit_callback(void)
{
    
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}


/* I2C Master end of receive Callback */
void i2c_master_end_of_receive_callback(void)
{
	invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

/* PAL I2C Init */
pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
	LOG_HAL("pal_i2c_init - Dummy\n. ");

    pal_i2c_t* ut_p_i2c_ctx = (pal_i2c_t*) p_i2c_context;

    ut_p_i2c_ctx->upper_layer_event_handler = (void *)ut_pal_slave_addr_event_handler;
	
    return PAL_STATUS_SUCCESS;
}

/* PAL I2C De-Init */
pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
	LOG_HAL("pal_i2c_deinit - Dummy\n. ");

    pal_i2c_release();
	
    return PAL_STATUS_SUCCESS;
}

/* PAL I2C Write */
pal_status_t pal_i2c_write(const pal_i2c_t* p_i2c_context,uint8_t* p_data , uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;

    /* Check if the I2C can be locked */
    if(PAL_STATUS_SUCCESS == pal_i2c_acquire())
    {
        /* Pointing to the current I2C Context */
        gp_pal_i2c_current_ctx = (pal_i2c_t *)p_i2c_context;
        /* Dummy component - Not write action is hapenning */
        i2c_master_end_of_transmit_callback();
        status = PAL_STATUS_SUCCESS;
    }
    else
    {
        /* I2C is busy, couldn'e acquire the lock */
        status = PAL_STATUS_I2C_BUSY;
        /* Calling upper layer callback with PAL_I2C_EVENT_BUSY event*/
        ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_BUSY);
    }

    return status;
}

/* PAL I2C Read */
pal_status_t pal_i2c_read(const pal_i2c_t* p_i2c_context , uint8_t* p_data , uint16_t length)
{
    pal_status_t i2c_read_status = PAL_STATUS_FAILURE;
    
    /* Check if the I2C can be locked */
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire())
    {    
        /* Pointing to the current I2C Context */
        gp_pal_i2c_current_ctx = (pal_i2c_t *)p_i2c_context;
        /* Dummy component - Not read action is hapenning */
        i2c_master_end_of_receive_callback();
        i2c_read_status = PAL_STATUS_SUCCESS;
    }
    else
    {
        /* I2C is busy, couldn'e acquire the lock */
        i2c_read_status = PAL_STATUS_I2C_BUSY;
        /* Calling upper layer callback with PAL_I2C_EVENT_BUSY event*/
        ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_BUSY);
    }
    return i2c_read_status;
}

   
/* PAL I2C Set Bitrate */
pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context , uint16_t bitrate)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;
    optiga_lib_status_t event = PAL_I2C_EVENT_ERROR;

    /* Check if the I2C can be locked */
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire())
    {    
        /* If the user provided bitrate is greater than the I2C master hardware maximum supported value,
         set the I2C master to its maximum supported value. */
        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)         
        {
            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
        }
        /* Dummy component - Not Bitrate will be set */
        return_status = PAL_STATUS_SUCCESS;
        event = PAL_I2C_EVENT_SUCCESS;
    }
    else
    {
        /* I2C is busy, couldn'e acquire the lock */
        return_status = PAL_STATUS_I2C_BUSY;
        event = PAL_I2C_EVENT_BUSY;
    }

    if (0 != p_i2c_context->upper_layer_event_handler)
    {
        /* Calling upper layer callback with event*/
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx  , event);
    }

    /* Release I2C Bus */
    pal_i2c_release();

    return return_status;
}

/**
* @}
*/
