/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal.c
 *
 * \brief    This file implements the platform abstraction layer APIs.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "optiga/pal/pal.h"

#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_logger.h"
#include "optiga/pal/pal_os_timer.h"

extern pal_gpio_t optiga_vdd_0;
extern pal_gpio_t optiga_reset_0;

pal_status_t pal_init(void) {
    pal_status_t status = pal_logger_init(NULL);

    if (status != PAL_STATUS_SUCCESS) {
        return status;
    }

    status = pal_timer_init();

    if (status != PAL_STATUS_SUCCESS) {
        return status;
    }

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_VDD_SUPPORT
    status = pal_gpio_init(&optiga_vdd_0);

    if (status != PAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_RST_SUPPORT
    status = pal_gpio_init(&optiga_reset_0);
#endif
    return status;
}

pal_status_t pal_deinit(void) {
    return PAL_STATUS_SUCCESS;
}
