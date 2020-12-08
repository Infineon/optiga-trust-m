/**
* \copyright
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
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
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#ifdef __WIN32__
#include "windows.h"
#include <sys\timeb.h>
#else
#include <time.h>
#include <math.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdint.h>

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
 
 
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

/// @cond hidden 
uint32_t pal_os_timer_get_time_in_microseconds(void)
{
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

/**
 * Function to wait or delay until the supplied milli seconds time
 *
 * \retval  uint32_t time in milliseconds 
 */
#ifdef __WIN32__
uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    struct timeb time;

    ftime(&time);

    return ((uint32_t) (1000 * time.time + time.millitm));
}
#else
uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    long ms; // Milliseconds
	long s; // Seconds
    struct timespec spec;
 
    clock_gettime(CLOCK_REALTIME, &spec);
 
    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
	if (ms > 999) {
        s++;
        ms = 0;
    }
 
    return (1000*s + ms);
}
#endif

/**
* Funtion to wait or delay until the supplied milli seconds time
* 
*\param[in] milliseconds Delay value in milli seconds
*
*/
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
#ifdef __WIN32__
    Sleep(milliseconds);
#else // LINUX
    usleep(milliseconds * 1000);
#endif
}
