/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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

#include "pal_os_event.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "pal.h"
#include "pal_os_timer.h"
#include "stdio.h"

/// @cond hidden
void pal_os_event_delayms(uint32_t time_ms);

static pal_os_event_t pal_os_event_0 = {0};
uint32_t timeout = 0;

void pal_os_event_start(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args
) {
    if (FALSE == p_pal_os_event->is_event_triggered) {
        p_pal_os_event->is_event_triggered = TRUE;
        pal_os_event_register_callback_oneshot(p_pal_os_event, callback, callback_args, 1000);
    }
}

void pal_os_event_stop(pal_os_event_t *p_pal_os_event) {
    // lint --e{714} suppress "The API pal_os_event_stop is not exposed in header file but used as extern in
    // optiga_cmd.c"
    p_pal_os_event->is_event_triggered = FALSE;
}

pal_os_event_t *pal_os_event_create(register_callback callback, void *callback_args) {
    if ((NULL != callback) && (NULL != callback_args)) {
        pal_os_event_start(&pal_os_event_0, callback, callback_args);
    }
    return (&pal_os_event_0);
}

/// @endcond

SemaphoreHandle_t xSemaphore = NULL;
TimerHandle_t xTimer = NULL;

/**
 *  Timer callback handler.
 *
 *  This get called from the TIMER elapse event.<br>
 *  Once the timer expires, the registered callback funtion gets called from the timer event handler, if
 *  the call back is not NULL.<br>
 *
 *\param[in] args Callback argument
 *
 */
void vTimerCallback(TimerHandle_t xTimer) {
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(xTimer);

    /*
     * You can't call callback from the timer callback, this might lead to a corruption
     * Use a semaphore instead
     * */
    xSemaphoreGive(xSemaphore);
}

/// @endcond

void pal_os_event_trigger_registered_callback(void) {
    register_callback func = NULL;
    void *func_args = NULL;

    /*
    See if we can obtain the element from the semaphore.  If the semaphore is not
    available wait block the task to see if it becomes free.
    portMAX_DELAY works only if INCLUDE_vTaskSuspend id define to 1
    */
    // printf("vTaskCallbackHandler\r\n");
    do {
        if (xSemaphoreTake(xSemaphore, (TickType_t)portMAX_DELAY) == pdTRUE) {
            if (pal_os_event_0.callback_registered) {
                func = pal_os_event_0.callback_registered;
                pal_os_event_0.callback_registered = NULL;
                func_args = pal_os_event_0.callback_ctx;
                func((void *)func_args);
            }
        }
    } while (1);
}

void _pal_os_event_trigger_registered_callback(void *pvParameters) {
    pal_os_event_trigger_registered_callback();
}

pal_status_t pal_os_event_init(void) {
    pal_status_t status = PAL_STATUS_FAILURE;
    BaseType_t xReturned;

    do {
        /* Create a semaphore and take it now */
        xSemaphore = xSemaphoreCreateBinary();
        if (xSemaphore == NULL) {
            break;
        }

        xSemaphoreTake(xSemaphore, (TickType_t)10);

        /* Create the handler for the callbacks. */
        xReturned = xTaskCreate(
            pal_os_event_trigger_registered_callback, /* Function that implements the task. */
            "otx_os_tsk", /* Text name for the task. */
            configMINIMAL_STACK_SIZE * 5, /* Stack size in words, not bytes. */
            NULL, /* Parameter passed into the task. */
            5, /* Priority at which the task is created. */
            NULL
        ); /* Used to pass out the created task's handle. */
        if (xReturned != pdPASS) {
            break;
        }

        ESP_LOGI("pal_os_event", "Init : Create Timer");
#if 0        
		xTimer = xTimerCreate("otx_os_tmr",        /* Just a text name, not used by the kernel. */
							  1 / portTICK_PERIOD_MS,    /* The timer period in ticks. */
							  pdFALSE,         /* The timers will auto-reload themselves when they expire. */
							  ( void * ) NULL,   /* Assign each timer a unique id equal to its array index. */
							  vTimerCallback  /* Each timer calls the same callback when it expires. */
							  );
#endif
        xTimer = xTimerCreate("otx_os_tmr2", pdMS_TO_TICKS(10), pdFALSE, (void *)0, vTimerCallback);

        if (xTimer == NULL) {
            break;
        }
        ESP_LOGI("pal_os_event", "Init : Create Timer successful");
        status = PAL_STATUS_SUCCESS;

    } while (0);

    return status;
}

void pal_os_event_register_callback_oneshot(
    pal_os_event_t *p_pal_os_event,
    register_callback callback,
    void *callback_args,
    uint32_t time_us
)

{
    if (time_us < 1000) {
        time_us = 1000;
    } else {
        if (time_us % 1000)
            time_us++;
    }

    // timeout = (time_us/1000);
    // if (time_us%1000) timeout++;
    p_pal_os_event->callback_registered = callback;
    p_pal_os_event->callback_ctx = callback_args;

    // ESP_LOGI("pal_os_event", "oneShot : %d", time_us);
    // xTimerChangePeriod( xTimer, (time_us / 1000) / portTICK_PERIOD_MS, 10);
    xTimerChangePeriod(xTimer, pdMS_TO_TICKS(10), portMAX_DELAY);
}

void pal_os_event_delayms(uint32_t time_ms) {
    const TickType_t xDelay = time_ms / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
}

void pal_os_event_destroy(pal_os_event_t *pal_os_event) {}

/**
 * @}
 */
