/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file cbor.h
 *
 * \brief   This file defines APIs, types and data structures used for cbor format coding.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#ifndef _PROTECTED_UPDATE_CBOR_H_
#define _PROTECTED_UPDATE_CBOR_H_

#include <stdint.h>

// Encodes cbor NULL
int32_t cbor_set_null(uint8_t *buffer, uint16_t *offset);
// Encodes cbor array
int32_t cbor_set_array_of_data(uint8_t *buffer, uint32_t value, uint16_t *offset);
// Encodes cbor unsiged integer
int32_t cbor_set_unsigned_integer(uint8_t *buffer, uint32_t value, uint16_t *offset);
// Encodes cbor unsiged integer
int32_t cbor_set_signed_integer(uint8_t *buffer, int32_t value, uint16_t *offset);
// Encodes cbor byte string
int32_t cbor_set_byte_string(uint8_t *buffer, uint32_t value, uint16_t *offset);
// Set the tag for map
void cbor_set_map_tag(uint8_t *buffer, uint8_t map_number, uint16_t *offset);
// Encodes cbor map for unsigned type
int32_t cbor_set_map_unsigned_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    uint32_t value_data_item,
    uint16_t *offset
);
// Encodes cbor map for signed type
int32_t cbor_set_map_signed_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    int32_t value_data_item,
    uint16_t *offset
);
// Encodes cbor map for byte array type
int32_t cbor_set_map_byte_string_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    const uint8_t *value_data_item,
    uint16_t value_data_item_len,
    uint16_t *offset
);

#endif  //_PROTECTED_UPDATE_CBOR_H_
/**
 * @}
 */
