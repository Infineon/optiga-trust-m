/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \file pal_os_lock.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os locks (e.g. semaphore).
 *
 * \ingroup  grPAL
 * @{
 */

#include "pal_os_lock.h"

#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t xLockSemaphoreHandle;

volatile uint8_t first_call_flag = 1;

void _lock_init(pal_os_lock_t *p_lock) {
    xLockSemaphoreHandle = xSemaphoreCreateBinary();
    pal_os_lock_release(p_lock);
}

void pal_os_lock_create(pal_os_lock_t *p_lock, uint8_t lock_type) {
    p_lock->type = lock_type;
    p_lock->lock = 0;
}

// lint --e{715} suppress "p_lock is not used here as it is placeholder for future."
// lint --e{818} suppress "Not declared as pointer as nothing needs to be updated in the pointer."
void pal_os_lock_destroy(pal_os_lock_t *p_lock) {}

pal_status_t pal_os_lock_acquire(pal_os_lock_t *p_lock) {
    (void)p_lock;
    vPortEnterCritical();
    if (first_call_flag) {
        _lock_init(p_lock);
        first_call_flag = 0;
    }
    vPortExitCritical();

    if (xSemaphoreTake(xLockSemaphoreHandle, portMAX_DELAY) == pdTRUE)
        return PAL_STATUS_SUCCESS;
    else {
        return PAL_STATUS_FAILURE;
    }
}

void pal_os_lock_release(pal_os_lock_t *p_lock) {
    (void)p_lock;

    xSemaphoreGive(xLockSemaphoreHandle);
}

void pal_os_lock_enter_critical_section() {
    vPortEnterCritical();
}

void pal_os_lock_exit_critical_section() {
    vPortExitCritical();
}

/**
 * @}
 */
