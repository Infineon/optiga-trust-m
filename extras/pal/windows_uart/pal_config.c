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

/*********************************************************************************************************************
 * pal ifx i2c instance
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * PAL GPIO configurations defined for XMC4500
 *********************************************************************************************************************/
/**
* \brief PAL vdd pin configuration for OPTIGA. 
 */
pal_gpio_t optiga_vdd_0 = {
    // Platform specific GPIO context for the pin used to toggle Vdd.
    NULL};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    NULL};

/**
* @}
*/
