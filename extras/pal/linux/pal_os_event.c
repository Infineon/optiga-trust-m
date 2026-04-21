/**
 * SPDX-FileCopyrightText: 2020-2026 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_event.c
 *
 * \brief   Platform abstraction layer APIs for OS event/scheduler.
 *
 * \ingroup  grPAL
 */

#include "pal_os_event.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "pal_os_timer.h"

//~ #define TRUSTM_PAL_EVENT_DEBUG  1

#if TRUSTM_PAL_EVENT_DEBUG == 1
#define TRUSTM_PAL_EVENT_DBG(x, ...) \
    fprintf(stderr, "%s:%d " x "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...) \
    fprintf(stderr, "%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#elif TRUSTM_PAL_EVENT_DEBUG == 2
#define TRUSTM_PAL_EVENT_DBG(x, ...) Log("%s:%d " x "\n", "Log", __LINE__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...) \
    Log("%s:%d %s: " x "\n", "Log", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TRUSTM_PAL_EVENT_DBG(x, ...)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...)
#endif

#ifndef TRUSTM_LINUX_TIMER_WAIT
/*
 * Wait time (in ms) before checking the status of the timer
 */
#define TRUSTM_LINUX_TIMER_WAIT 1
#endif /* TRUSTM_LINUX_TIMER_WAIT */

#define TRUSTM_PAL_EVENT_ERRFN(x, ...) \
    fprintf(stderr, "Error in %s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_MSGFN(x, ...) \
    fprintf(stderr, "Message:%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/* Use monotonic clock to avoid wall-clock adjustments affecting timers */
#define CLOCKID CLOCK_MONOTONIC

typedef enum { TIMER_UNINIT = 0, TIMER_READY, TIMER_DESTROYING } timer_state_t;

typedef struct {
    timer_t timerid; /* POSIX timer id */
    pthread_mutex_t lock; /* protects timer operations */
    pthread_cond_t cond; /* transitions condition */
    int timer_valid; /* is timer valid */
    timer_state_t state;
} pal_os_event_timer_t;

static pal_os_event_t pal_os_event_0 = {0};

static pal_os_event_timer_t g_pal_os_event_timer = {
    .timerid = (timer_t)0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .timer_valid = 0,
    .state = TIMER_UNINIT};

/* Return codes for wait_for_timer_ready_locked */
#define TIMER_RC_READY 0 /* timer is ready */
#define TIMER_RC_DESTROYED -1 /* timer was destroyed */
#define TIMER_RC_TIMEOUT -2 /* unexpected timeout */

static int wait_for_timer_ready_locked(long timeout_ms) {
    struct timespec abstime;
    if (clock_gettime(CLOCK_REALTIME, &abstime) != 0) {
        return TIMER_RC_TIMEOUT;
    }
    const long sec = timeout_ms / 1000;
    const long nsec_add = (timeout_ms % 1000) * 1000000L;
    abstime.tv_sec += sec;
    abstime.tv_nsec += nsec_add;
    if (abstime.tv_nsec >= 1000000000L) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000L;
    }

    while (g_pal_os_event_timer.state != TIMER_READY) {
        /*
         * TIMER_UNINIT means the timer was either:
         * - destroyed (normal at session shutdown), or
         * - never created
         *
         * No thread will signal this condvar, so return immediately.
         */
        if (g_pal_os_event_timer.state == TIMER_UNINIT) {
            return TIMER_RC_DESTROYED;
        }
        int rc = pthread_cond_timedwait(
            &g_pal_os_event_timer.cond,
            &g_pal_os_event_timer.lock,
            &abstime
        );
        if (rc == ETIMEDOUT) {
            return TIMER_RC_TIMEOUT;
        } else if (rc != 0) {
            return TIMER_RC_TIMEOUT;
        }
    }
    return TIMER_RC_READY;
}

static void timer_thread_func(union sigval sv) {
    (void)sv;

    pal_os_event_trigger_registered_callback();
}

void pal_os_event_start(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args
) {
    TRUSTM_PAL_EVENT_DBGFN(">");

    if (FALSE == p_pal_os_event->is_event_triggered) {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
    TRUSTM_PAL_EVENT_DBGFN("<");
}

void pal_os_event_stop(pal_os_event_t *p_pal_os_event) {
    TRUSTM_PAL_EVENT_DBGFN(">");
    p_pal_os_event->is_event_triggered = FALSE;
    TRUSTM_PAL_EVENT_DBGFN("<");
}

void pal_os_event_disarm(void) {
    struct itimerspec its;
    int ret;
    memset(&its, 0, sizeof(its));

    TRUSTM_PAL_EVENT_DBGFN(">");

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    pthread_mutex_lock(&g_pal_os_event_timer.lock);

    if (g_pal_os_event_timer.state != TIMER_READY) {
        ret = wait_for_timer_ready_locked(TRUSTM_LINUX_TIMER_WAIT);
        if (ret == TIMER_RC_TIMEOUT) {
            TRUSTM_PAL_EVENT_ERRFN(
                "pal_os_event_disarm: timer not ready (timeout), state=%d",
                (int)g_pal_os_event_timer.state
            );
        }
        if (ret != TIMER_RC_READY) {
            pthread_mutex_unlock(&g_pal_os_event_timer.lock);
            return;
        }
    }

    if (timer_settime(g_pal_os_event_timer.timerid, 0, &its, NULL) == -1) {
        TRUSTM_PAL_EVENT_ERRFN(
            "pal_os_event_disarm: timer_settime failed: errno=%d (%s)",
            errno,
            strerror(errno)
        );
        pthread_mutex_unlock(&g_pal_os_event_timer.lock);
        exit(1);
    }

    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    TRUSTM_PAL_EVENT_DBGFN("<");
}

void pal_os_event_arm(void) {
    struct itimerspec its;
    int ret;
    memset(&its, 0, sizeof(its));

    TRUSTM_PAL_EVENT_DBGFN(">");

    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 294967296;

    pthread_mutex_lock(&g_pal_os_event_timer.lock);

    if (g_pal_os_event_timer.state != TIMER_READY) {
        ret = wait_for_timer_ready_locked(TRUSTM_LINUX_TIMER_WAIT);
        if (ret == TIMER_RC_TIMEOUT) {
            TRUSTM_PAL_EVENT_ERRFN(
                "pal_os_event_arm: timer not ready (timeout), state=%d",
                (int)g_pal_os_event_timer.state
            );
        }
        if (ret != TIMER_RC_READY) {
            pthread_mutex_unlock(&g_pal_os_event_timer.lock);
            return;
        }
    }

    if (timer_settime(g_pal_os_event_timer.timerid, 0, &its, NULL) == -1) {
        TRUSTM_PAL_EVENT_ERRFN(
            "pal_os_event_arm: timer_settime failed: errno=%d (%s)",
            errno,
            strerror(errno)
        );
        pthread_mutex_unlock(&g_pal_os_event_timer.lock);
        exit(1);
    }

    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    TRUSTM_PAL_EVENT_DBGFN("<");
}

pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args) {
    TRUSTM_PAL_EVENT_DBGFN(">");

    if ((NULL != callback) && (NULL != callback_args)) {
        struct sigevent sev;
        memset(&sev, 0, sizeof(sev));
        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_notify_function = timer_thread_func;
        sev.sigev_notify_attributes = NULL;
        sev.sigev_value.sival_ptr = NULL;

        pthread_mutex_lock(&g_pal_os_event_timer.lock);
        if (g_pal_os_event_timer.state != TIMER_READY) {
            if (timer_create(CLOCKID, &sev, &g_pal_os_event_timer.timerid) == -1) {
                pthread_mutex_unlock(&g_pal_os_event_timer.lock);
                TRUSTM_PAL_EVENT_ERRFN(
                    "timer_create failed: errno=%d (%s)",
                    errno,
                    strerror(errno)
                );
                exit(1);
            }
            g_pal_os_event_timer.state = TIMER_READY;
            g_pal_os_event_timer.timer_valid = 1;
            pthread_cond_broadcast(&g_pal_os_event_timer.cond);
        } else {
            TRUSTM_PAL_EVENT_ERRFN("pal_os_event_create: timer already READY");
        }
        pthread_mutex_unlock(&g_pal_os_event_timer.lock);

        pal_os_event_start(&pal_os_event_0, callback, callback_args);
    }

    TRUSTM_PAL_EVENT_DBGFN("<");

    return (&pal_os_event_0);
}

void pal_os_event_trigger_registered_callback(void) {
    register_callback callback;
    void *ctx;

    /*
     * Snapshot and clear both fields under the lock to prevent a TOCTOU race
     * with concurrent register_callback_oneshot calls.
     *
     * The lock must be released before invoking cb() because callback
     * `optiga_cmd_queue_scheduler()` sets the timer via
     * `pal_os_event_register_callback_oneshot`
     */
    pthread_mutex_lock(&g_pal_os_event_timer.lock);
    callback = pal_os_event_0.callback_registered;
    ctx = pal_os_event_0.callback_ctx;
    pal_os_event_0.callback_registered = NULL;
    pal_os_event_0.callback_ctx = NULL;
    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    if (callback) {
        callback(ctx);
    }
}

void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
) {
    struct itimerspec its;
    int ret;
    long long freq_nanosecs;
    memset(&its, 0, sizeof(its));

    TRUSTM_PAL_EVENT_DBGFN(">");

    freq_nanosecs = (long long)time_us * 1000LL;
    its.it_value.tv_sec = (time_t)(freq_nanosecs / 1000000000LL);
    its.it_value.tv_nsec = (long)(freq_nanosecs % 1000000000LL);
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (its.it_value.tv_nsec < 0 || its.it_value.tv_nsec >= 1000000000L) {
        TRUSTM_PAL_EVENT_ERRFN("oneshot: invalid tv_nsec: %ld", (long)its.it_value.tv_nsec);
        return;
    }

    pthread_mutex_lock(&g_pal_os_event_timer.lock);

    if (g_pal_os_event_timer.state != TIMER_READY) {
        ret = wait_for_timer_ready_locked(TRUSTM_LINUX_TIMER_WAIT);
        if (ret == TIMER_RC_TIMEOUT) {
            TRUSTM_PAL_EVENT_ERRFN(
                "oneshot: timer not ready (timeout), state=%d",
                (int)g_pal_os_event_timer.state
            );
        }
        if (ret != TIMER_RC_READY) {
            pthread_mutex_unlock(&g_pal_os_event_timer.lock);
            return;
        }
    }

    /* Write callback and ctx inside the lock, as in pal_os_event_trigger_registered_callback
     * to eliminate the data race. */
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    if (timer_settime(g_pal_os_event_timer.timerid, 0, &its, NULL) == -1) {
        int errsv = errno;
        pthread_mutex_unlock(&g_pal_os_event_timer.lock);
        TRUSTM_PAL_EVENT_ERRFN(
            "pal_os_event_register_callback_oneshot: timer_settime FAILED: errno=%d (%s)",
            errsv,
            strerror(errsv)
        );
        return;
    }

    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    TRUSTM_PAL_EVENT_DBGFN("<");
}

