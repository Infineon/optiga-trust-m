/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_lock.h
 *
 * \brief   This file provides the prototype declarations of PAL OS lock functionalities
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_OS_LOCK_H_
#define _PAL_OS_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"
/**
 * @brief PAL OS lock structure .
 */
typedef struct pal_os_lock {
    uint8_t lock;
    uint8_t type;
} pal_os_lock_t;

/**
 * \brief Creates a lock.
 *
 * \details
 * Creates a lock to the instance of #pal_os_lock_t.
 * - Creates the lock to the supplied instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_lock         Valid instance of #pal_os_lock_t.
 * \param[in] lock_type      Type of the lock.
 *
 */
void pal_os_lock_create(pal_os_lock_t *p_lock, uint8_t lock_type);

/**
 * \brief Deinitializes the lock.
 *
 * \details
 * Destroys the lock to the instance of #pal_os_lock_t.
 * - Deinitializes the lock of the supplied instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_lock         Valid instance of #pal_os_lock_t.
 *
 */
void pal_os_lock_destroy(pal_os_lock_t *p_lock);

/**
 * \brief Acquires a lock.
 *
 * \details
 * Acquires the lock associated with the instance of #pal_os_lock_t.
 * - Acquires the lock to the supplied instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_lock         Valid instance of #pal_os_lock_t.
 *
 */
pal_status_t pal_os_lock_acquire(pal_os_lock_t *p_lock);

/**
 * \brief Releases the lock.
 *
 * \details
 * Releases the lock associated with the instance of #pal_os_lock_t.
 * - Releases the lock to the supplied instance.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_lock         Valid instance of #pal_os_lock_t.
 *
 */
void pal_os_lock_release(pal_os_lock_t *p_lock);

/**
 * \brief To enter critical section.
 *
 * \details
 * Enters critical section
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 */
void pal_os_lock_enter_critical_section(void);

/**
 * \brief To exit critical section.
 *
 * \details
 * Exits critical section
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 */
void pal_os_lock_exit_critical_section(void);

#ifdef __cplusplus
}
#endif

#endif /*_PAL_OS_LOCK_H_ */

/**
 * @}
 */
