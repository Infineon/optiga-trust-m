/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_gpio.c
 *
 * \brief   This file implements the platform abstraction layer APIs for GPIO.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_os_memory.h"

#include <zephyr/kernel.h>

void *pal_os_malloc(uint32_t block_size) {
    return (k_malloc(block_size));
}

void *pal_os_calloc(uint32_t number_of_blocks, uint32_t block_size) {
    return (k_calloc(number_of_blocks, block_size));
}

void pal_os_free(void *p_block) {
    k_free(p_block);
}

void pal_os_memcpy(void *p_destination, const void *p_source, uint32_t size) {
    // flawfinder: ignore
    memcpy(p_destination, p_source, size);
}

void pal_os_memset(void *p_buffer, uint32_t value, uint32_t size) {
    memset(p_buffer, (int32_t)value, size);
}
