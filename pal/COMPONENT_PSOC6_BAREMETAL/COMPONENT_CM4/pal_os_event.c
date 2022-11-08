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
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"


/// @cond hidden

#define PAL_OS_EVENT_INTR_PRIO    (4U)
#define CYHAL_TIMER_SCALING 10

static pal_os_event_t pal_os_event_0 = {0};
/* Timer object used */
cyhal_timer_t pal_os_event_timer_obj;

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

    if (( NULL != callback )&&( NULL != callback_args ))
    {
        pal_os_event_start(&pal_os_event_0,callback,callback_args);
    }
    return (&pal_os_event_0);
}

void pal_os_event_trigger_registered_callback(void)
{
    register_callback callback;

    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // The following steps related to TIMER must be taken care while porting to different platform
    cyhal_timer_stop(&pal_os_event_timer_obj);
    cyhal_timer_reset(&pal_os_event_timer_obj);
    /// If callback_ctx is NULL then callback function will have unexpected behavior 
    if (pal_os_event_0.callback_registered)
    {
        callback = pal_os_event_0.callback_registered;
        callback((void * )pal_os_event_0.callback_ctx);
    }
}
/// @endcond

void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                             register_callback callback,
                                             void * callback_args,
                                             uint32_t time_us)
{
  cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                       /* Timer compare value, not used */
        .period = time_us * CYHAL_TIMER_SCALING,  /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,          /* Timer counts up */
        .is_compare = false,                      /* Don't use compare mode */
        .is_continuous = false,                   /* Run the timer indefinitely */
        .value = 0                                /* Initial value of counter */
    };
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // The following steps related to TIMER must be taken care while porting to different platform
    //lint --e{534} suppress "Error handling is not required so return value is not checked"
    cyhal_timer_configure(&pal_os_event_timer_obj, &timer_cfg);
    cyhal_timer_start(&pal_os_event_timer_obj);
}

//lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t * pal_os_event)
{
    cyhal_timer_free (&pal_os_event_timer_obj);
}

void pal_os_event_init(void)
{
    cy_rslt_t cy_hal_status;
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = 1000,                     /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = false,             /* Run the timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };

    do
    {
        /* Initialize the timer object. Does not use pin output ('pin' is NC) and
         * does not use a pre-configured clock source ('clk' is NULL). */
          cy_hal_status = cyhal_timer_init(&pal_os_event_timer_obj, NC, NULL);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
          break;
        }
        /* Apply timer configuration such as period, count direction, run mode, etc. */
        cy_hal_status = cyhal_timer_configure(&pal_os_event_timer_obj, &timer_cfg);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
          break;
        }
        /* Set the frequency of timer to 10^7 Hz */
        cy_hal_status = cyhal_timer_set_frequency(&pal_os_event_timer_obj, 10000000);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
          break;
        }
        /* Assign the ISR to execute on timer interrupt */
        cyhal_timer_register_callback(&pal_os_event_timer_obj, 
                                     (cyhal_timer_event_callback_t)pal_os_event_trigger_registered_callback, 
                                      NULL);
        /* Set the event on which timer interrupt occurs and enable it */
        cyhal_timer_enable_event(&pal_os_event_timer_obj, CYHAL_TIMER_IRQ_ALL, PAL_OS_EVENT_INTR_PRIO, true);

    } while (false);
}


/**
* @}
*/
