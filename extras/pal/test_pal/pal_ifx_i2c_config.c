/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_ifx_i2c_config.c
 *
 * \brief   This file implements platform abstraction layer configurations for ifx i2c protocol.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "ifx_i2c_config.h"
#include "pal_gpio.h"
#include "pal_i2c.h"

/*!< gpio number for I2C master clock */
#ifndef CONFIG_PAL_I2C_MASTER_SCL_IO
#define PAL_I2C_MASTER_SCL_IO 22
#else
#define PAL_I2C_MASTER_SCL_IO 0  // dummy
#endif

/*!< gpio number for I2C master data  */
#ifndef CONFIG_PAL_I2C_MASTER_SDA_IO
#define PAL_I2C_MASTER_SDA_IO 21
#else
#define PAL_I2C_MASTER_SDA_IO 0  // dummy
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_RESET
#define PAL_I2C_MASTER_RESET 25
#else
#define PAL_I2C_MASTER_RESET 0  // dummy
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_VCC
#define PAL_I2C_MASTER_VCC 26
#else
#define PAL_I2C_MASTER_VCC 0  // dummy
#endif

/*!< I2C port number for master dev */
#ifndef CONFIG_PAL_I2C_MASTER_NUM
#define PAL_I2C_MASTER_NUM 0  // dummy
#else
#define PAL_I2C_MASTER_NUM 0  // dummy
#endif

/*!< I2C master clock frequency */
#ifndef CONFIG_PAL_I2C_MASTER_FREQ_HZ
#define PAL_I2C_MASTER_FREQ_HZ 400000
#else
#define PAL_I2C_MASTER_FREQ_HZ 0  // dummy
#endif

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context, dummy component = no hardware
    NULL,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
    /// Slave address
    0x30,
};

/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
    // Platform specific GPIO context for the pin used to toggle Vdd, dummy component = no hardware
    NULL};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset, dummy component = no hardware
    NULL};

/**
 * @}
 */
