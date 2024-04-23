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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * \file pal.c
 *
 * \brief    This file implements the platform abstraction layer APIs.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "optiga/pal/pal.h"
#include "optiga/pal/pal_logger.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal_gpio.h"

extern pal_gpio_t optiga_vdd_0;
extern pal_gpio_t optiga_reset_0;

pal_status_t pal_init(void)
{
    pal_status_t status = pal_logger_init(NULL);

    if (status != PAL_STATUS_SUCCESS)
    {
        return status;
    }

    status = pal_timer_init();

    if (status != PAL_STATUS_SUCCESS)
    {
        return status;
    }

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_VDD_SUPPORT
    status = pal_gpio_init(&optiga_vdd_0);

    if (status != PAL_STATUS_SUCCESS)
    {
        return status;
    }
#endif

#ifdef CONFIG_OPTIGA_TRUST_M_GPIO_RST_SUPPORT
    status = pal_gpio_init(&optiga_reset_0);
#endif
    return status;
}

pal_status_t pal_deinit(void)
{
    return PAL_STATUS_SUCCESS;
}
