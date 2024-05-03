/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \file
 *
 * \brief This file implements platform abstraction layer configurations for ifx i2c protocol.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "I2C_MASTER/i2c_master.h"
#include "pal_gpio.h"
#include "pal_i2c.h"
#include "xmc4_gpio.h"
#include "xmc_gpio.h"

/**
 * @brief Initialization data structure of DIGITAL_IO APP
 */
typedef struct DIGITAL_IO {
    uint8_t init_flag;
    XMC_GPIO_PORT_t *const gpio_port; /**< port number */
    const XMC_GPIO_CONFIG_t
        gpio_config; /**< mode, initial output level and pad driver strength / hysteresis */
    const uint8_t gpio_pin; /**< pin number */
    const XMC_GPIO_HWCTRL_t hwctrl; /**< Hardware port control */
} DIGITAL_IO_t;

const DIGITAL_IO_t pin_rst = {
    .gpio_port = XMC_GPIO_PORT0,
    .gpio_pin = 6U,
    .gpio_config =
        {
            .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
            .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,

        },
    .hwctrl = XMC_GPIO_HWCTRL_DISABLED};

extern I2C_MASTER_t i2c_master_0;
/*********************************************************************************************************************
 * pal ifx i2c instance
 *********************************************************************************************************************/
/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context
    (void *)&i2c_master_0,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
    /// Slave address
    0x30};

/*********************************************************************************************************************
 * PAL GPIO configurations defined for XMC4500
 *********************************************************************************************************************/
/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void *)NULL};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void *)&pin_rst};

/**
 * @}
 */
