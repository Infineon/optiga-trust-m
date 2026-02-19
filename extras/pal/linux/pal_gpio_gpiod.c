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
#if defined(LIBGPIOD_V1)
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
#else
    struct gpiod_chip *chip = NULL;
    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config *line_cfg = NULL;
    struct gpiod_request_config *req_cfg = NULL;
    struct gpiod_line_request *request = NULL;
    unsigned int offset;
    int ret = -1;

    offset = (unsigned int)pin->gpio_device_offset;

    chip = gpiod_chip_open(pin->gpio_device);
    if (!chip) {
        fprintf(stderr, "Failed to open chip %s: %s\n", pin->gpio_device, strerror(errno));
        goto cleanup;
    }

    settings = gpiod_line_settings_new();
    if (!settings) {
        goto cleanup;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(
        settings,
        value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE
    );

    line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        goto cleanup;
    }

    if (gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings) < 0) {
        fprintf(stderr, "add_line_settings failed: %s\n", strerror(errno));
        goto cleanup;
    }
    req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        goto cleanup;
    }

    gpiod_request_config_set_consumer(req_cfg, "trustm");

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if (!request) {
        fprintf(
            stderr,
            "Request_lines failed (dev=%s offset=%u): %s\n",
            pin->gpio_device,
            offset,
            strerror(errno)
        );
        goto cleanup;
    }
    ret = 0;
cleanup:
    if (request)
        gpiod_line_request_release(request);
    if (settings)
        gpiod_line_settings_free(settings);
    if (line_cfg)
        gpiod_line_config_free(line_cfg);
    if (req_cfg)
        gpiod_request_config_free(req_cfg);
    if (chip)
        gpiod_chip_close(chip);
    return ret;

#endif
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
