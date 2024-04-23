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
* \file pal_os_timer.c
*
* \brief   This file implements the platform abstraction layer APIs for timer.
*
* \ingroup  grPAL
*
* @{
*/

#include "pal_os_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stdio.h"

uint32_t pal_os_timer_get_time_in_microseconds(void)
{
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // This API is needed to support optiga cmd scheduler. 
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    return xTaskGetTickCount();
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
    const TickType_t xDelay = milliseconds / portTICK_PERIOD_MS;
	vTaskDelay( xDelay );
}

//lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_init(void)
{
    return PAL_STATUS_SUCCESS;
}

//lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_deinit(void)
{
    return PAL_STATUS_SUCCESS;
}

/**
* @}
*/

