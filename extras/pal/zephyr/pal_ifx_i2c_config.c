/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_ifx_i2c_config.c
 *
 * \brief   This file implements platform abstraction layer configurations for
 * ifx i2c protocol.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "pal_gpio.h"
#include "pal_i2c.h"

#define I2C_NODE DT_ALIAS(optiga_i2c)
#if !DT_NODE_HAS_STATUS(I2C_NODE, okay)
#error "Unsupported board: arduino_i2c devicetree alias is not defined"
#endif

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_RST_SUPPORT
#define GPIO_RESET_NODE DT_ALIAS(optiga_reset)
static struct gpio_dt_spec reset_gpio_spec = GPIO_DT_SPEC_GET(GPIO_RESET_NODE, gpios);
#endif

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_VDD_SUPPORT
#define GPIO_VDD_NODE DT_ALIAS(optiga_vdd)
static struct gpio_dt_spec vdd_gpio_spec = GPIO_DT_SPEC_GET(GPIO_VDD_NODE, gpios);
#endif

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context
    (void *)DEVICE_DT_GET(I2C_NODE),
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
    /// Slave address
    0x30,
};

/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_VDD_SUPPORT
    (void *)&vdd_gpio_spec
#else
    (void *)NULL
#endif
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_RST_SUPPORT
    (void *)&reset_gpio_spec
#else
    (void *)NULL
#endif
};
