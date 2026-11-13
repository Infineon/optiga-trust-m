/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
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
#include <stdlib.h>
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
* Reads the monotonic clock. The stack measures every timeout as a difference
* of these timestamps, so they must not follow wall-clock steps (settimeofday,
* NTP, a late RTC load), which would fail or stall an operation in flight.
*/
static struct timespec pal_os_monotonic_now(void) {
    struct timespec now;

    if (0 != clock_gettime(CLOCK_MONOTONIC, &now)) {
        exit(-1);
    }
    return now;
}

/**
 * Gets the time in microseconds; wraps modulo 2^32, the stack only subtracts.
 */
uint32_t pal_os_timer_get_time_in_microseconds(void) {
    struct timespec now = pal_os_monotonic_now();
    return (uint32_t)((uint64_t)now.tv_sec * 1000000u + (uint64_t)now.tv_nsec / 1000u);
}

/**
 * Gets the time in milliseconds; wraps modulo 2^32, the stack only subtracts.
 *
 * \retval  uint32_t time in milliseconds
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    struct timespec now = pal_os_monotonic_now();
    return (uint32_t)((uint64_t)now.tv_sec * 1000u + (uint64_t)now.tv_nsec / 1000000u);
}

/**
 * Funtion to wait or delay until the supplied milli seconds time
 *
 *\param[in] milliseconds Delay value in milli seconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    struct timespec deadline;

    clock_gettime(CLOCK_MONOTONIC, &deadline);

    deadline.tv_sec += milliseconds / 1000;
    deadline.tv_nsec += (long)(milliseconds % 1000) * 1000 * 1000;
    /* An unnormalised tv_nsec makes clock_nanosleep fail with EINVAL, i.e. no delay. */
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec += 1;
        deadline.tv_nsec -= 1000000000L;
    }

    /* clock_nanosleep returns the error number; it does not set errno. */
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL) == EINTR) {
    }
}

// lint --e{714} suppress "This function is used for to support multiple platform "
pal_status_t pal_timer_init(void) {
    return PAL_STATUS_SUCCESS;
}

// lint --e{714} suppress "This function is used for to support multiple platform "
pal_status_t pal_timer_deinit(void) {
    return PAL_STATUS_SUCCESS;
}
/**
 * @}
 */
