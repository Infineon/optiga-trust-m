/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file cbor.c
 *
 * \brief   This file implements APIs, types and data structures used for cbor format coding.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#include "cbor.h"

#include "common_utilites.h"

#define CBOR_MAJOR_TYPE_0 (0x00)
#define CBOR_MAJOR_TYPE_1 (0x20)
#define CBOR_MAJOR_TYPE_2 (0x40)
// lint --e{750} suppress "macro kept as per cbor standard for future use"
#define CBOR_MAJOR_TYPE_3 (0x60)
#define CBOR_MAJOR_TYPE_4 (0x80)
#define CBOR_MAJOR_TYPE_5 (0xA0)
#define CBOR_MAJOR_TYPE_7 (0xF6)

#define CBOR_ADDITIONAL_TYPE_0x17 (0x17)
#define CBOR_ADDITIONAL_TYPE_0x18 (0x18)
#define CBOR_ADDITIONAL_TYPE_0x19 (0x19)
#define CBOR_ADDITIONAL_TYPE_0x1A (0x1A)

static int32_t
cbor_encode_data(uint8_t *buffer, uint32_t value, uint16_t *offset, uint8_t major_type) {
    int32_t status = 0;
    if (CBOR_ADDITIONAL_TYPE_0x17 >= value) {
        buffer[*offset] = (uint8_t)(major_type | value);
        (*offset)++;
    } else if ((CBOR_ADDITIONAL_TYPE_0x17 < value) && (0xFF >= value)) {
        buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x18);
        (*offset)++;
        buffer[*offset] = (uint8_t)value;
        (*offset)++;
    } else if ((0xFF < value) && (0xFFFF >= value)) {
        buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x19);
        (*offset)++;
        protected_tool_common_set_uint16(&buffer[*offset], offset, (uint16_t)value);
    } else if (0xFFFF < value) {
        buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x1A);
        (*offset)++;
        protected_tool_common_set_uint32(&buffer[*offset], offset, value);
    } else {
        status = -1;
    }
    return status;
}

int32_t cbor_set_null(uint8_t *buffer, uint16_t *offset) {
    buffer[*offset] = CBOR_MAJOR_TYPE_7;
    *offset += 1;
    return 1;
}

int32_t cbor_set_array_of_data(uint8_t *buffer, uint32_t value, uint16_t *offset) {
    buffer[*offset] = (CBOR_MAJOR_TYPE_4 | (uint8_t)value);
    *offset += 1;
    return 1;
}

int32_t cbor_set_unsigned_integer(uint8_t *buffer, uint32_t value, uint16_t *offset) {
    return (cbor_encode_data(buffer, value, offset, CBOR_MAJOR_TYPE_0));
}

// lint --e{702, 737} suppress "Shifting an int value"
int32_t cbor_set_signed_integer(uint8_t *buffer, int32_t value, uint16_t *offset) {
    /* adapted from code in RFC 7049 appendix C (pseudocode) */
    uint32_t temp_value = (value >> 31); /* extend sign to whole length */
    temp_value ^= value; /* complement negatives */
    return (cbor_encode_data(buffer, temp_value, offset, CBOR_MAJOR_TYPE_1));
}

int32_t cbor_set_byte_string(uint8_t *buffer, uint32_t value, uint16_t *offset) {
    return (cbor_encode_data(buffer, value, offset, CBOR_MAJOR_TYPE_2));
}

void cbor_set_map_tag(uint8_t *buffer, uint8_t map_number, uint16_t *offset) {
    (void)cbor_encode_data(buffer, map_number, offset, CBOR_MAJOR_TYPE_5);
}

int32_t cbor_set_map_unsigned_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    uint32_t value_data_item,
    uint16_t *offset
) {
    (void)cbor_set_unsigned_integer(buffer, key_data_item, offset);
    (void)cbor_set_unsigned_integer(buffer, value_data_item, offset);
    return 1;
}

int32_t cbor_set_map_signed_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    int32_t value_data_item,
    uint16_t *offset
) {
    (void)cbor_set_unsigned_integer(buffer, key_data_item, offset);
    (void)cbor_set_signed_integer(buffer, value_data_item, offset);
    return 1;
}

int32_t cbor_set_map_byte_string_type(
    uint8_t *buffer,
    uint32_t key_data_item,
    const uint8_t *value_data_item,
    uint16_t value_data_item_len,
    uint16_t *offset
) {
    uint16_t index;

    (void)cbor_set_unsigned_integer(buffer, key_data_item, offset);
    (void)cbor_set_byte_string(buffer, value_data_item_len, offset);
    for (index = 0; index < value_data_item_len; index++) {
        buffer[*offset] = *(((uint8_t *)value_data_item) + ((value_data_item_len - index) - 1));
        (*offset)++;
    }
    return 1;
}

/**
 * @}
 */
