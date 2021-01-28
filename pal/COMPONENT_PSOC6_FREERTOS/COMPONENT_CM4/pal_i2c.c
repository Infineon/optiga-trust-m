/**
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
 *
 * \file
 *
 * \brief This file implements the platform abstraction layer(pal) APIs for I2C.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include <limits.h>

#include "optiga/pal/pal_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"

#include "cyhal.h"
#include "cybsp.h"

#include "FreeRTOS.h"
#include "task.h"

/* I2C bus frequency */
#define I2C_FREQ                (400000UL)

/// @cond hidden

/* Pointer to the current pal i2c context */
static const pal_i2c_t *gp_pal_i2c_current_ctx = NULL;
static TaskHandle_t i2c_taskhandle = NULL;

static void i2c_task(void *pvParameters)
{
	upper_layer_callback_t upper_layer_handler;
	uint32_t event = 0;

	while (1) {
		xTaskNotifyWait(0, UINT_MAX, &event, portMAX_DELAY);

		upper_layer_handler =
				(upper_layer_callback_t) gp_pal_i2c_current_ctx->upper_layer_event_handler;

		/* Check write complete event */
		if (0UL != (CYHAL_I2C_MASTER_WR_CMPLT_EVENT & event)) {
			upper_layer_handler(gp_pal_i2c_current_ctx->p_upper_layer_ctx,
					PAL_I2C_EVENT_SUCCESS);
		}
		/* Check read complete event */
		if (0UL != (CYHAL_I2C_MASTER_RD_CMPLT_EVENT & event)) {
			upper_layer_handler(gp_pal_i2c_current_ctx->p_upper_layer_ctx,
					PAL_I2C_EVENT_SUCCESS);
		}
		if (0UL != (CYHAL_I2C_MASTER_ERR_EVENT & event)) {
			/* In case of error abort transfer */
			cyhal_i2c_abort_async(gp_pal_i2c_current_ctx->p_i2c_hw_config);
			upper_layer_handler(gp_pal_i2c_current_ctx->p_upper_layer_ctx,
					PAL_I2C_EVENT_ERROR);
		}
	}
}

