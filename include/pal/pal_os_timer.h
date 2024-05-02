/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_timer.h
 *
 * \brief   This file provides the prototype declarations of PAL OS timer functionalities.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_OS_TIMER_H_
#define _PAL_OS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"

/**
 * \brief Gets tick count value in microseconds
 *
 * \details
 * Get the current time in microseconds
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  uint32_t time in microseconds
 */
uint32_t pal_os_timer_get_time_in_microseconds(void);

/**
 * \brief Gets tick count value in milliseconds
 *
 * \details
 * Get the current time in milliseconds
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  uint32_t time in milliseconds
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void);

/**
 * \brief Waits or delay until the supplied milliseconds
 *
 * \details
 * Waits or delays until the given milliseconds time
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] milliseconds Delay value in milliseconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds);

/**
 * \brief To initialize the timer
 *
 * \details
 * To initialize the timer
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS    On successful execution
 * \retval  #PAL_STATUS_FAILURE    On failure
 */
pal_status_t pal_timer_init(void);

/**
 * \brief To De-initialize the timer
 *
 * \details
 * To De-initialize the timer
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS    On successful execution
 * \retval  #PAL_STATUS_FAILURE    On failure
 */
pal_status_t pal_timer_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_OS_TIMER_H_ */

/**
 * @}
 */
