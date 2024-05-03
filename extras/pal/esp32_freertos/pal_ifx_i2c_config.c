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

#include "driver/i2c.h"
#include "ifx_i2c_config.h"
#include "pal_gpio.h"
#include "pal_i2c.h"

/*!< gpio number for I2C master clock */
#ifndef CONFIG_PAL_I2C_MASTER_SCL_IO
#define PAL_I2C_MASTER_SCL_IO 22
#else
#define PAL_I2C_MASTER_SCL_IO CONFIG_PAL_I2C_MASTER_SCL_IO
#endif

/*!< gpio number for I2C master data  */
#ifndef CONFIG_PAL_I2C_MASTER_SDA_IO
#define PAL_I2C_MASTER_SDA_IO 21
#else
#define PAL_I2C_MASTER_SDA_IO CONFIG_PAL_I2C_MASTER_SDA_IO
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_RESET
#define PAL_I2C_MASTER_RESET 25
#else
#define PAL_I2C_MASTER_RESET CONFIG_PAL_I2C_MASTER_RESET
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_VCC
#define PAL_I2C_MASTER_VCC 26
#else
#define PAL_I2C_MASTER_VCC CONFIG_PAL_I2C_MASTER_VCC
#endif

/*!< I2C port number for master dev */
#ifndef CONFIG_PAL_I2C_MASTER_NUM
#define PAL_I2C_MASTER_NUM I2C_NUM_0
#else
#define PAL_I2C_MASTER_NUM CONFIG_PAL_I2C_MASTER_NUM
#endif

/*!< I2C master clock frequency */
#ifndef CONFIG_PAL_I2C_MASTER_FREQ_HZ
#define PAL_I2C_MASTER_FREQ_HZ 400000
#else
#define PAL_I2C_MASTER_FREQ_HZ CONFIG_PAL_I2C_MASTER_FREQ_HZ
#endif

typedef struct esp32_i2c_ctx {
    uint8_t port;
    uint8_t scl_io;
    uint8_t sda_io;
    uint32_t bitrate;
} esp32_i2c_ctx_t;

esp32_i2c_ctx_t esp32_i2c_ctx_0 = {
    PAL_I2C_MASTER_NUM,
    PAL_I2C_MASTER_SCL_IO,
    PAL_I2C_MASTER_SDA_IO,
    PAL_I2C_MASTER_FREQ_HZ};

esp32_i2c_ctx_t esp32_i2c_ctx_0;

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 = {
    /// Pointer to I2C master platform specific context
    (void *)&esp32_i2c_ctx_0,
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
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void *)PAL_I2C_MASTER_VCC};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 = {
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void *)PAL_I2C_MASTER_RESET};

/**
 * @}
 */
