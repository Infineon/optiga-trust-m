/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_ifx_usb_config.c
 *
 * \brief   This file implements platform abstraction layer configurations for ifx i2c protocol.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
//#include <DAVE.h>
#include "pal_gpio.h"
#include "pal_i2c.h"
#include "pal_usb.h"

/*********************************************************************************************************************
 * pal ifx i2c instance
 *********************************************************************************************************************/
pal_usb_t usb_events = {0};
/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context
    (void *)&usb_events,
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
    //(void*)&pin_3_4
    NULL};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void *)&usb_events,
};

/**
 * @}
 */
