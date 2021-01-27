/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
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

#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden 

static void vTimerCallback(TimerHandle_t xTimer)
{
  /* Optionally do something if the pxTimer parameter is NULL. */
  configASSERT(xTimer);

  pal_os_event_t *p_pal_os_event = (pal_os_event_t *)pvTimerGetTimerID( xTimer );
  p_pal_os_event->callback_registered(p_pal_os_event->callback_ctx);
}


/// @endcond


pal_os_event_t *pal_os_event_create(register_callback callback, void * callback_args)
{
  TimerHandle_t xTimer;

  pal_os_event_t *p_pal_os_event = pvPortMalloc(sizeof(pal_os_event_t));
  if (p_pal_os_event != NULL)
  {
	p_pal_os_event->callback_registered = callback;
	p_pal_os_event->callback_ctx = callback_args;
    p_pal_os_event->is_event_triggered = false;

    xTimer = xTimerCreate(NULL, 1, pdFALSE, p_pal_os_event, vTimerCallback);
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

void pal_os_event_destroy(pal_os_event_t *p_pal_os_event)
{
  vPortFree(p_pal_os_event);
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

void pal_os_event_start(pal_os_event_t * p_pal_os_event, register_callback callback, void *callback_args)
{
  if ((p_pal_os_event != NULL) && (p_pal_os_event->is_event_triggered == false))
  {
	p_pal_os_event->is_event_triggered = true;
	pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
  }
}

void pal_os_event_stop(pal_os_event_t * p_pal_os_event)
{
  if (p_pal_os_event != NULL)
  {
	p_pal_os_event->is_event_triggered = false;
  }
}

/**
* @}
*/

