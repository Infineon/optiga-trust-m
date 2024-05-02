/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
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

#include "pal_os_timer.h"

#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "stdint.h"

#if IFX_I2C_LOG_PAL == 1
#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define ERR(...) fprintf(stderr, __VA_ARGS__)
#define LOG_PREFIX "[IFX-PAL-OS-TIMER] "

LIBRARY_EXPORTS uint32_t pal_os_timer_get_time_in_milliseconds(void);
LIBRARY_EXPORTS void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds);

/// @cond hidden
/**
* Gets tick count value in milliseconds
*
*\param[in] none
*

* \retval  uint32_t time in milliseconds
*/
static uint32_t pal_os_get_clock_tick_count() {
    uint32_t now_ms = 0;
    struct timeval tv;

    /* gettimeofday() returns 0 for success, or -1 for failure */
    if (0 == gettimeofday(&tv, NULL)) {
        now_ms = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
    } else {
        exit(-1);
    }
    return now_ms;
}

uint32_t pal_os_timer_get_time_in_microseconds(void) {
    /* pal_os_get_clock_tick_count return milliseconds, we multiply by 1000 */
    return pal_os_get_clock_tick_count() * 1000;
}

/**
 * Funtion to wait or delay until the supplied milli seconds time
 *
 * \retval  uint32_t time in milliseconds
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    /* pal_os_get_clock_tick_count return milliseconds */
    return pal_os_get_clock_tick_count();
}

/**
 * Funtion to wait or delay until the supplied milli seconds time
 *
 *\param[in] milliseconds Delay value in milli seconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    struct timespec deadline;

    /* Get clock time */
    clock_gettime(CLOCK_MONOTONIC, &deadline);

    /* Create a deadline */
    deadline.tv_sec += milliseconds / 1000;
    deadline.tv_nsec += (milliseconds % 1000) * 1000 * 1000;

    /* Sleep until deadline */
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL) != 0) {
        if (errno != EINTR) {
            /* error handling here */
            break;
        }
    }
}

pal_status_t pal_timer_init(void) {
    /* Nothing to do here */
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_timer_deinit(void) {
    /* Nothing to do here */
    return PAL_STATUS_SUCCESS;
}
/**
 * @}
 */
