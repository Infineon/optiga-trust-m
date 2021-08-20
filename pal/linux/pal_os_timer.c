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
* @{
*/

#include <errno.h>
#include <sys/time.h> 
#include <stdio.h>
#include "stdint.h"
#include <time.h>
#include <unistd.h>
#include "optiga/pal/pal_os_timer.h"

#if IFX_I2C_LOG_PAL == 1
#include <stdio.h>
#define LOG(...)  printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define ERR(...)  fprintf(stderr, __VA_ARGS__)
#define LOG_PREFIX "[IFX-PAL-OS-TIMER] "

LIBRARY_EXPORTS uint32_t pal_os_timer_get_time_in_milliseconds(void);
LIBRARY_EXPORTS void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds);

/// @cond hidden 
/**
* Gets tick count value in microseconds
*
*\param[in] none
*

* \retval  uint32_t time in microseconds 
*/
static uint32_t pal_os_get_clock_tick_count() 
{
    long   us_since_epoch = 0;
    struct timeval tv;

    // gettimeofday() returns 0 for success, or -1 for failure
    if (0 == gettimeofday(&tv, NULL))
    {
    	us_since_epoch = (tv.tv_sec * 1000) + tv.tv_usec;
    }
    else
    {
    	us_since_epoch = 0;
    	exit(-1);
    }
    //LOG(LOG_PREFIX "pal_os_timer_get_time_in_milliseconds() <\n");
    return (us_since_epoch & 0xffff);
}

uint32_t pal_os_timer_get_time_in_microseconds(void)
{
	return pal_os_get_clock_tick_count();
}

/**
 * Funtion to wait or delay until the supplied milli seconds time
 *
 * \retval  uint32_t time in milliseconds 
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    return (pal_os_get_clock_tick_count() / 1000);
}

/**
* Funtion to wait or delay until the supplied milli seconds time
* 
*\param[in] milliseconds Delay value in milli seconds
*
*/
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
    struct timespec deadline;

    clock_gettime(CLOCK_MONOTONIC, &deadline);

    deadline.tv_sec += milliseconds / 1000;
    deadline.tv_nsec += (milliseconds % 1000) * 1000*1000;

    /* Sleep until deadline */
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL) != 0) {
        if (errno != EINTR) {
            /* error handling here */
            break;
        }
    }
}

//lint --e{714} suppress "This function is used for to support multiple platform "
pal_status_t pal_timer_init(void)
{
    return PAL_STATUS_SUCCESS;
}

//lint --e{714} suppress "This function is used for to support multiple platform "
pal_status_t pal_timer_deinit(void)
{
    return PAL_STATUS_SUCCESS;
}
/**
* @}
*/

