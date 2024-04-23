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

#ifndef _PAL_COMMON_H_
#define _PAL_COMMON_H_

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#include "pal.h"
#include "pal_i2c.h"
#include "pal_gpio.h"
#ifdef WIN32
#include "libusb.h"
#else // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif
#include "pal_usb.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * DATA STRUCTURES
 *********************************************************************************************************************/
uint16_t usb_set_gpio_reset_pin(uint8_t high,pal_gpio_t* pin);
int usb_hid_get_feature(uint8_t report_id, uint8_t* report,pal_usb_t* usb_events);
uint16_t ifx_i2c_usb_reset(pal_usb_t usb_events);
int usb_hid_set_feature(uint8_t report_id, uint8_t* data, uint8_t length,pal_usb_t* usb_events);
void print_status(uint8_t s);

#endif
