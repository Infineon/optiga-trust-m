/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_ifx_i2c_config.c
 *
 * \brief   This file implements platform abstraction layer configurations for ifx i2c protocol.
 *
 * \ingroup  grPAL
 * @{
 */

#include "ifx_i2c_config.h"
#include "pal_gpio.h"
#include "pal_i2c.h"
#include "pal_linux.h"

pal_linux_t linux_events = {"/dev/i2c-1", 0, NULL};

#define GPIO_PIN_RESET 16
#define GPIO_PIN_VDD 27
#define GPIO_CHIP "/dev/gpiochip0"

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context
    (void *)&linux_events,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
    /// Slave address
    0x30};

static struct pal_linux_gpio_gpiod pin_reset = {GPIO_CHIP, GPIO_PIN_RESET};
static struct pal_linux_gpio_gpiod pin_vdd = {GPIO_CHIP, GPIO_PIN_VDD};

/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void *)&pin_vdd};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void *)&pin_reset};

/**
 * @}
 */
