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
* \file pal_ifx_i2c_config.h
*
* \brief   This file provides the platform abstraction layer extern declarations for IFX I2C.
*
* \ingroup  grPAL
*
* @{
*/

#ifndef _PAL_IFX_I2C_CONFIG_H_
#define _PAL_IFX_I2C_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"
#include "pal_i2c.h"
#include "pal_gpio.h"
#include "pal_os_datastore.h"

extern pal_i2c_t optiga_pal_i2c_context_0;
extern pal_gpio_t optiga_vdd_0;
extern pal_gpio_t optiga_reset_0;

#ifdef __cplusplus
}
#endif

#endif /* _PAL_IFX_I2C_CONFIG_H_ */

/**
* @}
*/
