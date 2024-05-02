/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_gpio_gpiod.c
 *
 * \brief   This file implements the platform abstraction layer APIs for GPIO.
 *
 * \ingroup  grPAL
 * @{
 */
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "errno.h"
#include "pal_gpio.h"
#include "pal_ifx_i2c_config.h"
#include "pal_linux.h"

#define LOW 0
#define HIGH 1

static int GPIOWrite(pal_linux_gpio_gpiod_t *pin, int value) {
    int ret = 0;
    ret = gpiod_ctxless_set_value(
        pin->gpio_device,
        pin->gpio_device_offset,
        value,
        false,
        "trustm",
        NULL,
        NULL
    );

    if (ret != 0) {
        char err_msg[100];
        sprintf(err_msg, "Failed to write value! Return code = %d\n", ret);
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        return -1;
    }

    return 0;
}

// lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    return PAL_STATUS_SUCCESS;
}

// lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        /*
         * Write GPIO value
         */
        if (GPIOWrite((pal_linux_gpio_gpiod_t *)(p_gpio_context->p_gpio_hw), HIGH) != 0) {
            fprintf(stderr, "Unable to set gpio pin high!");
        }
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        /*
         * Write GPIO value
         */
        if (GPIOWrite((pal_linux_gpio_gpiod_t *)(p_gpio_context->p_gpio_hw), LOW) != 0) {
            fprintf(stderr, "Unable to set gpio pin low!");
        }
    }
}

/**
 * @}
 */
