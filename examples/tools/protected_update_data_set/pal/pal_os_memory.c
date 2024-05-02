/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_memory.c
 *
 * \brief   This file implements the platform abstraction layer APIs for memory.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *pal_os_malloc(uint32_t block_size) {
    return (malloc(block_size));
}

void *pal_os_calloc(uint32_t number_of_blocks, uint32_t block_size) {
    return (calloc(number_of_blocks, block_size));
}

void pal_os_free(void *p_block) {
    free(p_block);
}

void pal_os_memcpy(void *p_destination, const void *p_source, uint32_t size) {
    memcpy(p_destination, p_source, size);
}

void pal_os_memset(void *p_buffer, uint32_t value, uint32_t size) {
    memset(p_buffer, (int32_t)value, size);
}

/**
 * @}
 */
