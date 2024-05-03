/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
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

// !!!OPTIGA_LIB_PORTING_REQUIRED
typedef struct locl_i2c_struct_to_descroibe_master {
    // you parameters to control the master instance
    // See other implementation to get intuition on how to implement this part
} local_i2c_struct_to_descroibe_master_t;

local_i2c_struct_to_descroibe_master_t i2c_master_0;

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
    0x30,
};

/**
 * \brief PAL vdd pin configuration for OPTIGA.
 */
pal_gpio_t optiga_vdd_0 = {
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // Platform specific GPIO context for the pin used to toggle Vdd.
    // You should have vdd_pin define in your system,
    // alternativly you can put here raw GPIO number, but without the & sign
    (void *)&vdd_pin};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // Platform specific GPIO context for the pin used to toggle Reset.
    // You should have reset_pin define in your system,
    // alternativly you can put here raw GPIO number, but without the & sign
    (void *)&reset_pin};

/**
 * @}
 */