/* Defining master callback handler */
void master_event_handler(void *callback_arg, cyhal_i2c_event_t event)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTaskNotifyFromISR(i2c_taskhandle, event, eSetBits,
			&xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/// @endcond

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

/**
 * API to initialize the i2c master with the given context.
 * <br>
 *
 *<b>API Details:</b>
 * - The platform specific initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not initialize and return #PAL_STATUS_I2C_BUSY status.
 *   - Repeated initialization must be taken care with respect to the platform requirements. (Example: Multiple users/applications  
 *     sharing the same I2C master resource)
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context   Pal i2c context to be initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C init fails.
 */
pal_status_t pal_i2c_init(const pal_i2c_t* p_i2c_context)
{
	cy_rslt_t result;
	if ((p_i2c_context != NULL) && (p_i2c_context->p_i2c_hw_config != NULL))
	{
		if (xTaskCreate(i2c_task, "i2c_task", configMINIMAL_STACK_SIZE * 2,
				NULL, configMAX_PRIORITIES - 1, &i2c_taskhandle) != pdPASS)
		{
			return PAL_STATUS_FAILURE;
		}

		cyhal_i2c_cfg_t mI2C_cfg;

		/* Configure I2C Master */
		printf(">> Configuring I2C Master..... ");
		mI2C_cfg.is_slave = false;
		mI2C_cfg.address = 0;
		mI2C_cfg.frequencyhal_hz = I2C_FREQ;
		result = cyhal_i2c_init(p_i2c_context->p_i2c_hw_config, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
		if (result != CY_RSLT_SUCCESS)
		{
			return PAL_STATUS_FAILURE;
		}
		result = cyhal_i2c_configure(p_i2c_context->p_i2c_hw_config, &mI2C_cfg);
		if (result != CY_RSLT_SUCCESS)
		{
			return PAL_STATUS_FAILURE;
		}

		/* Register I2C slave event callback */
		cyhal_i2c_register_callback(p_i2c_context->p_i2c_hw_config,
				(cyhal_i2c_event_callback_t) master_event_handler, NULL);
		/* Enable I2C Events */
		cyhal_i2c_enable_event(p_i2c_context->p_i2c_hw_config,
				(cyhal_i2c_event_t) (CYHAL_I2C_SLAVE_WR_CMPLT_EVENT
						| CYHAL_I2C_SLAVE_RD_CMPLT_EVENT
						| CYHAL_I2C_SLAVE_ERR_EVENT), 3u, true);

		return PAL_STATUS_SUCCESS;
	}
	return PAL_STATUS_FAILURE;
}

/**
 * API to de-initialize the I2C master with the specified context.
 * <br>
 *
 *<b>API Details:</b>
 * - The platform specific de-initialization of I2C master has to be implemented as part of this API, if required.<br>
 * - If the target platform does not demand explicit de-initialization of i2c master
 *   (Example: If the platform driver takes care of init after the reset), it would not be required to implement.<br>
 * - The implementation must take care the following scenarios depending upon the target platform selected.
 *   - The implementation must handle the acquiring and releasing of the I2C bus before de-initializing the I2C master to
 *     avoid interrupting the ongoing slave I2C transactions using the same I2C master.
 *   - If the I2C bus is in busy state, the API must not de-initialize and return #PAL_STATUS_I2C_BUSY status.
 *	 - This API must ensure that multiple users/applications sharing the same I2C master resource is not impacted.
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context   I2C context to be de-initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master de-init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C de-init fails.
 */
pal_status_t pal_i2c_deinit(const pal_i2c_t* p_i2c_context) {
	if ((p_i2c_context != NULL) && (p_i2c_context->p_i2c_hw_config != NULL)) {
		if (i2c_taskhandle != NULL) {
			vTaskDelete(i2c_taskhandle);
		}

		// ARM_DRIVER_I2C *I2Cdrv = (ARM_DRIVER_I2C *)p_i2c_context->p_i2c_hw_config;
		// I2Cdrv->PowerControl(ARM_POWER_OFF);
		// I2Cdrv->Uninitialize();

		return PAL_STATUS_SUCCESS;
	}
	return PAL_STATUS_FAILURE;
}

/**
 * Platform abstraction layer API to write the data to I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_write.png "pal_i2c_write()" width=20cm
 *
 *
 *<b>API Details:</b>
 * - The API attempts to write if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of transmission or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 * - The upper_layer_event_handler must be initialized in the p_i2c_context before invoking the API.<br>
 *
 *<b>Notes:</b><br> 
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *
 * \param[in] p_i2c_context  Pointer to the pal I2C context #pal_i2c_t
 * \param[in] p_data         Pointer to the data to be written
 * \param[in] length         Length of the data to be written
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C write is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C write fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy. 
 */
pal_status_t pal_i2c_write(const pal_i2c_t *p_i2c_context, uint8_t *p_data,
		uint16_t length) {
	pal_status_t pal_status = PAL_STATUS_FAILURE;
	cy_rslt_t status;

	if ((p_i2c_context != NULL) && (p_i2c_context->p_i2c_hw_config != NULL)) {
		gp_pal_i2c_current_ctx = p_i2c_context;

		status = cyhal_i2c_master_transfer_async(p_i2c_context->p_i2c_hw_config,
				p_i2c_context->slave_address, p_data, length, NULL, 0);

		if (status == CY_RSLT_SUCCESS) {
			pal_status = PAL_STATUS_SUCCESS;
		} else {
			cyhal_i2c_event_t event = CYHAL_I2C_MASTER_ERR_EVENT;
			xTaskNotify(i2c_taskhandle, event, eSetBits);
		}
	}

	return pal_status;
}

/**
 * Platform abstraction layer API to read the data from I2C slave.
 * <br>
 * <br>
 * \image html pal_i2c_read.png "pal_i2c_read()" width=20cm
 *
 *<b>API Details:</b>
 * - The API attempts to read if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released only after the completion of reception or after completion of error handling.<br>
 * - The API invokes the upper layer handler with the respective event status as explained below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed asynchronously
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 * - The upper_layer_event_handler must be initialized in the p_i2c_context before invoking the API.<br>
 *
 *<b>Notes:</b><br> 
 *  - Otherwise the below implementation has to be updated to handle different bitrates based on the input context.<br>
 *  - The caller of this API must take care of the guard time based on the slave's requirement.<br>
 *
 * \param[in]  p_i2c_context  pointer to the PAL i2c context #pal_i2c_t
 * \param[in]  p_data         Pointer to the data buffer to store the read data
 * \param[in]  length         Length of the data to be read
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C read is invoked successfully
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C read fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
pal_status_t pal_i2c_read(const pal_i2c_t* p_i2c_context, uint8_t* p_data,
		uint16_t length) {
	pal_status_t pal_status = PAL_STATUS_FAILURE;
	cy_rslt_t status;

	if ((p_i2c_context != NULL) && (p_i2c_context->p_i2c_hw_config != NULL)) {
		gp_pal_i2c_current_ctx = p_i2c_context;

		status = cyhal_i2c_master_transfer_async(p_i2c_context->p_i2c_hw_config,
				p_i2c_context->slave_address, NULL, 0, p_data, length);

		if (status == CY_RSLT_SUCCESS) {
			pal_status = PAL_STATUS_SUCCESS;
		} else {
			cyhal_i2c_event_t event = CYHAL_I2C_MASTER_ERR_EVENT;
			xTaskNotify(i2c_taskhandle, event, eSetBits);
		}
	}

	return pal_status;
}

/**
 * Platform abstraction layer API to set the bitrate/speed(KHz) of I2C master.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets the bitrate of I2C master if the I2C bus is free, else it returns busy status #PAL_STATUS_I2C_BUSY<br>
 * - The bus is released after the setting the bitrate.<br>
 * - This API must take care of setting the bitrate to I2C master's maximum supported value. 
 * - Eg. In XMC4500, the maximum supported bitrate is 400 KHz. If the supplied bitrate is greater than 400KHz, the API will 
 *   set the I2C master's bitrate to 400KHz.
 * - Use the #PAL_I2C_MASTER_MAX_BITRATE macro to specify the maximum supported bitrate value for the target platform.
 * - If upper_layer_event_handler is initialized, the upper layer handler is invoked with the respective event 
 *   status listed below.
 *   - #PAL_I2C_EVENT_BUSY when I2C bus in busy state
 *   - #PAL_I2C_EVENT_ERROR when API fails to set the bit rate 
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successful
 *<br>
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t  p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context  Pointer to the pal i2c context
 * \param[in] bitrate        Bitrate to be used by i2c master in KHz
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the setting of bitrate is successfully completed
 * \retval  #PAL_STATUS_FAILURE  Returns when the setting of bitrate fails.
 * \retval  #PAL_STATUS_I2C_BUSY Returns when the I2C bus is busy.
 */
pal_status_t pal_i2c_set_bitrate(const pal_i2c_t* p_i2c_context,
		uint16_t bitrate) {
	pal_status_t pal_status = PAL_STATUS_FAILURE;

	if ((p_i2c_context != NULL) && (p_i2c_context->p_i2c_hw_config != NULL)) {
		// ARM_DRIVER_I2C *I2Cdrv = (ARM_DRIVER_I2C *)p_i2c_context->p_i2c_hw_config;
		// uint32_t arg;
		// if (bitrate >= 400)
		// {
		// arg = ARM_I2C_BUS_SPEED_FAST;
		// }
		// else
		// {
		// arg = ARM_I2C_BUS_SPEED_STANDARD;
		// }

		// callback_handler_t upper_layer_event_handler = (callback_handler_t)p_i2c_context->upper_layer_event_handler;

		// int32_t status = I2Cdrv->Control(ARM_I2C_BUS_SPEED, arg);

		// if (status == ARM_DRIVER_OK)
		// {
		// if (upper_layer_event_handler)
		// {
		// upper_layer_event_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_SUCCESS);
		// }
		// return PAL_STATUS_SUCCESS;
		// }
		// else if (status == ARM_DRIVER_ERROR_BUSY)
		// {
		// if (upper_layer_event_handler)
		// {
		// upper_layer_event_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_BUSY);
		// }
		// pal_status = PAL_STATUS_I2C_BUSY;
		// }
		// else
		// {
		// if (upper_layer_event_handler)
		// {
		// upper_layer_event_handler(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
		// }
		// }
	}

	return pal_status;
}

/**
 * @}
 */
