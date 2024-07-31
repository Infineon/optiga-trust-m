/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_logger.c
 *
 * \brief   This file implements the APIs for the OPTIGA library logger.
 *
 * \ingroup  grOptigaLibCommon
 *
 * @{
 */

#include "optiga_lib_logger.h"

#include "optiga_lib_common.h"
#include "pal_logger.h"
#include "pal_os_memory.h"

#define OPTIGA_LOGGER_NEW_LINE_CHAR 0x0D, 0x0A

extern pal_logger_t logger_console;

/*Convert Byte to HexString */
#define OPTIGA_LOGGER_CONVERT_BYTE_TO_HEX(hex_byte, p_hex_string, index) \
    { \
        uint8_t nibble; \
        nibble = (hex_byte & 0xF0) >> 4; \
        p_hex_string[index++] = ((nibble > 0x09)) ? (nibble + 0x37) : (nibble + 0x30); \
        nibble = hex_byte & 0x0F; \
        p_hex_string[index++] = ((nibble > 0x09)) ? (nibble + 0x37) : (nibble + 0x30); \
    }

/* Converts the uint8 array to hex string format */
_STATIC_H void optiga_lib_byte_to_hex_string(
    const uint8_t *p_array_buffer,
    uint8_t *p_hex_string,
    uint32_t length,
    bool_t is_input_byte_array
) {
    uint32_t loop = 0;
    uint8_t hex_byte = 0, index = 0;

    do {
        if ((NULL == p_array_buffer) || (NULL == p_hex_string)) {
            return;
        }

        for (loop = 0; loop < length; loop++) {
            index = 0;
            hex_byte = p_array_buffer[loop];
            if (TRUE == is_input_byte_array) {
                p_hex_string[index++] = '0';
                p_hex_string[index++] = 'x';
            }

            /*Convert Byte to HexString */
            OPTIGA_LOGGER_CONVERT_BYTE_TO_HEX(hex_byte, p_hex_string, index);

            p_hex_string[index++] = ' ';
            p_hex_string += index;
        }
        *p_hex_string = 0x00;
    } while (0);
}
/* Converts the uint16 value to hex string format */
_STATIC_H void optiga_lib_word_to_hex_string(uint16_t value, uint8_t *p_buffer) {
    uint8_t array_buffer[5] = {0};
    uint8_t loop, hex_byte = 0, index = 0;

    array_buffer[0] = (uint8_t)((value & 0xFF00) >> 8);
    array_buffer[1] = (uint8_t)((value & 0x00FF));

    p_buffer[index++] = '0';
    p_buffer[index++] = 'x';

    for (loop = 0; loop < 2; loop++) {
        hex_byte = array_buffer[loop];

        /*Convert Byte to HexString */
        OPTIGA_LOGGER_CONVERT_BYTE_TO_HEX(hex_byte, p_buffer, index);
    }
}

/* Converts the uint16 value to hex string format */
_STATIC_H void optiga_lib_print_length_of_data(uint16_t value) {
    uint8_t uint16t_conv_buffer[10] = {0};
    char_t print_buffer[50];

    sprintf((char_t *)print_buffer, "%s%s", OPTIGA_LAYER_EMPTY, "Length of data - ");
    strcat(print_buffer, OPTIGA_HEX_BYTE_SEPERATOR);
    optiga_lib_word_to_hex_string(value, uint16t_conv_buffer);
    strcat(print_buffer, (char_t *)uint16t_conv_buffer);

    optiga_lib_print_string_with_newline(print_buffer);
}

void optiga_lib_print_string(const char_t *p_log_string) {
    if (NULL == p_log_string) {
        return;
    }

    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(
        &logger_console,
        (const uint8_t *)p_log_string,
        (uint32_t)strlen(p_log_string)
    );
}

void optiga_lib_print_string_with_newline(const char_t *p_log_string) {
    uint8_t new_line_characters[2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};

    if (NULL == p_log_string) {
        return;
    }

    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(
        &logger_console,
        (const uint8_t *)p_log_string,
        (uint32_t)strlen(p_log_string)
    );
    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(&logger_console, new_line_characters, 2);
}

void optiga_lib_print_message(
    const char_t *p_log_string,
    const char_t *p_log_layer,
    const char_t *p_log_color
) {
    uint8_t new_line_characters[2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};
    char_t color_buffer[400];

    if ((NULL == p_log_string) || (NULL == p_log_layer) || (NULL == p_log_color)) {
        return;
    }

    OPTIGA_LIB_LOGGER_PRINT_INFO(color_buffer, p_log_string, p_log_layer, p_log_color);
    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(
        &logger_console,
        (const uint8_t *)color_buffer,
        (uint32_t)strlen(color_buffer)
    );
    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(&logger_console, new_line_characters, 2);
}

void optiga_lib_print_status(
    const char_t *p_log_layer,
    const char_t *p_log_color,
    uint16_t return_value
) {
    uint8_t new_line_characters[2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};
    uint8_t uint16t_conv_buffer[10] = {0};
    char_t return_value_buffer[20] = {0};
    char_t string_buffer[100] = {0};
    char_t color_buffer[400];

    if ((NULL == p_log_layer) || (NULL == p_log_color)) {
        return;
    }

    // if return value is successful, log SUCCESS
    if (OPTIGA_LIB_SUCCESS == return_value) {
        strcat(string_buffer, p_log_layer);
        strcat(string_buffer, "Passed");
        strcat(return_value_buffer, "");
    } else {
        strcat(string_buffer, p_log_layer);
        strcat(string_buffer, "Failed with return value - ");
        strcat(return_value_buffer, OPTIGA_HEX_BYTE_SEPERATOR);
        optiga_lib_word_to_hex_string(return_value, uint16t_conv_buffer);
        strcat(return_value_buffer, (char_t *)uint16t_conv_buffer);
    }

    OPTIGA_LIB_LOGGER_PRINT_INFO(
        color_buffer,
        (char_t *)return_value_buffer,
        string_buffer,
        p_log_color
    );

    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(
        &logger_console,
        (const uint8_t *)color_buffer,
        (uint32_t)strlen(color_buffer)
    );
    // Print new line
    // lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(&logger_console, new_line_characters, 2);
}

void optiga_lib_print_array_hex_format(
    const uint8_t *p_log_string,
    uint16_t length,
    const char_t *p_log_color
) {
    uint8_t temp_buffer[350];
    char_t output_buffer[400];
    uint16_t index;
    uint16_t temp_length;
    char_t new_line_characters[2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};
    uint8_t buffer_window = 16;  // Alignment of 16 bytes per line

    if ((NULL == p_log_string) || (NULL == p_log_color)) {
        return;
    }

    optiga_lib_print_length_of_data(length);

    // Logging the arrays in chunks of 16 bytes through chaining
    for (index = 0; index < length; index += buffer_window) {
        temp_length = buffer_window;
        if ((length - index) < buffer_window) {
            temp_length = length - index;
        }

        pal_os_memset(temp_buffer, 0x00, sizeof(temp_buffer));
        pal_os_memset(output_buffer, 0x00, sizeof(output_buffer));

        optiga_lib_byte_to_hex_string(
            (uint8_t *)(p_log_string + index),
            temp_buffer,
            temp_length,
            FALSE
        );

        OPTIGA_LIB_LOGGER_PRINT_ARRAY(output_buffer, temp_buffer, p_log_color);

        // New line characted entered at the end of each segment
        output_buffer[strlen(output_buffer)] = new_line_characters[0];
        output_buffer[strlen(output_buffer) + 1] = new_line_characters[1];
        // lint --e{534} The return value is not used hence not checked*/
        pal_logger_write(
            &logger_console,
            (const uint8_t *)output_buffer,
            (uint32_t)(strlen(output_buffer) + 2)
        );
    }
}

/**
 * @}
 */
