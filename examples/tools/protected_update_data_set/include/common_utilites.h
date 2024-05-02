/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file common_utilities.h
 *
 * \brief   This file defines APIs for common utilities.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#ifndef _PROTECTED_UPDATE_COMMON_UTILITIES_H_
#define _PROTECTED_UPDATE_COMMON_UTILITIES_H_

#include <stdint.h>

// Copies 2 bytes of uint16 type value to the buffer
void protected_tool_common_set_uint16(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint16_t two_byte_value
);

// Copies 3 bytes to the buffer
void protected_tool_common_set_uint24(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint32_t three_byte_value
);

// Copies 4 bytes of uint32 type value to the buffer
void protected_tool_common_set_uint32(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint32_t four_byte_value
);

// Copies 2 bytes of uint16 type value to the buffer without updating offset
void protected_tool_common_set_uint16_without_offset(
    uint8_t *p_output_buffer,
    uint16_t two_byte_value
);

#endif  //_PROTECTED_UPDATE_COMMON_UTILITIES_H_

/**
 * @}
 */
