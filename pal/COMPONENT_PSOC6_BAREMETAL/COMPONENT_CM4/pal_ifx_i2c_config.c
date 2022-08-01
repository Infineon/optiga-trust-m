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

#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_i2c.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "pal_psoc_i2c_mapping.h"
#include "pal_psoc_gpio_mapping.h"
#include "optiga_lib_config.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

// i2c driver related
cyhal_i2c_t i2c_master_obj;

#ifdef OPTIGA_TRUSTM_VDD
pal_psoc_gpio_t optiga_vdd_config =
{
    .port_num = OPTIGA_TRUSTM_VDD,
    .init_state = true
};
#endif

#ifdef OPTIGA_TRUSTM_RST
pal_psoc_gpio_t optiga_reset_config =
{
    .port_num = OPTIGA_TRUSTM_RST,
    .init_state = true
};
#endif

pal_psoc_i2c_t optiga_i2c_master_config =
{
    .i2c_master_channel = &i2c_master_obj,
    .sca_port_num = OPTIGA_TRUSTM_SCL,
    .sda_port_num = OPTIGA_TRUSTM_SDA
};

/**
* \brief PAL vdd pin configuration for OPTIGA. 
 */
pal_gpio_t optiga_vdd_0 =
{
#ifdef OPTIGA_TRUSTM_VDD
	// Platform specific GPIO context for the pin used to toggle Vdd.
	(void * )&optiga_vdd_config
#else
    NULL
#endif
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 =
{
#ifdef OPTIGA_TRUSTM_RST
    // Platform specific GPIO context for the pin used to toggle Reset.
	(void * )&optiga_reset_config
#else
	NULL
#endif
};

/**
 * \brief PAL I2C configuration for OPTIGA.
 */
pal_i2c_t optiga_pal_i2c_context_0 =
{
    /// Pointer to I2C master platform specific context
    (void*)&optiga_i2c_master_config,
    /// Upper layer context
    NULL,
    /// Callback event handler
    NULL,
    /// Slave address
    0x30
};


/**
* @}
*/
