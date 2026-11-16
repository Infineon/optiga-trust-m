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

/*
 * The library drives every command as a chain of one-shot timer callbacks,
 * and each step arms its successor before it returns. On an MCU the next step
 * cannot start until the current one has returned, because both run in the
 * same timer interrupt. Delivering each expiry on a new thread (SIGEV_THREAD)
 * breaks that: under CPU load, step N+1 can run while step N is still
 * unwinding and still holds the I2C bus flag, both write the single callback
 * slot, one step is lost, and the operation never completes.
 *
 * Here one event thread waits on a CLOCK_MONOTONIC timerfd and runs each
 * callback to completion inside the PAL critical section (pal_os_lock.c),
 * which restores the single-interrupt model.
 */

#define _GNU_SOURCE /* pthread_setname_np */

#include "pal_os_event.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "pal_os_lock.h"

static pal_os_event_t pal_os_event_0 = {0};
/* Set inside the critical section once the timerfd and the thread both exist;
 * never changes after that, so the event thread reads it without the lock. */
static int g_timer_fd = -1;

static void *event_thread(void *arg) {
    sigset_t all;
    (void)arg;

    sigfillset(&all);
    pthread_sigmask(SIG_BLOCK, &all, NULL);

    for (;;) {
        uint64_t expirations;
        if (read(g_timer_fd, &expirations, sizeof(expirations)) < 0) {
            if (errno != EINTR) {
                fprintf(stderr, "pal_os_event: read(timerfd) failed: %s\n", strerror(errno));
                return NULL;
            }
            continue;
        }
        pal_os_event_trigger_registered_callback();
    }
}

/* Called inside the critical section. On failure nothing is kept, so the next
 * create retries. */
static void event_thread_start(void) {
    pthread_attr_t attr;
    pthread_t thread;
    int rc;
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);

    if (fd < 0) {
        fprintf(stderr, "pal_os_event: timerfd_create failed: %s\n", strerror(errno));
        return;
    }
    /* Published before the thread starts, so the thread never reads -1. */
    g_timer_fd = fd;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    rc = pthread_create(&thread, &attr, event_thread, NULL);
    pthread_attr_destroy(&attr);
    if (rc != 0) {
        fprintf(stderr, "pal_os_event: pthread_create failed: %s\n", strerror(rc));
        g_timer_fd = -1;
        close(fd);
        return;
    }
    pthread_setname_np(thread, "optiga-event");
}

pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args) {
    pal_os_event_t *event = NULL;

    /* The thread and timerfd live for the rest of the process: destroy only
     * disarms, so a later create (a new util/crypt instance) reuses them. */
    pal_os_lock_enter_critical_section();
    if (g_timer_fd < 0) {
        event_thread_start();
    }
    if (g_timer_fd >= 0) {
        event = &pal_os_event_0;
        if ((NULL != callback) && (NULL != callback_args)) {
            pal_os_event_start(event, callback, callback_args);
        }
    }
    pal_os_lock_exit_critical_section();
    return event;
}

void pal_os_event_destroy(pal_os_event_t *pal_os_event) {
    struct itimerspec disarm;
    (void)pal_os_event;

    memset(&disarm, 0, sizeof(disarm));
    pal_os_lock_enter_critical_section();
    if (g_timer_fd >= 0) {
        timerfd_settime(g_timer_fd, 0, &disarm, NULL);
    }
    pal_os_event_0.callback_registered = NULL;
    pal_os_event_0.callback_ctx = NULL;
    pal_os_event_0.is_event_triggered = FALSE;
    pal_os_lock_exit_critical_section();
}

void pal_os_event_start(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args
) {
    pal_os_lock_enter_critical_section();
    if (FALSE == p_pal_os_event->is_event_triggered) {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
    pal_os_lock_exit_critical_section();
}

void pal_os_event_stop(pal_os_event_t *p_pal_os_event) {
    pal_os_lock_enter_critical_section();
    p_pal_os_event->is_event_triggered = FALSE;
    pal_os_lock_exit_critical_section();
}

void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
) {
    struct itimerspec its;

    if (NULL == p_pal_os_event) {
        return;
    }
    /* A zero it_value would disarm the timerfd instead of firing at once. */
    if (0 == time_us) {
        time_us = 1;
    }
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = time_us / 1000000u;
    its.it_value.tv_nsec = (long)(time_us % 1000000u) * 1000L;

    pal_os_lock_enter_critical_section();
    if (g_timer_fd < 0) {
        pal_os_lock_exit_critical_section();
        return;
    }
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;
    if (timerfd_settime(g_timer_fd, 0, &its, NULL) != 0) {
        fprintf(stderr, "pal_os_event: timerfd_settime failed: %s\n", strerror(errno));
    }
    pal_os_lock_exit_critical_section();
}

void pal_os_event_trigger_registered_callback(void) {
    struct itimerspec pending;

    pal_os_lock_enter_critical_section();
    /* If the timer is armed again, a registration landed between the expiry
     * and this lock; that callback is not due yet and the thread wakes for it. */
    if ((timerfd_gettime(g_timer_fd, &pending) == 0) && (0 == pending.it_value.tv_sec)
        && (0 == pending.it_value.tv_nsec)) {
        register_callback callback = pal_os_event_0.callback_registered;
        void *ctx = pal_os_event_0.callback_ctx;
        /* Cleared first: the callback normally registers its successor. */
        pal_os_event_0.callback_registered = NULL;
        pal_os_event_0.callback_ctx = NULL;
        if (NULL != callback) {
            callback(ctx);
        }
    }
    pal_os_lock_exit_critical_section();
}
