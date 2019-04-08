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
* \file pal.h
*
* \brief   This file provides the prototype declarations of platform abstraction layer
*
* \ingroup  grPAL
*
* @{
*/

#ifndef _PAL_H_
#define _PAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/common/optiga_lib_types.h"

/// PAL API execution is successful
#define PAL_STATUS_SUCCESS          (0x0000)
/// PAL API execution failed
#define PAL_STATUS_FAILURE          (0x0001)
/// PAL I2C is busy
#define PAL_STATUS_I2C_BUSY         (0x0002)
/// PAL API execution with wrong input arguments provided 
#define PAL_STATUS_INVALID_INPUT    (0x0004)


/**
 * @brief PAL return status.
 */
typedef uint16_t pal_status_t;

/**
 * \brief Initializes the PAL layer
 *
 * \details
 * Initializes the PAL layer
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successful
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
LIBRARY_EXPORTS pal_status_t pal_init(void);

/**
 * \brief De-Initializes the PAL layer
 *
 * \details
 * De-Initializes the PAL layer
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL de-init it successful
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL de-init fails.
 */
LIBRARY_EXPORTS pal_status_t pal_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_H_ */

/**
* @}
*/

