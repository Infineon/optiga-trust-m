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
* \file    pal_logger.h
*
* \brief   This file provides the prototypes declarations for pal logger.
*
* \ingroup grPAL
*
* @{
*/


#ifndef _PAL_LOGGER_H_
#define _PAL_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/pal/pal.h"

/**
 * \brief Writes to logger port.
 *
 * \details
 * Writes to logger port.
 * - invokes the platform dependent function to log the information provided.<br>
 *
 * \pre
 * - The pal_logger is initialized if required.
 *
 * \note
 * - None
 *
 * \param[in] p_log_data          Pointer to the log data (data to be logged)
 * \param[in] log_data_length     Length of data to be logged.
 *
 * \retval    PAL_STATUS_SUCCESS  In case of successfully written to logger
 * \retval    PAL_STATUS_FAILURE  In case of failure while writing to logger
 *
 */
pal_status_t pal_logger_write(const uint8_t * p_p_log_data, uint32_t log_data_length);

#ifdef __cplusplus
}
#endif

#endif /*_PAL_LOGGER_H_ */

/**
* @}
*/
