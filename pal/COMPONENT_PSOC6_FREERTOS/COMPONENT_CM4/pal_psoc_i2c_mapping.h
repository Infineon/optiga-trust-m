/**
* \copyright
* MIT License
*
* Copyright (c) 2022 Infineon Technologies AG
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
* \file pal_psoc_i2c_mapping.h
*
* \brief   This file provides the PSOC specific I2C pin mapping.
*
* \ingroup  grPAL
*
* @{
*/

#ifndef PAL_PSOC_I2C_MAPPING
#define PAL_PSOC_I2C_MAPPING

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"
#include "cyhal_i2c.h"
/**
 * \brief Structure defines PSOC6 gpio pin configuration.
 */
typedef struct pal_psoc_i2c
{
    cyhal_i2c_t *     i2c_master_channel;
    cyhal_gpio_t      sda;
    cyhal_gpio_t      scl;
}   pal_psoc_i2c_t;


#ifdef __cplusplus
}
#endif

#endif /* PAL_PSOC_I2C_MAPPING */

/**
* @}
*/

