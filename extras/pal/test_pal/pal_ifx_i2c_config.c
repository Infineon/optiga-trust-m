/**
* \copyright
* MIT License
*
* Copyright (c) 2024 Infineon Technologies AG
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

#include "pal_gpio.h"
#include "pal_i2c.h"
#include "ifx_i2c_config.h"

/*!< gpio number for I2C master clock */
 #ifndef CONFIG_PAL_I2C_MASTER_SCL_IO
	#define PAL_I2C_MASTER_SCL_IO           22               
#else
	#define PAL_I2C_MASTER_SCL_IO			0 // dummy
#endif

/*!< gpio number for I2C master data  */
#ifndef CONFIG_PAL_I2C_MASTER_SDA_IO
	#define PAL_I2C_MASTER_SDA_IO           21              
#else
	#define PAL_I2C_MASTER_SDA_IO			0 // dummy
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_RESET
	#define PAL_I2C_MASTER_RESET            25
#else
	#define PAL_I2C_MASTER_RESET			0 // dummy
#endif

/*!< gpio number for software reset line  */
#ifndef CONFIG_PAL_I2C_MASTER_VCC
	#define PAL_I2C_MASTER_VCC           26
#else
	#define PAL_I2C_MASTER_VCC			0 // dummy
#endif

/*!< I2C port number for master dev */
#ifndef CONFIG_PAL_I2C_MASTER_NUM
	#define PAL_I2C_MASTER_NUM              0 // dummy        
#else
	#define PAL_I2C_MASTER_NUM				0 // dummy
#endif

/*!< I2C master clock frequency */
#ifndef CONFIG_PAL_I2C_MASTER_FREQ_HZ
	#define PAL_I2C_MASTER_FREQ_HZ          400000           
#else
	#define PAL_I2C_MASTER_FREQ_HZ			0 // dummy
#endif


/**
 * \brief PAL I2C configuration for OPTIGA. 
 */
pal_i2c_t optiga_pal_i2c_context_0 =
{
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
pal_gpio_t optiga_vdd_0 =
{
    // Platform specific GPIO context for the pin used to toggle Vdd, dummy component = no hardware
    NULL 
};

/**
 * \brief PAL reset pin configuration for OPTIGA.
 */
pal_gpio_t optiga_reset_0 =
{
    // Platform specific GPIO context for the pin used to toggle Reset, dummy component = no hardware
    NULL
};

/**
* @}
*/
