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

#include "optiga/pal/pal_os_timer.h"


static volatile uint32_t g_tick_count = 0;

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
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // You need to return here a unique value corresponding to the real-time
    return (g_tick_count);
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
    uint32_t start_time;
    uint32_t current_time;
    uint32_t time_stamp_diff;

    start_time = pal_os_timer_get_time_in_milliseconds();
    current_time = start_time;
    time_stamp_diff = current_time - start_time;
    while (time_stamp_diff <= (uint32_t)milliseconds)
    {
        current_time = pal_os_timer_get_time_in_milliseconds();
        time_stamp_diff = current_time - start_time;
        if (start_time > current_time)
        {
            time_stamp_diff = (0xFFFFFFFF + (current_time - start_time)) + 0x01;
        }        
    }
}

pal_status_t pal_timer_init(void)
{
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_timer_deinit(void)
{
    return PAL_STATUS_SUCCESS;
}
/**
* @}
*/
