/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file pal_os_timer.c
*
* \brief   This file implements the platform abstraction layer APIs for timer.
*
* \ingroup  grPAL
*
* @{
*/

#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/// @cond hidden


#define PAL_OS_TIMER_INTR_PRIO    (6U)

static volatile uint32_t g_tick_count = 0;
/* Timer object used */
static cyhal_timer_t pal_os_timer_obj;

static void pal_os_timer_callback_event(void)
{
    g_tick_count += 1U;
    cyhal_timer_reset (&pal_os_timer_obj);
}

/// @endcond


uint32_t pal_os_timer_get_time_in_microseconds(void)
{
    // !!!OPTIGA_LIB_PORTING_REQUIRED
    // This API is needed to support optiga cmd scheduler. 
    static uint32_t count = 0;
    // The implementation must ensure that every invocation of this API returns a unique value.
    return (count++);
}

uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    return (g_tick_count);
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
    cyhal_system_delay_ms (milliseconds);
}

//lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_init(void)
{
    cy_rslt_t cy_hal_status;
    pal_status_t pal_timer_init_status = PAL_STATUS_FAILURE;

    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = 10000,                    /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run the timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };
    
    do
    {
        /* Initialize the timer object. Does not use pin output ('pin' is NC) and
         * does not use a pre-configured clock source ('clk' is NULL). */
        cy_hal_status = cyhal_timer_init(&pal_os_timer_obj, NC, NULL);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
            break;
        }
          
        /* Apply timer configuration such as period, count direction, run mode, etc. */
        cy_hal_status = cyhal_timer_configure(&pal_os_timer_obj, &timer_cfg);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
            break;
        }
        cy_hal_status = cyhal_timer_set_frequency(&pal_os_timer_obj, 10000000);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
            break;
        }
        /* Assign the ISR to execute on timer interrupt */
        cyhal_timer_register_callback(&pal_os_timer_obj, (cyhal_timer_event_callback_t)pal_os_timer_callback_event, NULL);
        /* Set the event on which timer interrupt occurs and enable it */
        cyhal_timer_enable_event(&pal_os_timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, PAL_OS_TIMER_INTR_PRIO, true);
        /* Start the timer with the configured settings */
        cy_hal_status = cyhal_timer_start(&pal_os_timer_obj);
        if(CY_RSLT_SUCCESS != cy_hal_status)
        {
            break;
        }
        pal_timer_init_status = PAL_STATUS_SUCCESS;
        
    } while (false);
    return (pal_timer_init_status);
}

//lint --e{714} suppress "This is implemented for overall completion of API"
pal_status_t pal_timer_deinit(void)
{
    cyhal_timer_free(&pal_os_timer_obj);
    return (PAL_STATUS_SUCCESS);
}
/**
* @}
*/
