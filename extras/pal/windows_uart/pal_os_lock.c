/**
* SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
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

void pal_os_lock_create(pal_os_lock_t *p_lock, uint8_t lock_type) {
    p_lock->type = lock_type;
    p_lock->lock = 0;
}

//lint --e{715} suppress "p_lock is not used here as it is placeholder for future."
//lint --e{818} suppress "Not declared as pointer as nothing needs to be updated in the pointer."
void pal_os_lock_destroy(pal_os_lock_t *p_lock) {}

pal_status_t pal_os_lock_acquire(pal_os_lock_t *p_lock) {
    pal_status_t return_status = PAL_STATUS_FAILURE;

    if (!(p_lock->lock)) {
        p_lock->lock++;
        if (1 != p_lock->lock) {
            p_lock->lock--;
        }
        return_status = PAL_STATUS_SUCCESS;
    }
    return return_status;
}

void pal_os_lock_release(pal_os_lock_t *p_lock) {
    if (0 != p_lock->lock) {
        p_lock->lock--;
    }
}

void pal_os_lock_enter_critical_section() {}

void pal_os_lock_exit_critical_section() {}

/**
* @}
*/