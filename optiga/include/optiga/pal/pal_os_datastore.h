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
* \file pal_os_datastore.h
*
* \brief   This file provides API prototypes of platform abstraction layer for datastore operations.
*
* \ingroup  grPAL
*
* @{
*/


#ifndef _PAL_OS_DATASTORE_H_
#define _PAL_OS_DATASTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/pal/pal.h"

/// Identifier to store and read OPTIGA Platform Binding Shared secret on host platform
#define OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID        (0x11)


// Persistant data store is not configured
#define OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED        (0x00)

// !!!OPTIGA_LIB_PORTING_REQUIRED
// Identifier to store and read OPTIGA Shielded connection manage context on host platform,
// If the manage context data is to be stored in volatile memory only, 
// set OPTIGA_COMMS_MANAGE_CONTEXT_ID to OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED.
#define OPTIGA_COMMS_MANAGE_CONTEXT_ID                  (0x22)

// !!!OPTIGA_LIB_PORTING_REQUIRED
// Identifier to store and read OPTIGA application context handle on host platform, 
// If the application context data is to be stored in volatile memory only, 
// set OPTIGA_COMMS_MANAGE_CONTEXT_ID to OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED.
#define OPTIGA_HIBERNATE_CONTEXT_ID                     (0x33)

/// @cond hidden
/// Size of application context handle buffer
#define APP_CONTEXT_SIZE        (0x08)
/// @endcond
/**
 * \brief Reads data from the specified location for a given datastore_id.
 *
 * \details
 * Reads data from the specified location for a given datastore_id.
 *  - Reads data from any configured specific location.
 *  - Once the timer expires, the registered callback function gets called from the timer event handler, if the call back is not NULL.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in]     datastore_id          Datastore id from where the data should be read.
 * \param[out]    p_buffer              Valid Pointer to output buffer
 * \param[in,out] p_buffer_length       Valid Pointer to the data buffer length to store the read data length. The input value must be the data length to be read.
 *
 * \retval        #PAL_STATUS_SUCCESS   On successful execution
 * \retval        #PAL_STATUS_FAILURE   On failure
 */
pal_status_t pal_os_datastore_read(uint16_t datastore_id,
                                   uint8_t * p_buffer,
                                   uint16_t * p_buffer_length);

/**
 * \brief Writes data to the specified location for a given datastore_id
 *
 * \details
 * Writes data to the specified location for a given datastore_id.
 * - Writes data to any configured specific location.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] datastore_id          Datastore id where the data should be written.
 * \param[in] p_buffer              Valid pointer to the input buffer
 * \param[in] length                Length of the data to be written
 *
 * \retval    #PAL_STATUS_SUCCESS   On successful execution
 * \retval    #PAL_STATUS_FAILURE   On failure
 */
pal_status_t pal_os_datastore_write(uint16_t datastore_id,
                                    const uint8_t * p_buffer,
                                    uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_OS_DATASTORE_H_ */

/**
* @}
*/
