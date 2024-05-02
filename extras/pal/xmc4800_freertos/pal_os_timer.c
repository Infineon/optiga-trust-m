/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \file
 *
 * \brief This file implements the platform abstraction layer APIs for timer.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "pal_os_timer.h"

#include "FreeRTOS.h"
#include "stdio.h"
#include "task.h"
/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/// @cond hidden
/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/// @endcond
/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

uint32_t pal_os_timer_get_time_in_microseconds(void) {
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // This API is needed to support optiga cmd scheduler.
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

/**
 * Get the current time in milliseconds<br>
 *
 *
 * \retval  uint32_t time in milliseconds
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void) {
    return xTaskGetTickCount();
}

/**
 * Waits or delays until the given milliseconds time
 *
 * \param[in] milliseconds Delay value in milliseconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds) {
    const TickType_t xDelay = milliseconds / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
}

/**
 * @}
 */
