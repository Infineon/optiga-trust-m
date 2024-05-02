/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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
#include <sys\timeb.h>

#include "windows.h"
#else
#include <math.h>
#include <time.h>
#include <unistd.h>
#endif

#include <stdint.h>
#include <stdio.h>

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
uint32_t pal_os_timer_get_time_in_microseconds(void) {
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
uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    struct timeb time;

    ftime(&time);

    return ((uint32_t)(1000 * time.time + time.millitm));
}
#else
uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    long ms;  // Milliseconds
    long s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6);  // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    return (1000 * s + ms);
}
#endif

/**
 * Funtion to wait or delay until the supplied milli seconds time
 *
 *\param[in] milliseconds Delay value in milli seconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
#ifdef __WIN32__
    Sleep(milliseconds);
#else  // LINUX
    usleep(milliseconds * 1000);
#endif
}
