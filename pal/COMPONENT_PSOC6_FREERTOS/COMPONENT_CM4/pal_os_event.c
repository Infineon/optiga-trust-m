/**
* \copyright
* MIT License
*
* Copyright (c) 2022 Infineon Technologies AG
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

#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"

#include "FreeRTOS.h"
#include "timers.h"


/// @cond hidden

#define PAL_OS_EVENT_INTR_PRIO    (4U)
#define CYHAL_TIMER_SCALING 10

/* Timer object used */
cyhal_timer_t pal_os_event_timer_obj;
/* An internal timer initialisation function */
pal_os_event_t * pal_os_event_init(register_callback callback, void * callback_args);

void pal_os_event_trigger_registered_callback(void) { }

static void pal_os_event_timer_callback(TimerHandle_t xTimer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(xTimer);

    pal_os_event_t *p_pal_os_event = (pal_os_event_t *)pvTimerGetTimerID( xTimer );
    p_pal_os_event->callback_registered(p_pal_os_event->callback_ctx);
}

void pal_os_event_start(pal_os_event_t * p_pal_os_event, register_callback callback, void * callback_args)
{
    if (FALSE == p_pal_os_event->is_event_triggered)
    {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event,callback,callback_args, 1000);
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
    pal_os_event_t * p_pal_os_event;
    if (( NULL != callback )&&( NULL != callback_args ))
    {
        p_pal_os_event = pal_os_event_init(callback, callback_args);
        pal_os_event_start(p_pal_os_event, callback, callback_args);
    }
    return (p_pal_os_event);
}

//lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t * p_pal_os_event)
{
    if (p_pal_os_event != NULL)
    {
        pal_os_event_stop(p_pal_os_event);
        xTimerDelete(p_pal_os_event->os_timer, 1000);
        vPortFree(p_pal_os_event);
    }
}

void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                             register_callback callback,
                                             void * callback_args,
                                             uint32_t time_us)
{
    if (p_pal_os_event != NULL)
    {
        p_pal_os_event->callback_registered = callback;
        p_pal_os_event->callback_ctx = callback_args;

        if (time_us < 1000)
        {
        time_us = 1000;
        }

        xTimerChangePeriod((TimerHandle_t)p_pal_os_event->os_timer, pdMS_TO_TICKS(time_us / 1000), 0);
    }
}

pal_os_event_t * pal_os_event_init(register_callback callback, void * callback_args)
{
    TimerHandle_t xTimer;

    pal_os_event_t *p_pal_os_event = pvPortMalloc(sizeof(pal_os_event_t));
    if (p_pal_os_event != NULL)
    {
        p_pal_os_event->callback_registered = callback;
        p_pal_os_event->callback_ctx = callback_args;
        p_pal_os_event->is_event_triggered = false;

        xTimer = xTimerCreate(NULL, 1, pdFALSE, p_pal_os_event, pal_os_event_timer_callback);
        if (xTimer != NULL)
        {
            p_pal_os_event->os_timer = xTimer;
        }
        else
        {
            return NULL;
        }

        if ((callback != NULL) && (callback_args != NULL))
        {
            pal_os_event_start(p_pal_os_event, callback, callback_args);
        }
    }

    return p_pal_os_event;
}


/**
* @}
*/
