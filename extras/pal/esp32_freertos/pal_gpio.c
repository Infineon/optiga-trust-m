/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
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

#include "driver/gpio.h"

// lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        gpio_set_direction((gpio_num_t)p_gpio_context->p_gpio_hw, GPIO_MODE_OUTPUT);
        gpio_set_level((gpio_num_t)p_gpio_context->p_gpio_hw, 1);
    }

    return PAL_STATUS_SUCCESS;
}

// lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        gpio_set_level((gpio_num_t)p_gpio_context->p_gpio_hw, 1);
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        gpio_set_level((gpio_num_t)p_gpio_context->p_gpio_hw, 0);
    }
}

/**
 * @}
 */
