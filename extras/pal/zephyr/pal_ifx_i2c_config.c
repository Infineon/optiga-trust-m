/**
 * \copyright
 * MIT License
 *
 * Copyright (c) 2024 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * \file pal_ifx_i2c_config.c
 *
 * \brief   This file implements platform abstraction layer configurations for
 * ifx i2c protocol.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_gpio.h"
#include "pal_i2c.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

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
    (void *) DEVICE_DT_GET(I2C_NODE),
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
    (void *) &vdd_gpio_spec
#else
    (void *) NULL
#endif
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_RST_SUPPORT
    (void *) &reset_gpio_spec
#else
    (void *) NULL
#endif
};
