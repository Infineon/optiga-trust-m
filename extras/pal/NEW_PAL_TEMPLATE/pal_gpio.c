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

pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
        // Your function to set the pin's mode
    }
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
        // Your function to reset the pin's mode to e.g. defaults
    }
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
        // Your function to set the pin high
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
        // Your function to set the pin low
    }
}

/**
 * @}
 */
