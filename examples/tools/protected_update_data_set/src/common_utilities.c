/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file common_utilities.c
 *
 * \brief   This file implements commonly used functions.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void protected_tool_common_set_uint16(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint16_t two_byte_value
) {
    *p_output_buffer = (uint8_t)(two_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint24(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint32_t three_byte_value
) {
    *p_output_buffer = (uint8_t)(three_byte_value >> 16);
    (*offset)++;
    *(p_output_buffer + 1) = (uint8_t)(three_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 2) = (uint8_t)(three_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint32(
    uint8_t *p_output_buffer,
    uint16_t *offset,
    uint32_t four_byte_value
) {
    *(p_output_buffer) = (uint8_t)(four_byte_value >> 24);
    (*offset)++;
    *(p_output_buffer + 1) = (uint8_t)(four_byte_value >> 16);
    (*offset)++;
    *(p_output_buffer + 2) = (uint8_t)(four_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 3) = (uint8_t)(four_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint16_without_offset(
    uint8_t *p_output_buffer,
    uint16_t two_byte_value
) {
    *p_output_buffer = (uint8_t)(two_byte_value >> 8);
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
}

/**
 * @}
 */
