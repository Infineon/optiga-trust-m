/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_file_system.c
 *
 * \brief   This file implements APIs, types and data structures used for protect update tool file system.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */
#include <stdio.h>
#include <stdlib.h>

#include "pal\pal_logger.h"
#include "pal\pal_os_memory.h"

int32_t pal_file_system_read_file_to_array(
    const int8_t *file_name,
    uint8_t **byte_array,
    uint16_t *byte_array_length
) {
    int32_t status = -1;

    FILE *fp = NULL;
    uint32_t filelen;
    int8_t *buffer = NULL;

    do {
        if (NULL == file_name) {
            pal_logger_print_message(" Error : File name is NULL\n");
            break;
        }
        fp = fopen(file_name, "rb");
        if (NULL == fp) {
            pal_logger_print_message(" Error : Failed in fopen\n");
            break;
        }
        // Jump to the end of the file
        (void)fseek(fp, 0, SEEK_END);
        // Get the current byte offset in the file
        filelen = (uint32_t)ftell(fp);
        // Jump back to the beginning of the file
        rewind(fp);
        // Enough memory for file + \0
        buffer = (int8_t *)pal_os_malloc((filelen) * sizeof(int8_t));
        // Read in the entire file
        if (NULL != buffer) {
            (void)fread(buffer, filelen, 1, fp);
            *byte_array = (uint8_t *)buffer;
            *byte_array_length = (uint16_t)filelen;
            status = 0;
        }
    } while (0);
    if (NULL != fp) {
        (void)fclose(fp);
    }
    return status;
}

int32_t pal_file_system_read_file_to_array_in_hex(
    const int8_t *file_name,
    uint8_t **byte_array,
    uint16_t *byte_array_length
) {
    int32_t status = -1;

    FILE *fp = NULL;
    int32_t idx = 0;
    uint32_t val;
    uint32_t filelen = 0;
    int8_t *buffer = NULL;
    uint32_t data_copied_count = 0;
    int32_t c;

    do {
        if (NULL == file_name) {
            pal_logger_print_message(" Error : File name is NULL\n");
            break;
        }
        fp = fopen(file_name, "rb");
        if (NULL == fp) {
            pal_logger_print_message(" Error : Failed in fopen\n");
            break;
        }

        // Get the number of characters in file
        for (c = getc(fp); c != EOF; c = getc(fp)) {
            // Increment count for this character
            if (('\r' != (uint8_t)c) && ('\n' != (uint8_t)c)) {
                filelen = filelen + 1;
            }
        }
        filelen = filelen / 2;

        // Jump back to the beginning of the file
        rewind(fp);

        // Enough memory for file + \0
        buffer = (int8_t *)pal_os_malloc((filelen) * sizeof(int8_t));
        if (NULL == buffer) {
            break;
        }

        while (fscanf(fp, "%2x", &val) != EOF) {
            buffer[idx++] = (int8_t)val;
            data_copied_count++;
        }

        if (filelen != data_copied_count) {
            break;
        }
        *byte_array = (uint8_t *)buffer;
        *byte_array_length = (uint16_t)filelen;
        status = 0;
    } while (0);

    if (NULL != fp) {
        (void)fclose(fp);
    }
    return status;
}

// lint --e{715} suppress "byte_array_length is kept in case where number of bytes write needs to be mentioned"
int32_t pal_file_system_write_to_file(
    const int8_t *file_name,
    int8_t *byte_array,
    uint16_t byte_array_length
) {
    int32_t status = -1;

    FILE *fp = NULL;

    do {
        if (NULL == file_name) {
            pal_logger_print_message(" Error : File name is NULL");
            break;
        }
        fp = fopen(file_name, "a+");
        if (NULL == fp) {
            pal_logger_print_message(" Error : Failed in fopen");
            break;
        }

        fprintf(fp, "%s", byte_array);
        status = 0;
    } while (0);

    if (NULL != fp) {
        (void)fclose(fp);
    }
    return status;
}

/**
 * @}
 */
