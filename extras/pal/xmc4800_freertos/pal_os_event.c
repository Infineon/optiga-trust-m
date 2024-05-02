/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \file
 *
 * \brief This file implements the platform abstraction layer APIs for os event/scheduler.
 *
 * \ingroup  grPAL
 * @{
 */

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "pal_os_event.h"

#include "FreeRTOS.h"
#include "task.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden
/// Callback function when timer elapses
static volatile register_callback callback_registered = NULL;

static pal_os_event_t pal_os_event_0 = {0};
static uint32_t pal_os_ts_0 = 0;

void pal_os_event_start(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args
) {
    if (FALSE == p_pal_os_event->is_event_triggered) {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
}

void pal_os_event_stop(pal_os_event_t *p_pal_os_event) {
    // lint --e{714} suppress "The API pal_os_event_stop is not exposed in header file but used as extern in
    // optiga_cmd.c"
    p_pal_os_event->is_event_triggered = FALSE;
}

pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args) {
    if ((NULL != callback) && (NULL != callback_args)) {
        pal_os_event_start(&pal_os_event_0, callback, callback_args);
    }
    return (&pal_os_event_0);
}

/**
 *  Timer callback handler.
 *
 *  This get called from the TIMER elapse event.<br>
 *  Once the timer expires, the registered callback funtion gets called from the timer event handler, if
 *  the call back is not NULL.<br>
 *
 *\param[in] args Callback argument
 *
 */
void pal_os_event_trigger_registered_callback(void) {
    register_callback callback;

    if ((pal_os_ts_0 != 0) && (pal_os_ts_0 < xTaskGetTickCount())
        && pal_os_event_0.callback_registered) {
        pal_os_ts_0 = 0;
        callback = pal_os_event_0.callback_registered;
        callback((void *)pal_os_event_0.callback_ctx);
    }
}
/// @endcond

/**
 * Platform specific event call back registration function to trigger once when timer expires.
 * <br>
 *
 * <b>API Details:</b>
 *         This function registers the callback function supplied by the caller.<br>
 *         It triggers a timer with the supplied time interval in microseconds.<br>
 *         Once the timer expires, the registered callback function gets called.<br>
 *
 * \param[in] callback              Callback function pointer
 * \param[in] callback_args         Callback arguments
 * \param[in] time_us               time in micro seconds to trigger the call back
 *
 */
void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
) {
    pal_os_event_0.callback_registered = callback;
    pal_os_event_0.callback_ctx = callback_args;

    pal_os_ts_0 = xTaskGetTickCount() + pdMS_TO_TICKS(time_us / 1000);
}

/**
 * @}
 */

// lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t *pal_os_event) {}

/**
 * @}
 */
