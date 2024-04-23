/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
#include "pal.h"

/** \brief PAL logger context structure */
typedef struct pal_logger
{
  /// Pointer to logger hardware
  void * logger_config_ptr;
  /// Receive complete flag
  volatile uint8_t logger_rx_flag;
  /// Transmit complete flag
  volatile uint8_t logger_tx_flag;

} pal_logger_t;

/**
 * \brief Initializes the logger port.
 *
 * \details
 * Initializes the logger port.
 * - Creates and initializes structure low level logging.<br>
 *
 * \pre
 * - None.
 *
 * \note
 * - None
 *
 * \param[in] p_logger_context    Valid pointer to the PAL logger context that should be initialized
 *
 * \retval    PAL_STATUS_SUCCESS  In case of successfully written to logger
 * \retval    PAL_STATUS_FAILURE  In case of failure while writing to logger
 *
 */
pal_status_t pal_logger_init(void * p_logger_context);

/**
 * \brief De-Initializes the logger port.
 *
 * \details
 * De-Initializes the logger port.
 * - Clears the structure low level logging.<br>
 *
 * \pre
 * - None.
 *
 * \note
 * - None
 *
 * \param[in] p_logger_context    Valid pointer to the PAL logger context that should be initialized
 *
 * \retval    PAL_STATUS_SUCCESS  In case of successfully written to logger
 * \retval    PAL_STATUS_FAILURE  In case of failure while writing to logger
 *
 */
pal_status_t pal_logger_deinit(void * p_logger_context);

/**
 * \brief Writes to logger port.
 *
 * \details
 * Writes to logger port.
 * - Invokes the platform dependent function to log the information provided.<br>
 *
 * \pre
 * - The pal_logger is initialized if required.
 *
 * \note
 * - None
 *
 * \param[in] p_logger_context    Valid pointer to the PAL logger context that should be initialized
 * \param[in] p_log_data          Pointer to the log data (data to be logged)
 * \param[in] log_data_length     Length of data to be logged.
 *
 * \retval    PAL_STATUS_SUCCESS  In case of successfully written to logger
 * \retval    PAL_STATUS_FAILURE  In case of failure while writing to logger
 *
 */
pal_status_t pal_logger_write(void * p_logger_context, const uint8_t * p_log_data, uint32_t log_data_length);

/**
 * \brief Read to logger port.
 *
 * \details
 * Reads to logger port.
 * - Invokes the platform dependent function to log the information provided.<br>
 *
 * \pre
 * - The pal_logger is initialized if required.
 *
 * \note
 * - None
 *
 * \param[in] p_logger_context    Valid pointer to the PAL logger context that should be initialized
 * \param[in] p_log_data          Pointer to the log data (data to be logged)
 * \param[in] log_data_length     Length of data to be logged.
 *
 * \retval    PAL_STATUS_SUCCESS  In case of successfully read to logger
 * \retval    PAL_STATUS_FAILURE  In case of failure while read to logger
 *
 */
pal_status_t pal_logger_read(void * p_logger_context, uint8_t * p_log_data, uint32_t log_data_length);

#ifdef __cplusplus
}
#endif

#endif /*_PAL_LOGGER_H_ */

/**
* @}
*/
