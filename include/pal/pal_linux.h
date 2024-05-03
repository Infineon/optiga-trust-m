/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_linux.h
 *
 * \brief   This file provides the prototype declarations w.r.t Linux.
 *
 * \ingroup  grPAL
 * @{
 */

#ifndef _PAL_LINUX_H_
#define _PAL_LINUX_H_

#include "pal.h"

#define false 0
#define true 1

#define HIGH 1
#define LOW 0

/** @brief PAL I2C context structure */
typedef struct pal_linux {
    /// I2C device file path, e.g. /dev/i2c-1
    const char *i2c_if;
    /// This field consists the handle for I2c device
    int i2c_handle;
    /// Pointer to store the callers handler
    void *upper_layer_event_handler;
} pal_linux_t;

#ifdef HAS_LIBGPIOD

typedef const char *gpio_device_t;
typedef uint16_t gpio_device_offset_t;

typedef struct pal_linux_gpio_gpiod {
    gpio_device_t gpio_device;
    gpio_device_offset_t gpio_device_offset;
} pal_linux_gpio_gpiod_t;

#else

typedef uint16_t gpio_pin_t;

typedef struct pal_linux_gpio {
    gpio_pin_t pin_nr;
    int fd;
} pal_linux_gpio_t;

#endif  // HAS_LIBGPIOD

#endif
