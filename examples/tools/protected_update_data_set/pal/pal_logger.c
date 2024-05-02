/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_logger.c
 *
 * \brief   This file defines APIs, types and data structures used for cbor format coding.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */
#include <stdio.h>

#include "pal\pal_file_system.h"
#include "pal\pal_logger.h"

#define PRINT_C_CODE_FORMAT_ENABLED 1

const uint8_t *dataset_file_path = NULL;

void pal_logger_print_byte(uint8_t datam) {
#if PRINT_C_CODE_FORMAT_ENABLED
    printf("0x%02X, ", datam);
#else

    printf("%02X ", datam);
#endif
}

void pal_logger_print_message(const int8_t *str) {
    printf("%s", str);
}

void pal_logger_print_format_message(const int8_t *str, ...) {
    printf("%s\n", str);
}

void pal_logger_print_hex_data(const uint8_t *data, uint16_t data_len) {
    uint32_t count = data_len;
    uint32_t index;
    for (index = 0; index < count; index++) {
        pal_logger_print_byte(data[index]);
        if (0 == ((index + 1)) % 16) {
            printf("\n\t");
        }
    }
    printf("\b");
}

void pal_logger_print_variable_name(uint8_t *var_name, uint8_t value) {
#if PRINT_C_CODE_FORMAT_ENABLED
    if (NULL != var_name) {
        if (0 == value) {
            printf("%s %s%s", "uint8_t", var_name, "[] = \n");
        } else {
            printf("%s %s%s%02d%s", "uint8_t", var_name, "_", value, "[] = \n");
        }
        printf("\t%s", "{\n\t");
    } else {
        printf("\n\t%s", "};");
    }
#endif
}

void pal_logger_print_to_file(int8_t *byte_array, uint16_t size) {
    if (NULL != dataset_file_path) {
        if (0
            != pal_file_system_write_to_file((const int8_t *)dataset_file_path, byte_array, size)) {
            pal_logger_print_message("Error : Unable to write to file\n");
        }
    }
}

/**
 * @}
 */
