/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
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
* \brief   This file implements the platform abstraction layer APIs for os event/scheduler.
*
* \ingroup  grPAL
*
* @{
*/

/* OPTIGA includes */
#include "pal_zephyr.h"
#include "optiga/pal/pal.h"
#include "optiga/pal/pal_os_event.h"

/* Zephyr OS includes */
#include <kernel.h>
#include <logging/log.h>

LOG_MODULE_DECLARE(TRUST_M, CONFIG_LOG_DEFAULT_LEVEL);

// Use this instead of normal pal_os_event_trigger_registered_callback to avoid mismatch of parameters
static void pal_os_event_trigger_registered_callback_wq(struct k_work *item);

struct pal_info {
    struct k_delayed_work work;
    char name[16];
} pal_device;

/// @cond hidden

static pal_os_event_t pal_os_event_0 = {0};

void pal_os_event_start(pal_os_event_t * p_pal_os_event, register_callback callback, void * callback_args)
{
    if (FALSE == p_pal_os_event->is_event_triggered)
    {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback,callback_args, 1000);
    }
}

void pal_os_event_stop(pal_os_event_t * p_pal_os_event)
{
    //lint --e{714} suppress "The API pal_os_event_stop is not exposed in header file but used as extern in 
    //optiga_cmd.c"
    p_pal_os_event->is_event_triggered = FALSE;
}

pal_os_event_t * pal_os_event_create(register_callback callback, void * callback_args)
{
    if (( NULL != callback )&&( NULL != callback_args ))
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
        callback((void *)pal_os_event_0.callback_ctx);
    } 
}
/// @endcond

//
void pal_os_event_trigger_registered_callback_wq(struct k_work *item)
{
    register_callback callback;

    if (pal_os_event_0.callback_registered)
    {
        callback = pal_os_event_0.callback_registered;
        callback((void *)pal_os_event_0.callback_ctx);
    } 
}

void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                             register_callback callback,
                                             void * callback_args,
                                             uint32_t time_us)
{
    /* TODO: put it in a different location
    * 
    *
    */
    static int flag = 0;
    if (flag == 0)
    {
        strcpy(pal_device.name, "PAL_dev");
        k_delayed_work_init(&pal_device.work, pal_os_event_trigger_registered_callback_wq);
        flag++;
    }
    
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    // Zephyr timeout API uses a resolution of 1 ms.
    if (time_us < 1000)
    {
        time_us = 1000;
    }

    // Submit work item with specified delay to work queue
    k_delayed_work_submit(&pal_device.work, K_MSEC(time_us/1000));
}

//lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t * pal_os_event)
{
    // User should take care to destroy the event if it's not required
}

/**
* @}
*/
