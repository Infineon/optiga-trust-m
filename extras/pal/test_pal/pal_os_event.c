/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_event.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os event/scheduler.
 *
 * \ingroup  grPAL
 * @{
 */

#include "pal_os_event.h"

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "pal_os_timer.h"

/* Event Debugging print functions */
#ifdef TRUSTM_PAL_EVENT_DEBUG

#define TRUSTM_PAL_EVENT_DBG(x, ...) \
    fprintf(stderr, "%s:%d " x "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...) \
    fprintf(stderr, "%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_ERRFN(x, ...) \
    fprintf(stderr, "Error in %s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_MSGFN(x, ...) \
    fprintf(stderr, "Message:%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#else

#define TRUSTM_PAL_EVENT_DBG(x, ...)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...)
#define TRUSTM_PAL_EVENT_ERRFN(x, ...) \
    fprintf(stderr, "Error in %s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_MSGFN(x, ...) \
    fprintf(stderr, "Message:%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif

/* CLOCK ID Definition */
#define CLOCKID CLOCK_REALTIME
/* Signal Definition */
#define SIG SIGRTMIN

/* pal os event handler */
static void pal_os_event_handler(int sig, siginfo_t *si, void *uc) {
    TRUSTM_PAL_EVENT_DBGFN(">");
    pal_os_event_trigger_registered_callback();
    TRUSTM_PAL_EVENT_DBGFN("<");
}

/* local pal os event 0 definition */
static pal_os_event_t pal_os_event_0 = {0};
/* Local timer definition */
static timer_t timerid;

/* Start pal os event */
void pal_os_event_start(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args
) {
    TRUSTM_PAL_EVENT_DBGFN(">");

    /* if event not triggered yet */
    if (FALSE == p_pal_os_event->is_event_triggered) {
        p_pal_os_event->is_event_triggered = TRUE;
        /* Event triggered after 1000us = 1ms */
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
    TRUSTM_PAL_EVENT_DBGFN("<");
}

/* Stop pal os event */
void pal_os_event_stop(pal_os_event_t *p_pal_os_event) {
    TRUSTM_PAL_EVENT_DBGFN(">");
    /* Set event as not triggered */
    p_pal_os_event->is_event_triggered = FALSE;
    TRUSTM_PAL_EVENT_DBGFN("<");
}

/* Create pal os event */
pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args) {
    struct sigevent sev;
    struct sigaction sa;

    TRUSTM_PAL_EVENT_DBGFN(">");

    if ((NULL != callback) && (NULL != callback_args)) {
        /* Establishing handler for signal */
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = pal_os_event_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIG, &sa, NULL) == -1) {
            printf("sigaction\n");
            exit(1);
        }

        /* Create the timer */
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIG;
        sev.sigev_value.sival_ptr = &timerid;
        if (timer_create(CLOCKID, &sev, &timerid) == -1) {
            printf("timer_create\n");
            exit(1);
        }

        /* Start pal os event */
        pal_os_event_start(&pal_os_event_0, callback, callback_args);
    }

    TRUSTM_PAL_EVENT_DBGFN("<");

    return (&pal_os_event_0);
}

/* pal os event triggered callback */
void pal_os_event_trigger_registered_callback(void) {
    register_callback callback;
    struct itimerspec its;

    TRUSTM_PAL_EVENT_DBGFN(">");

    /* Set timer value to 0 */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        fprintf(stderr, "Error in timer_settime\n");
        exit(1);
    }

    /* If a callaback is registered */
    if (pal_os_event_0.callback_registered) {
        callback = pal_os_event_0.callback_registered;
        pal_os_event_0.callback_registered = NULL;
        /* Calling callback function */
        callback((void *)pal_os_event_0.callback_ctx);
    }

    TRUSTM_PAL_EVENT_DBGFN("<");
}
/// @endcond

/* pal os event triggered callback one time */
void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
) {
    struct itimerspec its;
    long long freq_nanosecs;
    int ret = 0;

    TRUSTM_PAL_EVENT_DBGFN(">");

    /* Set callback and arguments in the event */
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    /* Start the timer */
    freq_nanosecs = time_us * 1000;
    its.it_value.tv_sec = (freq_nanosecs / 1000000000);
    its.it_value.tv_nsec = (freq_nanosecs % 1000000000);
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if ((ret = timer_settime(timerid, 0, &its, NULL)) == -1) {
        int errsv = errno;
        if (errsv == EINVAL) {
            fprintf(stderr, "timer_settime INVALID VALUE!\n");
        } else {
            fprintf(stderr, "timer_settime UNKOWN ERROR: %d\n", errsv);
        }
        exit(1);
    }

    TRUSTM_PAL_EVENT_DBGFN("<");
}

/* Destroy pal os event */
void pal_os_event_destroy(pal_os_event_t *pal_os_event) {
    TRUSTM_PAL_EVENT_DBGFN(">");
    /* delete local timer */
    timer_delete(timerid);
    TRUSTM_PAL_EVENT_DBGFN("<");
}

/**
 * @}
 */
