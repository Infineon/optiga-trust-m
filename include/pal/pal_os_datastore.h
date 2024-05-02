/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
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

#include "pal.h"

/// Identifier to store and read OPTIGA Platform Binding Shared secret on host platform
#define OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID (0x11)

/// Maximum shared secret length this value is bind with OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID
#define OPTIGA_SHARED_SECRET_MAX_LENGTH (0x40)

// Persistant data store is not configured
#define OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED (0x00)

// !!!OPTIGA_LIB_PORTING_REQUIRED
// Identifier to store and read OPTIGA Shielded connection manage context on host platform,
// If the manage context data is to be stored in volatile memory only,
// set OPTIGA_COMMS_MANAGE_CONTEXT_ID to OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED.
#define OPTIGA_COMMS_MANAGE_CONTEXT_ID (0x22)

// !!!OPTIGA_LIB_PORTING_REQUIRED
// Identifier to store and read OPTIGA application context handle on host platform,
// If the application context data is to be stored in volatile memory only,
// set OPTIGA_COMMS_MANAGE_CONTEXT_ID to OPTIGA_LIB_PAL_DATA_STORE_NOT_CONFIGURED.
#define OPTIGA_HIBERNATE_CONTEXT_ID (0x33)

/// @cond hidden
/// Size of application context handle buffer
#define APP_CONTEXT_SIZE (0x08)
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
 * \param[in,out] p_buffer_length       Valid Pointer to the data buffer length to store the read data length. The input value gets updated with the actual length read from the data store.
 *
 * \retval        #PAL_STATUS_SUCCESS   On successful execution
 * \retval        #PAL_STATUS_FAILURE   On failure
 */
pal_status_t
pal_os_datastore_read(uint16_t datastore_id, uint8_t *p_buffer, uint16_t *p_buffer_length);

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
pal_status_t
pal_os_datastore_write(uint16_t datastore_id, const uint8_t *p_buffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_OS_DATASTORE_H_ */

/**
 * @}
 */
