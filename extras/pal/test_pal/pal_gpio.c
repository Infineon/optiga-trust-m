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
 * @{
 */

#include "pal_gpio.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "errno.h"
#include "pal_ifx_i2c_config.h"
#include "pal_linux.h"

/**
 * @brief Initializes the PAL GPIO layer
 *
 * Initializes the PAL GPIO layer.
 * <br>
 *
 *<b>API Details:</b>
 * - Initializes the PAL GPIO layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    /* Dummy component*/
    return PAL_STATUS_SUCCESS;
}

/**
 * @brief Deinitializes the PAL GPIO layer
 *
 * Deinitializes the PAL GPIO layer.
 * <br>
 *
 *<b>API Details:</b>
 * - Deinitializes the PAL GPIO layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    /* Dummy component*/
    return PAL_STATUS_SUCCESS;
}

/**
 * @brief Sets GPIO Pin High
 *
 * Sets GPIO Pin High.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets GPIO Pin High<br>
 *
 *<b>User Input:</b><br>
 * - GPIO context
 *
 * \retval  None
 */
void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    /* Dummy component*/
    return;
}

/**
 * @brief Sets GPIO Pin Low
 *
 * Sets GPIO Pin High.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets GPIO Pin High<br>
 *
 *<b>User Input:</b><br>
 * - GPIO context
 *
 * \retval  None
 */
void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    /* Dummy component*/
    return;
}

/**
 * @}
 */
