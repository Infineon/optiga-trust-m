/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_event.h
 *
 * \brief   This file provides the prototype declarations of PAL OS event
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_OS_EVENT_H_
#define _PAL_OS_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_lib_types.h"

/**
 * \brief typedef for Callback function when timer elapses.
 */
typedef void (*register_callback)(void *);

/** \brief PAL os event structure */
typedef struct pal_os_event {
    /// context to be passed to callback
    void *callback_ctx;
    /// os timer
    void *os_timer;
    /// event triggered status
    bool_t is_event_triggered;
    /// Holds the next event timeout value in microseconds
    uint32_t timeout_us;
    /// To synchronize between events
    uint8_t sync_flag;
    /// registered callback
    register_callback callback_registered;
} pal_os_event_t;

/**
 * \brief Create an os event.
 *
 * \details
 * Starts an OS event.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] callback                      Callback function to be registered internally
 * \param[in] callback_args                 Argument to be passed to registered callback
 *
 */
LIBRARY_EXPORTS pal_os_event_t *
pal_os_event_create(register_callback callback, void *callback_args);

/**
 * \brief Destroys an os event.
 *
 * \details
 * Destroys an OS event.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] pal_os_event                  pal_os_event to be destroyed
 *
 */
LIBRARY_EXPORTS void pal_os_event_destroy(pal_os_event_t *pal_os_event);

/**
 * \brief Callback registration function to trigger once when timer expires.
 *
 * \details
 * Platform specific event call back registration function to trigger once when timer expires.
 *  - This function registers the callback function supplied by the caller.
 *  - It triggers a timer with the supplied time interval in microseconds.
 *  - Once the timer expires, the registered callback function gets called.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_pal_os_event        Pointer to pal_os_event
 * \param[in] callback              Callback function pointer
 * \param[in] callback_args         Callback arguments
 * \param[in] time_us               time in micro seconds to trigger the call back
 *
 */
LIBRARY_EXPORTS void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
);

/**
 * \brief Timer callback handler.
 *
 * \details
 * Timer callback handler.
 *  - This gets called from the TIMER elapse event.
 *  - Once the timer expires, the registered callback function gets called from the timer event handler, if the call back is not NULL.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 */
void pal_os_event_trigger_registered_callback(void);

/**
 * \brief Start an os event.
 *
 * \details
 * Starts an OS event.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_pal_os_event                Pointer to os event
 * \param[in] callback                      Callback function to be registered internally
 * \param[in] callback_args                 Arguement to be passed to registered callback
 *
 */
LIBRARY_EXPORTS void
pal_os_event_start(pal_os_event_t *p_pal_os_event, register_callback callback, void *callback_args);

/**
 * \brief Stops an os event.
 *
 * \details
 * Stops an OS event.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_pal_os_event                Pointer to os event
 *
 */
LIBRARY_EXPORTS void pal_os_event_stop(pal_os_event_t *p_pal_os_event);

#ifdef __cplusplus
}
#endif

#endif /*_PAL_OS_EVENT_H_*/

/**
 * @}
 */
