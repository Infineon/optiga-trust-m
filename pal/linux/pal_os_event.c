/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
* @{
*/

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "optiga/pal/pal_os_timer.h"
#include "optiga/pal/pal_os_event.h"

//#define TRUSTM_PAL_EVENT_DEBUG = 1

#ifdef TRUSTM_PAL_EVENT_DEBUG

#define TRUSTM_PAL_EVENT_DBG(x, ...)      fprintf(stderr, "%s:%d " x "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...)    fprintf(stderr, "%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_ERRFN(x, ...)    fprintf(stderr, "Error in %s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_MSGFN(x, ...)    fprintf(stderr, "Message:%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#else

#define TRUSTM_PAL_EVENT_DBG(x, ...)
#define TRUSTM_PAL_EVENT_DBGFN(x, ...)
#define TRUSTM_PAL_EVENT_ERRFN(x, ...)    fprintf(stderr, "Error in %s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRUSTM_PAL_EVENT_MSGFN(x, ...)    fprintf(stderr, "Message:%s:%d %s: " x "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN


static void handler(int sig, siginfo_t *si, void *uc)
{
	TRUSTM_PAL_EVENT_DBGFN(">");    
	pal_os_event_trigger_registered_callback();
	TRUSTM_PAL_EVENT_DBGFN("<");    
}


/// @cond hidden

static pal_os_event_t pal_os_event_0 = {0};
static 	timer_t timerid;

void pal_os_event_start(pal_os_event_t * p_pal_os_event, register_callback callback, void * callback_args)
{
    TRUSTM_PAL_EVENT_DBGFN(">");    
    
    if(FALSE == p_pal_os_event->is_event_triggered)
    {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event,callback,callback_args,1000);
    }
    TRUSTM_PAL_EVENT_DBGFN("<");    

}

void pal_os_event_stop(pal_os_event_t * p_pal_os_event)
{
    TRUSTM_PAL_EVENT_DBGFN(">");    
    
    //lint --e{714} suppress "The API pal_os_event_stop is not exposed in header file but used as extern in optiga_cmd.c"
    p_pal_os_event->is_event_triggered = FALSE;

    TRUSTM_PAL_EVENT_DBGFN("<");    

}

void pal_os_event_disarm(void)
{
	struct itimerspec its;

	TRUSTM_PAL_EVENT_DBGFN(">");    
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;
	
	if (timer_settime(timerid, 0, &its, NULL) == -1)
	{
		printf("Error in timer_settime\n");
	    exit(1);
	}

	TRUSTM_PAL_EVENT_DBGFN("<");    
} 

void pal_os_event_arm(void)
{   
	struct itimerspec its;

	TRUSTM_PAL_EVENT_DBGFN(">");    
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 1000000;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 294967296;
	
	if (timer_settime(timerid, 0, &its, NULL) == -1)
	{
		printf("Error in timer_settime\n");
	    exit(1);
	}

	TRUSTM_PAL_EVENT_DBGFN("<");    
} 

void pal_os_event_destroy1(void)
{
    TRUSTM_PAL_EVENT_DBGFN(">");    
    timer_delete(timerid);
    TRUSTM_PAL_EVENT_DBGFN("<");    
}

pal_os_event_t * pal_os_event_create(register_callback callback, void * callback_args)
{
    struct sigevent sev;
    struct sigaction sa;

    TRUSTM_PAL_EVENT_DBGFN(">");    
	
    if(( NULL != callback )&&( NULL != callback_args ))
    {
		/* Establishing handler for signal */
		
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIG, &sa, NULL) == -1)
        {
            printf("sigaction\n");
            exit(1);
        }

        /* Create the timer */

        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIG;
        sev.sigev_value.sival_ptr = &timerid;
        if (timer_create(CLOCKID, &sev, &timerid) == -1)
        {
            printf("timer_create\n");
            exit(1);
        }

        pal_os_event_start(&pal_os_event_0,callback,callback_args);
    }

    TRUSTM_PAL_EVENT_DBGFN("<");    
    
    return (&pal_os_event_0);
}

void pal_os_event_trigger_registered_callback(void)
{
    register_callback callback;
    struct itimerspec its;

    TRUSTM_PAL_EVENT_DBGFN(">");  
    
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    
    if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
        fprintf(stderr, "Error in timer_settime\n");
        exit(1);
    }

    if (pal_os_event_0.callback_registered)
    {
        callback = pal_os_event_0.callback_registered;
        pal_os_event_0.callback_registered = NULL;
        callback((void * )pal_os_event_0.callback_ctx);
    }

    TRUSTM_PAL_EVENT_DBGFN("<"); 
}
/// @endcond

void pal_os_event_register_callback_oneshot(pal_os_event_t * p_pal_os_event,
                                             register_callback callback,
                                             void * callback_args,
                                             uint32_t time_us)
{
    struct itimerspec its;
    long long freq_nanosecs;
    int ret = 0;
    //sigset_t mask;

    TRUSTM_PAL_EVENT_DBGFN(">");
    
    //uint8_t scheduler_timer;
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;
    
    /* Start the timer */
    freq_nanosecs = time_us * 1000;
    its.it_value.tv_sec = (freq_nanosecs / 1000000000);
    its.it_value.tv_nsec = (freq_nanosecs % 1000000000);
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    
    if (( ret = timer_settime(timerid, 0, &its, NULL)) == -1)    
    {
        int errsv = errno;
        fprintf(stderr,"timer_settime FAILED!!!\n");
        if(errsv == EINVAL)
        {
            fprintf(stderr,"INVALID VALUE!\n");
        }
        else
        {
            fprintf(stderr,"UNKOWN ERROR: %d\n",errsv);
        }
        exit(1);
    }
    
    TRUSTM_PAL_EVENT_DBGFN("<"); 
}
//lint --e{818,715} suppress "As there is no implementation, pal_os_event is not used"
void pal_os_event_destroy(pal_os_event_t * pal_os_event)
{
    TRUSTM_PAL_EVENT_DBGFN(">");    
    timer_delete(timerid);
    TRUSTM_PAL_EVENT_DBGFN("<");    

}

/**
* @}
*/

