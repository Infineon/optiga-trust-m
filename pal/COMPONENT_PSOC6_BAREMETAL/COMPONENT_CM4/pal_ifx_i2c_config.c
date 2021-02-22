/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file pal_ifx_i2c_config.c
*
* \brief   This file implements platform abstraction layer configurations for ifx i2c protocol.
*
* \ingroup  grPAL
*
* @{
*/

//#include <DAVE.h>
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

// i2c driver related
cyhal_i2c_t i2c_master_obj;

/**
 * \brief Structure defines PSOC6 gpio pin configuration.
 */
typedef struct pal_psoc_gpio
{
    uint8_t port_num;
    bool_t init_state;

} pal_psoc_gpio_t;

/**
 * \brief Structure defines PSOC6 gpio pin configuration.
 */
typedef struct pal_psoc_i2c
{
    cyhal_i2c_t *     i2c_master_channel;
    uint32_t         sda_port_num;
    uint32_t         sca_port_num;
}   pal_psoc_i2c_t;

pal_psoc_gpio_t optiga_vdd_config =
{
    .port_num = P5_5,
    .init_state = false
};

pal_psoc_gpio_t optiga_reset_config =
{
    .port_num = P5_6,
    .init_state = true
};

pal_psoc_i2c_t optiga_i2c_master_config =
{
    .i2c_master_channel = &i2c_master_obj,
    .sca_port_num = P6_0,
    .sda_port_num = P6_1
};

/**
* \brief PAL vdd pin configuration for OPTIGA. 
 */
pal_gpio_t optiga_vdd_0 =
{
    // Platform specific GPIO context for the pin used to toggle Vdd.
    (void*)&optiga_vdd_config
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 =
{
    // Platform specific GPIO context for the pin used to toggle Reset.
    (void*)&optiga_reset_config
};

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 =
{
    /// Pointer to I2C master platform specific context
    (void*)&optiga_i2c_master_config,
    /// Slave address
    0x30,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL
};


/**
* @}
*/
