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
* \file
*
* \brief This file implements the platform abstraction layer APIs.
*
* \ingroup  grPAL
* @{
*/


/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
//#include <DAVE.h>
#include "pal_gpio.h"
#include "pal_usb.h"
#ifdef __WIN32__
#include "libusb.h"
#else // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif
#include "pal_common.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
 #if IFX_I2C_LOG_PAL == 1
#define LOG_PAL IFX_I2C_LOG
#else
#include <stdio.h>
#define LOG_PAL(...) //printf(__VA_ARGS__)
#endif
/**********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/
int usb_hid_set_feature(uint8_t report_id, uint8_t * data, uint8_t length, pal_usb_t * usb_events)
{
    int status;
    uint8_t report[HID_REPORT_SIZE] = {0};
    report[0] = report_id;
    memcpy(&report[1], data, length);
    LOG_PAL("usb_hid_set_feature\n. ");
    status = libusb_control_transfer(usb_events->handle,
                                     LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                     HID_REQ_SET_REPORT,
                                     (HID_REPORT_TYPE_FEATURE << 8) | report_id, USB_INTERFACE,
                                     report,
                                     sizeof(report),
                                     USB_TIMEOUT);
    LOG_PAL("[IFX-HAL]: HID Set Feature status = %d\n", status);

    if (status >= (1 + length))
    {
        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_I2C_EVENT_ERROR;
    }
}


uint16_t ifx_i2c_usb_reset(pal_usb_t usb_events)
{
    uint8_t payload[3];
    LOG_PAL("ifx_i2c_usb_reset\n. ");
    payload[0] = 0x20; // I2C Reset
	
    if (usb_hid_set_feature(REPORT_ID_SYSTEM_SETTING, payload, 1, &usb_events) != PAL_STATUS_SUCCESS)
        return PAL_I2C_EVENT_ERROR;

    payload[0] = 0x22; // I2C Clock Speed
    payload[1] = 0x90;//0xE8;//0x90;//0x64; // Value (LSB)
    payload[2] = 0x01;//0x03;//0x01;//0x00; // Value (MSB)
    if (usb_hid_set_feature(REPORT_ID_SYSTEM_SETTING, payload, 3, &usb_events) != PAL_STATUS_SUCCESS)
        return PAL_I2C_EVENT_ERROR;

    return PAL_STATUS_SUCCESS;
}

int usb_hid_get_feature(uint8_t report_id, uint8_t * report, pal_usb_t * usb_events)
{
    int i;
    int status;
    LOG_PAL("usb_hid_get_feature\n. ");
    report[0] = report_id;
	
	if (usb_events == NULL)
		return -1;
	
    status = libusb_control_transfer(usb_events->handle,
                                     LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                     HID_REQ_GET_REPORT,
                                     (HID_REPORT_TYPE_FEATURE << 8) | report_id, USB_INTERFACE,
                                     report,
                                     HID_REPORT_SIZE,
                                     USB_TIMEOUT);

    LOG_PAL("[IFX-HAL]: HID Get Feature status %d: ", status);
    for (i = 0; i < status; i++)
    {
        LOG_PAL("%02X ", report[i]);
    }
    LOG_PAL("   ");
    return status;
}

uint16_t usb_set_gpio_reset_pin(uint8_t high, pal_gpio_t * p_gpio_context)
{
    pal_usb_t * pal_usb;
    uint8_t report[HID_REPORT_SIZE] = {0};
    pal_usb = (pal_usb_t * )&p_gpio_context->p_gpio_hw;
    LOG_PAL("usb_set_gpio_reset_pin\n. ");
    if (usb_hid_get_feature(REPORT_ID_GPIO, report, pal_usb) != 5)
    {
        return PAL_I2C_EVENT_ERROR;
    }

    // DIO9 (DTNR / GPIOF) => OPTIGA Reset PIN
    if (high)
    {
        report[3] |= (1 << 5); // enable GPIOF
    }
    else
    {
        report[3] &= ~(1 << 5); // disable GPIOF
    }

    report[4] |= (1 << 5); // GPIOA-H direction output

    return usb_hid_set_feature(REPORT_ID_GPIO, report + 1, 4, pal_usb);
}

void print_status(uint8_t s)
{
    if (s & 0x01)
    {
        LOG_PAL("Busy; other bits invalid. ");
    }
    else
    {
        LOG_PAL("Idle. ");
        if (s & 0x02)
        {
            LOG_PAL("Error. ");
        }
        if (s & 0x04)
        {
            LOG_PAL("Addr NACK. ");
        }
        if (s & 0x08)
        {
            LOG_PAL("Data NACK. ");
        }
        if (s & 0x10)
        {
            LOG_PAL("Arb Lost. ");
        }
        if (s & 0x20)
        {
            LOG_PAL("Idle. ");
        }
        if (s & 0x40)
        {
            LOG_PAL("Bus Busy. ");
        }
    }
    LOG_PAL("\n");
}
