/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \file
 *
 * \brief This file implements the platform abstraction layer APIs for gpio.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "pal_gpio.h"

#include "xmc4_gpio.h"
#include "xmc_gpio.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
typedef struct DIGITAL_IO {
    uint8_t init_flag;
    XMC_GPIO_PORT_t *const gpio_port; /**< port number */
    const XMC_GPIO_CONFIG_t
        gpio_config; /**< mode, initial output level and pad driver strength / hysteresis */
    const uint8_t gpio_pin; /**< pin number */
    const XMC_GPIO_HWCTRL_t hwctrl; /**< Hardware port control */
} DIGITAL_IO_t;

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

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
    DIGITAL_IO_t *p_io = NULL;
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        p_io = (DIGITAL_IO_t *)(p_gpio_context->p_gpio_hw);

        if (p_io->init_flag == 0) {
            /* Initializes input / output characteristics */
            XMC_GPIO_Init(p_io->gpio_port, p_io->gpio_pin, &p_io->gpio_config);

            /*Configure hardware port control*/
            XMC_GPIO_SetHardwareControl(p_io->gpio_port, p_io->gpio_pin, p_io->hwctrl);
            p_io->init_flag = 1;
        }

        XMC_GPIO_SetOutputHigh(p_io->gpio_port, p_io->gpio_pin);
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
    DIGITAL_IO_t *p_io = NULL;
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL)) {
        p_io = (DIGITAL_IO_t *)(p_gpio_context->p_gpio_hw);

        if (p_io->init_flag == 0) {
            /* Initializes input / output characteristics */
            XMC_GPIO_Init(p_io->gpio_port, p_io->gpio_pin, &p_io->gpio_config);

            /*Configure hardware port control*/
            XMC_GPIO_SetHardwareControl(p_io->gpio_port, p_io->gpio_pin, p_io->hwctrl);
            p_io->init_flag = 1;
        }

        XMC_GPIO_SetOutputLow(p_io->gpio_port, p_io->gpio_pin);
    }
}

/**
 * @}
 */
