/**
 * \copyright
 * MIT License
 *
 * Copyright (c) 2024 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE
 *
 * \endcopyright
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_event.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os
 * event/scheduler.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_os_event.h"
#include <zephyr/kernel.h>

#define WORK_QUEUE_STACK_SIZE (4096)
#define WORK_PRIORITY (0)

K_THREAD_STACK_DEFINE(work_queue_stack_area, WORK_QUEUE_STACK_SIZE);

struct k_work_q optiga_event_work_q;

static pal_os_event_t pal_os_event_0 = {0};
static bool work_queue_initialized = false;

void work_handler(struct k_work *work)
{
    (void) work;
    pal_os_event_trigger_registered_callback();
}

K_WORK_DELAYABLE_DEFINE(work_item, work_handler);

void pal_os_event_start(pal_os_event_t *p_pal_os_event, register_callback callback, void *callback_args)
{
    if (0 == p_pal_os_event->is_event_triggered)
    {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
}

void pal_os_event_stop(pal_os_event_t *p_pal_os_event)
{
    p_pal_os_event->is_event_triggered = 0;
}

pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args)
{
    if ((NULL != callback) && (NULL != callback_args))
    {
        pal_os_event_start(&pal_os_event_0, callback, callback_args);
    }
    return (&pal_os_event_0);
}

void pal_os_event_trigger_registered_callback(void)
{
    register_callback callback;

    if (pal_os_event_0.callback_registered)
    {
        callback = pal_os_event_0.callback_registered;
        callback((void *) pal_os_event_0.callback_ctx);
    }
}

// NOLINTBEGIN(readability-function-cognitive-complexity)
void pal_os_event_register_callback_oneshot(pal_os_event_t *p_pal_os_event, register_callback callback,
                                            void *callback_args, uint32_t time_us)
{
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    if (!work_queue_initialized)
    {
        k_work_queue_init(&optiga_event_work_q);
        k_work_queue_start(&optiga_event_work_q, work_queue_stack_area, K_THREAD_STACK_SIZEOF(work_queue_stack_area),
                           WORK_PRIORITY, NULL);
        work_queue_initialized = true;
    }

    k_work_schedule_for_queue(&optiga_event_work_q, &work_item, K_USEC(time_us));
}
// NOLINTEND(readability-function-cognitive-complexity)

void pal_os_event_destroy(pal_os_event_t *pal_os_event)
{
    (void) pal_os_event;
    // No dynamic structures to cleanup
}
