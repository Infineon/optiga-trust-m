/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_file_system.h
 *
 * \brief   This file defines APIs, types and data structures used for protect update tool file system.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#ifndef _PROTECTED_UPDATE_PAL_FILE_SYSTEM_
#define _PROTECTED_UPDATE_PAL_FILE_SYSTEM_

#include <stdint.h>

// Read file to a byte array
int32_t pal_file_system_read_file_to_array(
    const int8_t *file_name,
    uint8_t **byte_array,
    uint16_t *byte_array_length
);

int32_t pal_file_system_read_file_to_array_in_hex(
    const int8_t *file_name,
    uint8_t **byte_array,
    uint16_t *byte_array_length
);

int32_t pal_file_system_write_to_file(
    const int8_t *file_name,
    int8_t *byte_array,
    uint16_t byte_array_length
);
#endif  //_PROTECTED_UPDATE_PAL_FILE_SYSTEM_

/**
 * @}
 */