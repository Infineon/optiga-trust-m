/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_common.h
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

#ifdef WIN32
#include "libusb.h"
#else  // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif

#include "pal.h"
#include "pal_gpio.h"
#include "pal_i2c.h"
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
uint16_t usb_set_gpio_reset_pin(uint8_t high, pal_gpio_t *pin);
int usb_hid_get_feature(uint8_t report_id, uint8_t *report, pal_usb_t *usb_events);
uint16_t ifx_i2c_usb_reset(pal_usb_t usb_events);
int usb_hid_set_feature(uint8_t report_id, uint8_t *data, uint8_t length, pal_usb_t *usb_events);
void print_status(uint8_t s);

#endif
