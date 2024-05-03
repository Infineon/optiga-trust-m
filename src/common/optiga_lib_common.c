/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_common.c
 *
 * \brief   This file implements the commonly used functions by the OPTIGA Library .
 *
 * \ingroup  grOptigaLibCommon
 *
 * @{
 */

#include "optiga_lib_common.h"

#include "optiga_lib_types.h"

void optiga_common_set_uint16(uint8_t *p_output_buffer, uint16_t two_byte_value) {
    *p_output_buffer = (uint8_t)(two_byte_value >> 8);
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
}

void optiga_common_set_uint32(uint8_t *p_output_buffer, uint32_t four_byte_value) {
    *(p_output_buffer) = (uint8_t)(four_byte_value >> 24);
    *(p_output_buffer + 1) = (uint8_t)(four_byte_value >> 16);
    *(p_output_buffer + 2) = (uint8_t)(four_byte_value >> 8);
    *(p_output_buffer + 3) = (uint8_t)(four_byte_value);
}

uint32_t optiga_common_get_uint32(const uint8_t *p_input_buffer) {
    uint32_t four_byte_value;
    four_byte_value =
        ((uint32_t)(*p_input_buffer)) << 24
        | ((uint32_t)(*(p_input_buffer + 1)) << 16 | ((uint32_t)(*(p_input_buffer + 2))) << 8
           | (uint32_t)(*(p_input_buffer + 3)));

    return (four_byte_value);
}

void optiga_common_get_uint16(const uint8_t *p_input_buffer, uint16_t *p_two_byte_value) {
    *p_two_byte_value = (uint16_t)(*p_input_buffer << 8);
    *p_two_byte_value |= (uint16_t)(*(p_input_buffer + 1));
}

/**
 * @}
 */
