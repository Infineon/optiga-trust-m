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

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/pal/pal_i2c.h"
#ifdef __WIN32__
#include "libusb.h"
#else // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif
#include "pal_usb.h"
#include "pal_common.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
// Setup debug log statements
#if IFX_I2C_LOG_PAL == 1
#define LOG_PAL IFX_I2C_LOG
#else
#include<stdio.h>
#define LOG_PAL(...) //printf(__VA_ARGS__)
#endif

/// @cond hidden
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

void i2c_master_end_of_transmit_callback(void);
void i2c_master_end_of_receive_callback(void);
void invoke_upper_layer_callback(const pal_i2c_t * p_pal_i2c_ctx, optiga_lib_status_t event);
static uint16_t usb_i2c_poll_operation_result(pal_i2c_t * p_i2c_context);

/* Variable to indicate the re-entrant count of the i2c bus acquire function*/
static volatile uint32_t g_entry_count = 0;
/* Pointer to the current pal i2c context*/
static pal_i2c_t * gp_pal_i2c_current_ctx;
extern pal_usb_t usb_events;

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/
// I2C acquire bus function
//lint --e{715} suppress the unused p_i2c_context variable lint error , since this is kept for future enhancements
static pal_status_t pal_i2c_acquire(const void * p_i2c_context)
{
    if (g_entry_count == 0)
    {
        g_entry_count++;
        if (g_entry_count == 1)
        {
            return PAL_STATUS_SUCCESS;
        }
    }
    return PAL_STATUS_FAILURE;
}

// I2C release bus function
//lint --e{715} suppress the unused p_i2c_context variable lint, since this is kept for future enhancements
static void pal_i2c_release(const void * p_i2c_context)
{
    g_entry_count = 0;
}
/// @endcond

/**
 * Pal I2C event handler function to invoke the registered upper layer callback<br>
 *
 *<b>API Details:</b>
 *  - This function implements the platform specific i2c event handling mechanism<br>
 *  - It calls the registered upper layer function after completion of the I2C read/write operations<br>
 *  - The respective event status are explained below.
 *   - #PAL_I2C_EVENT_ERROR when I2C fails due to low level failures(NACK/I2C protocol errors)
 *   - #PAL_I2C_EVENT_SUCCESS when operation is successfully completed
 *
 * \param[in] p_pal_i2c_ctx   Pointer to the pal i2c context #pal_i2c_t
 * \param[in] event           Status of the event reported after read/write completion or due to I2C errors
 *
 */
void invoke_upper_layer_callback (const pal_i2c_t * p_pal_i2c_ctx, optiga_lib_status_t event)
{
	upper_layer_callback_t upper_layer_handler;
    //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
    upper_layer_handler = (upper_layer_callback_t)p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx, event);

    //Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->p_upper_layer_ctx);
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
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_ERROR);
}


// I2C driver callback function when the nack error detected
void i2c_master_nack_received_callback(void)
{
    i2c_master_error_detected_callback();
}

// I2C driver callback function when the arbitration lost error detected
void i2c_master_arbitration_lost_callback(void)
{
    i2c_master_error_detected_callback();
}


uint16_t usb_i2c_poll_operation_result(pal_i2c_t * p_i2c_context)
{
    uint8_t report[HID_REPORT_SIZE] = {0};
    LOG_PAL("usb_i2c_poll_operation_result\n. ");
    while (1)
    {
        if (usb_hid_get_feature(REPORT_ID_I2C_STATUS, report, &usb_events) != 5)
        {
            LOG_PAL("[IFX-HAL]: USB get I2C status failed.\n");
            return PAL_I2C_EVENT_ERROR;
        }

        if (!(report[1] & I2C_STATUS_CONTROLLER_BUSY))
        {
            if (report[1] & I2C_STATUS_ERROR_CONDITION)
            {
                return PAL_I2C_EVENT_ERROR;
            }

            if (report[1] & I2C_STATUS_CONTROLLER_IDLE)
            {
                if (!(report[1] & I2C_STATUS_BUS_BUSY))
                {
                    return PAL_STATUS_SUCCESS;
                }
            }
        }
    }
}

/// @endcond
/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

/**
 * Initializes the i2c master with the given context.
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
pal_status_t pal_i2c_init(const pal_i2c_t * p_i2c_context)
{
    return PAL_STATUS_SUCCESS;
}

/**
 * De-initializes the I2C master with the specified context.
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
 *     - This API must ensure that multiple users/applications sharing the same I2C master resource is not impacted.
 *
 *<b>User Input:</b><br>
 * - The input #pal_i2c_t p_i2c_context must not be NULL.<br>
 *
 * \param[in] p_i2c_context   I2C context to be de-initialized
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the I2C master de-init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the I2C de-init fails.
 */
pal_status_t pal_i2c_deinit(const pal_i2c_t * p_i2c_context)
{
    LOG_PAL("pal_i2c_deinit\n. ");
    return PAL_STATUS_SUCCESS;
}

