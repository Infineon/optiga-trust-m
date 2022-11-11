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
* \file pal.c
*
* \brief    This file implements the platform abstraction layer APIs.
*
* \ingroup  grPAL
*
* @{
*/


#include "optiga/pal/pal.h"
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_os_timer.h"
#include "pal_psoc_gpio_mapping.h"
#include "optiga_lib_config.h"

extern void pal_os_event_init(void);
#ifdef OPTIGA_TRUSTM_VDD
extern pal_gpio_t optiga_vdd_0;
#endif

#ifdef OPTIGA_TRUSTM_RST
extern pal_gpio_t optiga_reset_0;
#endif


pal_status_t pal_init(void)
{
    #ifdef OPTIGA_TRUSTM_VDD
    pal_gpio_init(&optiga_vdd_0);
    #endif

    #ifdef OPTIGA_TRUSTM_RST
    pal_gpio_init(&optiga_reset_0);
    #endif
    return PAL_STATUS_SUCCESS;
}


pal_status_t pal_deinit(void)
{
    return PAL_STATUS_SUCCESS;
}

/**
* @}
*/
