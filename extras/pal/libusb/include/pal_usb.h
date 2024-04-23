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
*
* \file
*
* \brief This file implements the prototype declarations of pal i2c
*
* \ingroup  grPAL
* @{
*/

#ifndef _PAL_USB_H_
#define _PAL_USB_H_

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#include "pal.h"
#ifdef __WIN32__
#include "libusb.h"
#else // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif
#include "pal_i2c.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

#define PAL_I2C_MASTER_MAX_BITRATE  (400)
// USB Vendor and Product IDs
#define USB_VID                     0x0403 // FTDI
#define USB_PID                     0x6030 // FT260

// USB device settings
#define USB_INTERFACE               0
#define USB_TIMEOUT                 2000

// USB HID report size (fixed for static buffer placement)
#define HID_REPORT_SIZE             64

// USB HID constants
#define HID_REQ_GET_REPORT          0x01
#define HID_REQ_SET_REPORT          0x09
#define HID_REPORT_TYPE_FEATURE     0x03

// FT260 HID Report IDs
#define REPORT_ID_SYSTEM_SETTING    0xA1
#define REPORT_ID_GPIO              0xB0
#define REPORT_ID_I2C_STATUS        0xC0
#define REPORT_ID_I2C_READ_REQ      0xC2
#define REPORT_ID_I2C_WRITE_REQ     0xDE

// FT260 I2C Flags
#define I2C_FLAG_START              0x02
#define I2C_FLAG_STOP               0x04

// FT260 I2C Status Bits
#define I2C_STATUS_CONTROLLER_BUSY  0x01
#define I2C_STATUS_ERROR_CONDITION  0x1E
#define I2C_STATUS_CONTROLLER_IDLE  0x20
#define I2C_STATUS_BUS_BUSY         0x40

#define false 0
#define true 1

/**********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * DATA STRUCTURES
 *********************************************************************************************************************/

/*
* context_type as below
* PAL_I2C_CONTEXT         (0x01)
* PAL_RESET_GPIO_CONTEXT  (0x02)
*/
LIBRARY_EXPORTS void get_pal_context(void *_optiga_pal_i2c_context_0,int32_t context_type);

/** @brief PAL I2C context structure */
typedef struct pal_usb
{
    /// Handle for the usb device
    libusb_device_handle* handle;
    /// Endpoint to read from device 
    uint8_t hid_ep_in;
    /// Endpoint to write to device
    uint8_t hid_ep_out;
   
} pal_usb_t;

#endif
