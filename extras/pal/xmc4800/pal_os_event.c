/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_event.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os event/scheduler.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_os_event.h"

#include <DAVE.h>

/// @cond hidden

static pal_os_event_t pal_os_event_0 = {0};

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

void pal_os_event_trigger_registered_callback(void) {
    register_callback callback;

    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // The following steps related to TIMER must be taken care while porting to different platform
    TIMER_ClearEvent(&scheduler_timer);
    // lint --e{534} suppress "Error handling is not required so return value is not checked"
    TIMER_Stop(&scheduler_timer);
    TIMER_Clear(&scheduler_timer);
    /// If callback_ctx is NULL then callback function will have unexpected behavior
    if (pal_os_event_0.callback_registered) {
        callback = pal_os_event_0.callback_registered;
        callback((void *)pal_os_event_0.callback_ctx);
    }
}
/// @endcond

void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
) {
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // The following steps related to TIMER must be taken care while porting to different platform
    // lint --e{534} suppress "Error handling is not required so return value is not checked"
    TIMER_SetTimeInterval(&scheduler_timer, (time_us * 100));
    TIMER_Start(&scheduler_timer);
}

// lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t *pal_os_event) {
    pal_os_event->callback_ctx = NULL;
    pal_os_event->callback_registered = NULL;

    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // The following steps related to TIMER must be taken care while porting to different platform
    TIMER_ClearEvent(&scheduler_timer);
    // lint --e{534} suppress "Error handling is not required so return value is not checked"
    TIMER_Stop(&scheduler_timer);
    TIMER_Clear(&scheduler_timer);
}

/**
 * @}
 */
