/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
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

// If you use Raspberry Pi, you can uncomment the following lines
// gpio_pin_t gpio_pin_vdd = 27;
// gpio_pin_t gpio_pin_reset = 17;

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

/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void *)NULL
    // If you use Raspberry Pi, you can uncomment the following line and comment out the previous
    //(void*)&gpio_pin_vdd
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void *)NULL
    // If you use Raspberry Pi, you can uncomment the following line and comment out the previous
    //(void*)&gpio_pin_reset
};

/**
 * @}
 */
