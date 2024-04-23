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
 * \file pal_gpio.c
 *
 * \brief   This file implements the platform abstraction layer APIs for GPIO.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal.h"
#include "pal_gpio.h"
#include <zephyr/drivers/gpio.h>

pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *) p_gpio_context->p_gpio_hw;
        if (gpio_pin_configure(gpio_spec->port, gpio_spec->pin, GPIO_OUTPUT) != 0)
        {
            return PAL_STATUS_FAILURE;
        }
        return PAL_STATUS_SUCCESS;
    }
    return PAL_STATUS_FAILURE;
}

pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context)
{
    (void) p_gpio_context;
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *) p_gpio_context->p_gpio_hw;

        if (gpio_pin_is_output(gpio_spec->port, gpio_spec->pin) == 0)
        {
            if (pal_gpio_init(p_gpio_context) != PAL_STATUS_SUCCESS)
            {
                return;
            }
        }
        gpio_pin_set(gpio_spec->port, gpio_spec->pin, 1);
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *) p_gpio_context->p_gpio_hw;

        if (gpio_pin_is_output(gpio_spec->port, gpio_spec->pin) == 0)
        {
            if (pal_gpio_init(p_gpio_context) != PAL_STATUS_SUCCESS)
            {
                return;
            }
        }
        gpio_pin_set(gpio_spec->port, gpio_spec->pin, 0);
    }
}
