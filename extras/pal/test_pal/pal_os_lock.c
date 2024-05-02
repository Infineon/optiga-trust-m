/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_lock.c
 *
 * \brief   This file implements the platform abstraction layer APIs for os locks (e.g. semaphore).
 *
 * \ingroup  grPAL
 * @{
 */

#include "pal_os_lock.h"

/* Pal OS create Lock */
void pal_os_lock_create(pal_os_lock_t *p_lock, uint8_t lock_type) {
    p_lock->type = lock_type;
    p_lock->lock = 0;
}

/* Pal OS Destroy Lock */
void pal_os_lock_destroy(pal_os_lock_t *p_lock) {
    p_lock->type = 0;
    p_lock->lock = 0;
}

/* Pal OS acquire Lock */
pal_status_t pal_os_lock_acquire(pal_os_lock_t *p_lock) {
    pal_status_t return_status = PAL_STATUS_FAILURE;

    /* If not already locked */
    if (!(p_lock->lock)) {
        p_lock->lock++;
        return_status = PAL_STATUS_SUCCESS;
    }
    return return_status;
}

/* Pal OS release Lock */
void pal_os_lock_release(pal_os_lock_t *p_lock) {
    /* If already locked */
    if (0 != p_lock->lock) {
        p_lock->lock--;
    }
}

void pal_os_lock_enter_critical_section() {
    /* Not used */
}

void pal_os_lock_exit_critical_section() {
    /* Not used */
}

/**
 * @}
 */
