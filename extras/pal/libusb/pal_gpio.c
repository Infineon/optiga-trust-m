/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
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

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
//#include <DAVE.h>
#include "pal_gpio.h"

#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "pal_common.h"
#include "pal_usb.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

// lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context) {
    return PAL_STATUS_SUCCESS;
}

// lint --e{714,715} suppress "This function is used for to support multiple platforms "
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context) {
    return PAL_STATUS_SUCCESS;
}

/**
 * Sets the gpio pin to high state
 *
 * <b>API Details:</b>
 *      The API sets the pin high, only if the pin is assigned to a valid gpio context.<br>
 *      Otherwise the API returns without any faliure status.<br>
 *
 *\param[in] p_gpio_context Pointer to pal layer gpio context
 *
 *
 */
void pal_gpio_set_high(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        usb_set_gpio_reset_pin(1, p_gpio_context->p_gpio_hw);
    }
}

/**
 * Sets the gpio pin to low state
 *
 * <b>API Details:</b>
 *      The API set the pin low, only if the pin is assigned to a valid gpio context.<br>
 *      Otherwise the API returns without any faliure status.<br>
 *
 *\param[in] p_gpio_context Pointer to pal layer gpio context
 *
 */
void pal_gpio_set_low(const pal_gpio_t *p_gpio_context) {
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        usb_set_gpio_reset_pin(0, p_gpio_context->p_gpio_hw);
    }
}

/**
 * @}
 */
