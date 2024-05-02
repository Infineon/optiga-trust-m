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
 *
 * @{
 */

#include "pal_os_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stdio.h"

uint32_t pal_os_timer_get_time_in_microseconds(void) {
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // This API is needed to support optiga cmd scheduler.
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    return xTaskGetTickCount();
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    const TickType_t xDelay = milliseconds / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
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