void pal_os_event_destroy1(void) {
    TRUSTM_PAL_EVENT_DBGFN(">");

    pthread_mutex_lock(&g_pal_os_event_timer.lock);
    if (g_pal_os_event_timer.state == TIMER_READY) {
        struct itimerspec its;
        memset(&its, 0, sizeof(its));

        g_pal_os_event_timer.state = TIMER_DESTROYING;
        g_pal_os_event_timer.timer_valid = 0;
        pthread_cond_broadcast(&g_pal_os_event_timer.cond);

        (void)timer_settime(g_pal_os_event_timer.timerid, 0, &its, NULL);
        if (timer_delete(g_pal_os_event_timer.timerid) != 0) {
            TRUSTM_PAL_EVENT_ERRFN("timer_delete failed: errno=%d (%s)", errno, strerror(errno));
        }

        g_pal_os_event_timer.state = TIMER_UNINIT;
        pthread_cond_broadcast(&g_pal_os_event_timer.cond);
    } else {
        TRUSTM_PAL_EVENT_ERRFN(
            "pal_os_event_destroy1: timer not READY (state=%d)",
            (int)g_pal_os_event_timer.state
        );
    }
    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    TRUSTM_PAL_EVENT_DBGFN("<");
}

void pal_os_event_destroy(pal_os_event_t *pal_os_event) {
    (void)pal_os_event;
    TRUSTM_PAL_EVENT_DBGFN(">");

    pthread_mutex_lock(&g_pal_os_event_timer.lock);
    if (g_pal_os_event_timer.state == TIMER_READY) {
        struct itimerspec its;
        memset(&its, 0, sizeof(its));

        g_pal_os_event_timer.state = TIMER_DESTROYING;
        g_pal_os_event_timer.timer_valid = 0;
        pthread_cond_broadcast(&g_pal_os_event_timer.cond);

        (void)timer_settime(g_pal_os_event_timer.timerid, 0, &its, NULL);
        if (timer_delete(g_pal_os_event_timer.timerid) != 0) {
            TRUSTM_PAL_EVENT_ERRFN("timer_delete failed: errno=%d (%s)", errno, strerror(errno));
        }

        g_pal_os_event_timer.state = TIMER_UNINIT;
        pthread_cond_broadcast(&g_pal_os_event_timer.cond);
    } else {
        TRUSTM_PAL_EVENT_ERRFN(
            "pal_os_event_destroy: timer not READY (state=%d)",
            (int)g_pal_os_event_timer.state
        );
    }
    pthread_mutex_unlock(&g_pal_os_event_timer.lock);

    TRUSTM_PAL_EVENT_DBGFN("<");
}
