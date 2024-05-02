/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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

#include "pal_gpio.h"

#include <zephyr/drivers/gpio.h>

#include "pal.h"

pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *)p_gpio_context->p_gpio_hw;
        if (gpio_pin_configure(gpio_spec->port, gpio_spec->pin, GPIO_OUTPUT) != 0) {
            return PAL_STATUS_FAILURE;
        }
        return PAL_STATUS_SUCCESS;
    }
    return PAL_STATUS_FAILURE;
}

pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    (void)p_gpio_context;
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *)p_gpio_context->p_gpio_hw;

        if (gpio_pin_is_output(gpio_spec->port, gpio_spec->pin) == 0) {
            if (pal_gpio_init(p_gpio_context) != PAL_STATUS_SUCCESS) {
                return;
            }
        }
        gpio_pin_set(gpio_spec->port, gpio_spec->pin, 1);
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        struct gpio_dt_spec *gpio_spec = (struct gpio_dt_spec *)p_gpio_context->p_gpio_hw;

        if (gpio_pin_is_output(gpio_spec->port, gpio_spec->pin) == 0) {
            if (pal_gpio_init(p_gpio_context) != PAL_STATUS_SUCCESS) {
                return;
            }
        }
        gpio_pin_set(gpio_spec->port, gpio_spec->pin, 0);
    }
}
