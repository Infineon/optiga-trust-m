/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_lock.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os
 * locks (e.g. semaphore).
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_os_lock.h"

#include <zephyr/kernel.h>

#include "pal.h"

K_SEM_DEFINE(os_lock, 1, 1);

void pal_os_lock_create(pal_os_lock_t *p_lock, uint8_t lock_type) {
    p_lock->type = lock_type;
    p_lock->lock = 0;
}

void pal_os_lock_destroy(pal_os_lock_t *p_lock) {
    (void)p_lock;
}

pal_status_t pal_os_lock_acquire(pal_os_lock_t *p_lock) {
    (void)p_lock;
    if (k_sem_take(&os_lock, K_FOREVER) != 0) {
        return PAL_STATUS_FAILURE;
    }
    return PAL_STATUS_SUCCESS;
}

void pal_os_lock_release(pal_os_lock_t *p_lock) {
    (void)p_lock;
    k_sem_give(&os_lock);
}

void pal_os_lock_enter_critical_section() {
    // For safety critical systems it is recommended to implement a critical
    // section entry
}

void pal_os_lock_exit_critical_section() {
    // For safety critical systems it is recommended to implement a critical
    // section exit
}
