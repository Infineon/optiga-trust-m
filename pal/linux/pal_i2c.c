/**
* \copyright
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
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

#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "optiga/pal/pal_i2c.h"
#include "pal_linux.h"

#if IFX_I2C_LOG_HAL == 1
#define LOG_HAL IFX_I2C_LOG
#else
#include<stdio.h>
#define LOG_HAL(...) //printf(__VA_ARGS__)
#endif

/// I2C device
char * i2c_if = "/dev/i2c-1";

// Slave address not initialization
#define IFXI2C_SLAVE_ADDRESS_INIT 0xFFFF
#define PAL_I2C_MASTER_MAX_BITRATE 100
#define WAIT_500_MS	(500)
/// @cond hidden

void i2c_master_end_of_transmit_callback(void);
void i2c_master_end_of_receive_callback(void);
void invoke_upper_layer_callback (const pal_i2c_t* p_pal_i2c_ctx, optiga_lib_status_t event);
uint16_t usb_i2c_poll_operation_result(pal_i2c_t* p_i2c_context);

/* Varibale to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;

/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;

//lint --e{715} suppress the unused p_i2c_context variable lint error , since this is kept for future enhancements
static pal_status_t pal_i2c_acquire(const void * p_i2c_context)
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

// I2C release bus function
//lint --e{715} suppress the unused p_i2c_context variable lint, since this is kept for future enhancements
static void pal_i2c_release(const void* p_i2c_context)
{
    g_entry_count = 0;
}
/// @endcond

void invoke_upper_layer_callback (const pal_i2c_t * p_pal_i2c_ctx, optiga_lib_status_t event)
{
    upper_layer_callback_t  upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
    upper_layer_handler = (upper_layer_callback_t )p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx , event);

    //Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->p_upper_layer_ctx );
}

/// @cond hidden
// I2C driver callback function when the transmit is completed successfully
void i2c_master_end_of_transmit_callback(void)
{
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}


// I2C driver callback function when the receive is completed successfully
void i2c_master_end_of_receive_callback(void)
{
	invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

// I2C error callback function
void i2c_master_error_detected_callback(void)
{
    //I2C_MASTER_t *p_i2c_master;
    //
    //p_i2c_master = gp_pal_i2c_current_ctx->p_i2c_hw_config;
    //if (I2C_MASTER_IsTxBusy(p_i2c_master))
    //{
    //    //lint --e{534} suppress "Return value is not required to be checked"
    //    I2C_MASTER_AbortTransmit(p_i2c_master);
    //    while (I2C_MASTER_IsTxBusy(p_i2c_master)){}
    //}  

    //if (I2C_MASTER_IsRxBusy(p_i2c_master)) 
    //{
    //    //lint --e{534} suppress "Return value is not required to be checked"
    //    I2C_MASTER_AbortReceive(p_i2c_master);
    //    while (I2C_MASTER_IsRxBusy(p_i2c_master)){}
    //}

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

pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
	int32_t ret = PAL_I2C_EVENT_ERROR;
	pal_linux_t *pal_linux;
	do
	{
		pal_linux = (pal_linux_t*) p_i2c_context->p_i2c_hw_config;
		pal_linux->i2c_handle = open(i2c_if, O_RDWR);
		LOG_HAL("IFX OPTIGA TRUST X Logs \n");
		
		// Assign the slave address
		ret = ioctl(pal_linux->i2c_handle, I2C_SLAVE, p_i2c_context->slave_address);
		if(PAL_STATUS_SUCCESS != ret)
		{
			LOG_HAL((uint32_t)pal_linux->i2c_handle, "ioctl returned an error = ", ret);
			break;
		}
		
		//start_transceive_thread();
	}while(0);
    return ret;
}


pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context)
{
	LOG_HAL("pal_i2c_deinit\n. ");
	
    return PAL_STATUS_SUCCESS;
}


pal_status_t pal_i2c_write(const pal_i2c_t* p_i2c_context,uint8_t* p_data , uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    int32_t i2c_write_status;
	pal_linux_t *pal_linux;

	pal_linux = (pal_linux_t*) p_i2c_context->p_i2c_hw_config;
	LOG_HAL("[IFX-HAL]: I2C TX (%d): ", length);
#if 1
    for (int i = 0; i < length; i++)
    {
        LOG_HAL("%02X ", p_data[i]);
    }
#endif
    LOG_HAL("\n");
    if(PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        gp_pal_i2c_current_ctx = (pal_i2c_t *)p_i2c_context;

        //Invoke the low level i2c master driver API to write to the bus

		i2c_write_status = write(pal_linux->i2c_handle, p_data, length);
        if (0 > i2c_write_status)
        {
            //If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.

            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
            ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_ERROR);
            
            //Release I2C Bus
            pal_i2c_release((void *)p_i2c_context);
        }
        else
        {
        	i2c_master_end_of_transmit_callback();
            status = PAL_STATUS_SUCCESS;
			//transmission_completed = true;
        }
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
        ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_BUSY);
    }

    return status;
}


pal_status_t pal_i2c_read(const pal_i2c_t* p_i2c_context , uint8_t* p_data , uint16_t length)
{
    int32_t i2c_read_status = PAL_STATUS_FAILURE;
	pal_linux_t *pal_linux;
    

	pal_linux = (pal_linux_t*) p_i2c_context->p_i2c_hw_config;
    //Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {    
        gp_pal_i2c_current_ctx = (pal_i2c_t *)p_i2c_context;
		i2c_read_status = read(pal_linux->i2c_handle,p_data, length);
		if (0 > i2c_read_status)
		{
    		LOG_HAL("[IFX-HAL]: libusb_interrupt_transfer ERROR %d\n.", i2c_read_status);
            //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
            ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_ERROR);
													   			//Release I2C Bus
			pal_i2c_release((void *)p_i2c_context);
    		return i2c_read_status;
		}
		else
        {
			LOG_HAL("[IFX-HAL]: I2C RX (%d)\n", length);
#if 1
			for (int i = 0; i < length; i++)
			{
				LOG_HAL("%02X ", p_data[i]);
			}
#endif
			
			i2c_master_end_of_receive_callback();
			i2c_read_status = PAL_STATUS_SUCCESS;
			//reception_started = true;
        }
    }
    else
    {
        i2c_read_status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
        ((upper_layer_callback_t )(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx  , PAL_I2C_EVENT_BUSY);
    }
    return i2c_read_status;
}

   

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context , uint16_t bitrate)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;
    optiga_lib_status_t event = PAL_I2C_EVENT_ERROR;
	LOG_HAL("pal_i2c_set_bitrate\n. ");
    //Acquire the I2C bus before setting the bitrate
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {    
        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
        // set the I2C master to its maximum supported value.
        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)         
        {
            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
        }
        return_status = PAL_STATUS_SUCCESS;
        event = PAL_I2C_EVENT_SUCCESS;
    }
    else
    {
        return_status = PAL_STATUS_I2C_BUSY;
        event = PAL_I2C_EVENT_BUSY;
    }
    if (0 != p_i2c_context->upper_layer_event_handler)
    {
        //lint --e{611} suppress "void* function pointer is type casted to upper_layer_callback_t  type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx  , event);
    }
    //Release I2C Bus
    pal_i2c_release((void *)p_i2c_context);
    return return_status;
}

/**
* @}
*/
