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
* \brief   This file implements the platform abstraction layer APIs for GPIO.
*
* \ingroup  grPAL
*
* @{
*/

/* OPTIGA includes */
#include "pal_zephyr.h"
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_ifx_i2c_config.h"

/* Zephyr OS includes */
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>

LOG_MODULE_DECLARE(TRUST_M, CONFIG_LOG_DEFAULT_LEVEL);

//lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context)
{   
    if (p_gpio_context->p_gpio_hw != NULL)
    {
        pal_zephyr_gpio_t *gpio_ctx = (pal_zephyr_gpio_t*)(p_gpio_context->p_gpio_hw);
        
        gpio_ctx->p_device = device_get_binding(gpio_ctx->p_port_name);

        if (gpio_pin_configure(gpio_ctx->p_device, gpio_ctx->pin, GPIO_DIR_OUT) == 0)
        {
            gpio_ctx->init_flag = 1;
        }
        else
        {
            LOG_ERR("could not configure GPIO-Pin %d", gpio_ctx->pin);

            return PAL_STATUS_FAILURE;
        }
    }

    return PAL_STATUS_SUCCESS;
}

//lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context)
{
    /* Not used for Zephyr Port */
    return PAL_STATUS_SUCCESS;
}

void pal_gpio_set_high(const pal_gpio_t *p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        pal_zephyr_gpio_t *gpio_ctx = (pal_zephyr_gpio_t*)(p_gpio_context->p_gpio_hw);
        
        // Make sure not to write to uninitialized gpio pins
        if (gpio_ctx->init_flag == 0)
        {
            pal_gpio_init(p_gpio_context);
        }

        gpio_pin_write(gpio_ctx->p_device, gpio_ctx->pin, HIGH);
    }
}

void pal_gpio_set_low(const pal_gpio_t *p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        pal_zephyr_gpio_t *gpio_ctx = (pal_zephyr_gpio_t*)(p_gpio_context->p_gpio_hw);

        // Make sure not to write to uninitialized gpio pins
        if (gpio_ctx->init_flag == 0)
        {
            pal_gpio_init(p_gpio_context);
        }

        gpio_pin_write(gpio_ctx->p_device, gpio_ctx->pin, LOW);
    }
}

/**
* @}
*/
