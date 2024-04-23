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
* \file pal_gpio.c
*
* \brief   This file implements the platform abstraction layer APIs for GPIO.
*
* \ingroup  grPAL
* @{
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "errno.h"

#include "pal_gpio.h"
#include "pal_ifx_i2c_config.h"
#include "pal_linux.h"

/**
 * @brief Initializes the PAL GPIO layer
 *
 * Initializes the PAL GPIO layer.
 * <br>
 *
 *<b>API Details:</b>
 * - Initializes the PAL GPIO layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
pal_status_t pal_gpio_init(const pal_gpio_t * p_gpio_context)
{
    /* Dummy component*/
    return PAL_STATUS_SUCCESS;
}

/**
 * @brief Deinitializes the PAL GPIO layer
 *
 * Deinitializes the PAL GPIO layer.
 * <br>
 *
 *<b>API Details:</b>
 * - Deinitializes the PAL GPIO layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
pal_status_t pal_gpio_deinit(const pal_gpio_t * p_gpio_context)
{
    /* Dummy component*/        
    return PAL_STATUS_SUCCESS;
}

/**
 * @brief Sets GPIO Pin High
 *
 * Sets GPIO Pin High.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets GPIO Pin High<br>
 *
 *<b>User Input:</b><br>
 * - GPIO context
 *
 * \retval  None
 */
void pal_gpio_set_high(const pal_gpio_t * p_gpio_context)
{
    /* Dummy component*/
    return;
}

/**
 * @brief Sets GPIO Pin Low
 *
 * Sets GPIO Pin High.
 * <br>
 *
 *<b>API Details:</b>
 * - Sets GPIO Pin High<br>
 *
 *<b>User Input:</b><br>
 * - GPIO context
 *
 * \retval  None
 */
void pal_gpio_set_low(const pal_gpio_t* p_gpio_context)
{
    /* Dummy component*/
    return;
}

/**
* @}
*/
