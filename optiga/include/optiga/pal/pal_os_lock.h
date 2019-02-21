/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
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
typedef struct pal_os_lock
{
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
void pal_os_lock_create(pal_os_lock_t * p_lock, uint8_t lock_type);

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
void pal_os_lock_destroy(pal_os_lock_t * p_lock);

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
pal_status_t pal_os_lock_acquire(pal_os_lock_t * p_lock);

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
void pal_os_lock_release(pal_os_lock_t * p_lock);

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
