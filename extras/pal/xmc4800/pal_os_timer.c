/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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

#include <DAVE.h>

/// @cond hidden
static volatile uint32_t g_tick_count = 0;

void delay_timer_isr(void) {
    TIMER_ClearEvent(&tick_timer);
    (void)TIMER_Clear(&tick_timer);
    g_tick_count += 1U;
}

/// @endcond

uint32_t pal_os_timer_get_time_in_microseconds(void) {
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // This API is needed to support optiga cmd scheduler.
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    return (g_tick_count);
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    uint32_t start_time;
    uint32_t current_time;
    uint32_t time_stamp_diff;

    start_time = pal_os_timer_get_time_in_milliseconds();
    current_time = start_time;
    time_stamp_diff = current_time - start_time;
    while (time_stamp_diff <= (uint32_t)milliseconds) {
        current_time = pal_os_timer_get_time_in_milliseconds();
        time_stamp_diff = current_time - start_time;
        if (start_time > current_time) {
            time_stamp_diff = (0xFFFFFFFF + (current_time - start_time)) + 0x01;
        }
    }
}

// lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_init(void) {
    return PAL_STATUS_SUCCESS;
}

// lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_deinit(void) {
    return PAL_STATUS_SUCCESS;
}
/**
 * @}
 */
