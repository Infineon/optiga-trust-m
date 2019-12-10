/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
*
* \author Infineon Technologies AG
*
* \file pal_gpio.c
*
* \brief   This file provides the prototype declarations w.r.t Zephyr OS.
*
* \ingroup  grPAL
*
* @{
*/

#ifndef _PAL_ZEPHYR_H
#define _PAL_ZEPHYR_H

/* OPTIGA includes */
#include "optiga/pal/pal.h"

/* TODO: Make defines not static but depending on KCONFIG */
#define I2C_OPTIGA_ADDRESS  CONFIG_TRUSTM_I2C_ADDR


/* I2C PORT to be used to connection */
#define I2C_PORT_NAME     CONFIG_TRUSTM_I2C_PORT_NAME // DT_ALIAS_I2C_0_LABEL
/* I2C_0 pins on nRF52840_pca10056 */
#define I2C_SDA     26 /* 26 */
#define I2C_SCL     27 /* 27 */

/*
 * Zephyr OS I2C SPEED TABLE
 * I2C_SPEED_STANDARD   = 1 :=  100 KHz -> OPTIGA supported
 * I2C_SPEED_FAST       = 2 :=  400 KHz -> OPTIGA supported
 * I2C_SPEED_FAST_PLUS  = 3 :=  1.0 MHz ~> OPTIGA supported
 * I2C_SPEED_HIGH       = 4 :=  3.4 MHz
 * I2C_SPEED_ULTRA      = 5 :=  5.0 MHz
 * 
 */
#define I2C_FREQ_KHZ                    (100U)
#define PAL_I2C_MASTER_DEFAULT_BITRATE  (100U)
#define PAL_I2C_MASTER_FAST_BITRATE     (400U)
#define PAL_I2C_MASTER_MAX_BITRATE      (1000U)

/* Defines for semaphore mechanism to lock down acccess to i2c bus */
#define BUS_ENTRY_SUCCESS       0
#define BUS_ENTRY_INIT_VALUE    1
#define BUS_ENTRY_MAX_VALUE     1

/* GPIOs ports & pins used to control VDD and RST lines: e.g. on nRF52840_pca10056 */
#define VDD_PORT_NAME   CONFIG_TRUSTM_VDD_GPIO_PORT_NAME // DT_GPIO_P1_DEV_NAME // "GPIO_1"
#define RST_PORT_NAME   CONFIG_TRUSTM_RST_GPIO_PORT_NAME // DT_GPIO_P1_DEV_NAME // "GPIO_1"
#define VDD_PIN         CONFIG_TRUSTM_VDD_PIN // 7 -> P1.07
#define RST_PIN         CONFIG_TRUSTM_RST_PIN // 8 -> P1.08

/* Defines used for setting GPIO pins*/
#define LOW     0
#define HIGH    1

/* I2C device context */
typedef struct pal_zephyr_i2c {
    struct device  *p_i2c_dev;
    uint8_t         scl_pin;
    uint8_t         sda_pin;
    uint32_t        bitrate;
} pal_zephyr_i2c_t;

/* GPIO device context */
typedef struct pal_zephyr_gpio {
    struct device   *p_device;
    char            *p_port_name;
    uint8_t          pin;
    uint8_t          init_flag;
} pal_zephyr_gpio_t;

#endif
