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
 *
 * @{
 */

#include "pal_os_timer.h"

#include <zephyr/kernel.h>

#include "pal.h"

// NOLINTBEGIN(readability-function-cognitive-complexity)
uint32_t pal_os_timer_get_time_in_microseconds(void) {
    return (uint32_t)k_cyc_to_us_near32(k_cycle_get_32());
}

uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    return k_uptime_get_32();
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    k_sleep(K_MSEC(milliseconds));
}
// NOLINTEND(readability-function-cognitive-complexity)

pal_status_t pal_timer_init(void) {
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_timer_deinit(void) {
    return PAL_STATUS_SUCCESS;
}
