/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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

#ifdef __WIN32__
#include "libusb.h"
#else  // LINUX
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#endif

#include "optiga_lib_logger.h"
#include "pal_common.h"
#include "pal_gpio.h"
#include "pal_usb.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
#if IFX_I2C_LOG_PAL == 1
#define LOG_PAL IFX_I2C_LOG
#else
#include <stdio.h>
#define LOG_PAL(...)  // printf(__VA_ARGS__)
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
int usb_hid_set_feature(uint8_t report_id, uint8_t *data, uint8_t length, pal_usb_t *usb_events) {
    int status;
    uint8_t report[HID_REPORT_SIZE] = {0};

    report[0] = report_id;

    memcpy(&report[1], data, length);
    LOG_PAL("%sUSB HID Set Feature\n", OPTIGA_PAL_LAYER);

    status = libusb_control_transfer(
        usb_events->handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
        HID_REQ_SET_REPORT,
        (HID_REPORT_TYPE_FEATURE << 8) | report_id,
        USB_INTERFACE,
        report,
        sizeof(report),
        USB_TIMEOUT
    );

    LOG_PAL("%sHID Set Feature status: %d\n", OPTIGA_PAL_LAYER, status);

    if (status >= (1 + length)) {
        return PAL_STATUS_SUCCESS;
    } else {
        return PAL_I2C_EVENT_ERROR;
    }
}

uint16_t ifx_i2c_usb_reset(pal_usb_t usb_events) {
    uint8_t payload[3];

    LOG_PAL("%sUSB I2C Reset\n", OPTIGA_PAL_LAYER);

    payload[0] = 0x20;  // I2C Reset

    if (usb_hid_set_feature(REPORT_ID_SYSTEM_SETTING, payload, 1, &usb_events)
        != PAL_STATUS_SUCCESS)
        return PAL_I2C_EVENT_ERROR;

    payload[0] = 0x22;  // I2C Clock Speed
    payload[1] = 0x90;  // 0xE8;//0x90;//0x64; // Value (LSB)
    payload[2] = 0x01;  // 0x03;//0x01;//0x00; // Value (MSB)
    if (usb_hid_set_feature(REPORT_ID_SYSTEM_SETTING, payload, 3, &usb_events)
        != PAL_STATUS_SUCCESS)
        return PAL_I2C_EVENT_ERROR;

    return PAL_STATUS_SUCCESS;
}

int usb_hid_get_feature(uint8_t report_id, uint8_t *report, pal_usb_t *usb_events) {
    int status;

    LOG_PAL("%sUSB HID Get Feature\n", OPTIGA_PAL_LAYER);

    report[0] = report_id;

    if (usb_events == NULL)
        return -1;

    status = libusb_control_transfer(
        usb_events->handle,
        LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
        HID_REQ_GET_REPORT,
        (HID_REPORT_TYPE_FEATURE << 8) | report_id,
        USB_INTERFACE,
        report,
        HID_REPORT_SIZE,
        USB_TIMEOUT
    );

    LOG_PAL("%sHID Set Feature status = %d\n", OPTIGA_PAL_LAYER, status);

    LOG_PAL("%sHID Set Feature report: ", OPTIGA_PAL_LAYER);
    int i;
    for (i = 0; i < status; i++) {
        LOG_PAL("%02X ", report[i]);
    }
    LOG_PAL("\n");

    return status;
}

uint16_t usb_set_gpio_reset_pin(uint8_t high, pal_gpio_t *p_gpio_context) {
    pal_usb_t *pal_usb;
    uint8_t report[HID_REPORT_SIZE] = {0};
    pal_usb = (pal_usb_t *)&p_gpio_context->p_gpio_hw;
    LOG_PAL("USB Set GPIO Reset Pin\n");
    if (usb_hid_get_feature(REPORT_ID_GPIO, report, pal_usb) != 5) {
        return PAL_I2C_EVENT_ERROR;
    }

    // DIO9 (DTNR / GPIOF) => OPTIGA Reset PIN
    if (high) {
        report[3] |= (1 << 5);  // enable GPIOF
    } else {
        report[3] &= ~(1 << 5);  // disable GPIOF
    }

    report[4] |= (1 << 5);  // GPIOA-H direction output

    return usb_hid_set_feature(REPORT_ID_GPIO, report + 1, 4, pal_usb);
}
