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
#include "main.h"
#include "optiga/pal/pal_i2c.h"

typedef struct stm_i2c_params
{
	I2C_TypeDef* i2c_inst;
	uint32_t     speed;
	uint32_t     timing;
}stm_i2c_params_t;

typedef struct stm_i2c_ctx
{
	I2C_HandleTypeDef 	i2c;
	stm_i2c_params_t    i2c_params;
}stm_i2c_ctx_t;

#define PAL_I2C_MASTER_MAX_BITRATE  (400U)
/// @cond hidden

_STATIC_H volatile uint32_t g_entry_count = 0;
_STATIC_H pal_i2c_t * gp_pal_i2c_current_ctx;

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


pal_status_t pal_i2c_init(const pal_i2c_t * p_i2c_context)
{
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t * p_i2c_context)
{
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    I2C_HandleTypeDef * p_i2c = NULL;



    //Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context) && (p_i2c_context != NULL))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;
        p_i2c = &((stm_i2c_ctx_t *)(p_i2c_context->p_i2c_hw_config))->i2c;


        //Invoke the low level i2c master driver API to write to the bus
        if (HAL_I2C_Master_Transmit(p_i2c, (p_i2c_context->slave_address << 1), p_data, length, 50) != HAL_OK)
        {
        	((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
        }
        else
        {
        	((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
            status = PAL_STATUS_SUCCESS;
        }

        //Release I2C Bus
        pal_i2c_release((void *)p_i2c_context);
    }
    else
    {
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
    }
    return status;
}

pal_status_t pal_i2c_read(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
	pal_status_t status = PAL_STATUS_FAILURE;
	I2C_HandleTypeDef * p_i2c = NULL;

	//Acquire the I2C bus before read/write
	if ((PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) && (p_i2c_context != NULL))
	{
		gp_pal_i2c_current_ctx = p_i2c_context;
		p_i2c = &((stm_i2c_ctx_t *)(p_i2c_context->p_i2c_hw_config))->i2c;

		//Invoke the low level i2c master driver API to read from the bus
		if (HAL_I2C_Master_Receive(p_i2c, (p_i2c_context->slave_address << 1), p_data, length, 50) != HAL_OK)
		{
			//If I2C Master fails to invoke the read operation, invoke upper layer event handler with error.
			((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
		}
		else
		{
			//lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
			((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
			status = PAL_STATUS_SUCCESS;
		}

		//Release I2C Bus
		pal_i2c_release((void *)p_i2c_context);
	}
	else
	{
		status = PAL_STATUS_I2C_BUSY;
		((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
	}
	return status;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t * p_i2c_context, uint16_t bitrate)
{

	pal_status_t return_status = PAL_STATUS_SUCCESS;

    return return_status;
}


/**
* @}
*/
