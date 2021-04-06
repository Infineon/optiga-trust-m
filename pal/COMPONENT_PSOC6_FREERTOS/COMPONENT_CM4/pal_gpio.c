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
* \file pal_gpio.c
*
* \brief   This file implements the platform abstraction layer APIs for GPIO.
*
* \ingroup  grPAL
*
* @{
*/

//#include <DAVE.h>
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "optiga/pal/pal_gpio.h"
#include "pal_psoc_gpio_mapping.h"


#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

//lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_init(const pal_gpio_t * p_gpio_context)
{
    pal_status_t cy_hal_status = PAL_STATUS_SUCCESS;
    pal_psoc_gpio_t* pin_config = (pal_psoc_gpio_t *)p_gpio_context->p_gpio_hw;
    cy_hal_status = cyhal_gpio_init(pin_config->port_num,
                                       CYHAL_GPIO_DIR_OUTPUT, 
                                       CYHAL_GPIO_DRIVE_STRONG, 
                                       pin_config->init_state);
    if (CY_RSLT_SUCCESS != cy_hal_status)
    {
    	cy_hal_status = PAL_STATUS_FAILURE;
    }

    return (cy_hal_status);
}

//lint --e{714,715} suppress "This is implemented for overall completion of API"
pal_status_t pal_gpio_deinit(const pal_gpio_t * p_gpio_context)
{
    pal_psoc_gpio_t* pin_config = (pal_psoc_gpio_t *)p_gpio_context->p_gpio_hw;
    cyhal_gpio_free(pin_config->port_num);
    return (PAL_STATUS_SUCCESS);
}

void pal_gpio_set_high(const pal_gpio_t * p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        pal_psoc_gpio_t* pin_config = (pal_psoc_gpio_t *)p_gpio_context->p_gpio_hw;
        cyhal_gpio_write((cyhal_gpio_t)(pin_config->port_num), true);
    }
}

void pal_gpio_set_low(const pal_gpio_t * p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))
    {
        pal_psoc_gpio_t* pin_config = (pal_psoc_gpio_t *)p_gpio_context->p_gpio_hw;
        cyhal_gpio_write((cyhal_gpio_t)(pin_config->port_num), false);
    }
}

/**
* @}
*/