/**
 * Writes the data to I2C slave.
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

pal_status_t pal_i2c_write(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    pal_status_t status = PAL_STATUS_FAILURE;
    int32_t usb_lib_status;
    int32_t transfered = 0;
    pal_usb_t * pal_usb;
    uint8_t report[HID_REPORT_SIZE] = {0};

    pal_usb = (pal_usb_t * ) p_i2c_context->p_i2c_hw_config;
    report[0] = REPORT_ID_I2C_WRITE_REQ;
    report[1] = p_i2c_context->slave_address;
    report[2] = I2C_FLAG_START | I2C_FLAG_STOP;
    report[3] = (uint8_t)length;

    memcpy(&report[4], p_data, length);
    //Acquire the I2C bus before read/write

    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;

        //Invoke the low level i2c master driver API to write to the bus
        usb_lib_status = libusb_interrupt_transfer(pal_usb->handle,
                                                   pal_usb->hid_ep_out,
                                                   report,
                                                   sizeof(report),
                                                   &transfered,
                                                   USB_TIMEOUT);
        if (usb_lib_status != 0 || transfered != HID_REPORT_SIZE)
        {
            //If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.
            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_ERROR);
            //Release I2C Bus
            pal_i2c_release((void * )p_i2c_context);
        }
        else
        {
            if (usb_i2c_poll_operation_result(p_i2c_context) == PAL_STATUS_SUCCESS)
            {
                i2c_master_end_of_transmit_callback();
                status = PAL_STATUS_SUCCESS;
            }
            else
            {
                invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_ERROR);
            }
        }
    }
    else
    {
        status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_BUSY);
    }

    return status;
}

/**
 * Reads the data from I2C slave.
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
pal_status_t pal_i2c_read(pal_i2c_t * p_i2c_context, uint8_t * p_data, uint16_t length)
{
    int32_t usb_lib_status = PAL_STATUS_FAILURE;
    int32_t transfered;
    uint8_t report[HID_REPORT_SIZE] = {0};
    uint16_t rx_result;
    pal_usb_t * pal_usb;
    LOG_PAL("[IFX-HAL]: I2C RX (%d)\n", length);

    report[0] = REPORT_ID_I2C_READ_REQ;
    report[1] = p_i2c_context->slave_address;
    report[2] = I2C_FLAG_START | I2C_FLAG_STOP;
    report[3] = (uint8_t)length;
    report[4] = 0;
    pal_usb = (pal_usb_t * ) p_i2c_context->p_i2c_hw_config;
    //Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
    {
        gp_pal_i2c_current_ctx = p_i2c_context;
        usb_lib_status = libusb_interrupt_transfer(pal_usb->handle,
                                                   pal_usb->hid_ep_out,
                                                   report,
                                                   sizeof(report),
                                                   &transfered,
                                                   USB_TIMEOUT);
        LOG_PAL("[IFX-HAL]: HID Write status = %d, %d\n", usb_lib_status, transfered);
        if (usb_lib_status)
        {
            LOG_PAL("[IFX-HAL]: libusb_interrupt_transfer ERROR %d\n.", usb_lib_status);
            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                       (p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_ERROR);
            return usb_lib_status;
        }
        //Invoke the low level i2c master driver API to read from the bus
        memset(report, 0x00, sizeof(report));
        transfered = 0;
        usb_lib_status = libusb_interrupt_transfer(pal_usb->handle,
                                                   pal_usb->hid_ep_in,
                                                   report,
                                                   sizeof(report),
                                                   &transfered,
                                                   USB_TIMEOUT);
        rx_result = usb_i2c_poll_operation_result(p_i2c_context);
        if (rx_result == PAL_STATUS_SUCCESS && usb_lib_status == 0 && transfered >= (2 + length) && report[1] == length)
        {
            memcpy(p_data, &report[2], report[1]);
            usb_lib_status = PAL_STATUS_SUCCESS;
            i2c_master_end_of_receive_callback();
        }
        else
        {
            //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_ERROR);
            //Release I2C Bus
            pal_i2c_release((void * )p_i2c_context);
        }
    }
    else
    {
        usb_lib_status = PAL_STATUS_I2C_BUSY;
        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler))
                                                        (p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_BUSY);
    }
    return usb_lib_status;
}


/**
 * Sets the bitrate/speed(KHz) of I2C master.
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
pal_status_t pal_i2c_set_bitrate(const pal_i2c_t * p_i2c_context, uint16_t bitrate)
{
    pal_status_t return_status = PAL_STATUS_SUCCESS;
//    host_lib_status_t event = PAL_I2C_EVENT_ERROR;
//    LOG_PAL("pal_i2c_set_bitrate\n. ");
//    //Acquire the I2C bus before setting the bitrate
//    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context))
//    {
//        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
//        // set the I2C master to its maximum supported value.
//        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE)
//        {
//            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
//        }
//        return_status = PAL_STATUS_SUCCESS;
//        event = PAL_I2C_EVENT_SUCCESS;
//    }
//    else
//    {
//        return_status = PAL_STATUS_I2C_BUSY;
//        event = PAL_I2C_EVENT_BUSY;
//    }
//    if (p_i2c_context->upper_layer_event_handler)
//    {
//        //lint --e{611} suppress "void* function pointer is type casted to app_event_handler_t type"
//        ((app_event_handler_t)(p_i2c_context->upper_layer_event_handler))(p_i2c_context->upper_layer_ctx, event);
//    }
//    //Release I2C Bus
//    pal_i2c_release((void * )p_i2c_context);
    return return_status;
}

/**
* @}
*/
