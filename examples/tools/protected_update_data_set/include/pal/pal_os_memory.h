/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_os_memory.h
 *
 * \brief   This file defines APIs, for protected update tool memory system.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#ifndef _PROTECTED_UPDATE_PAL_OS_MEMORY_H_
#define _PROTECTED_UPDATE_PAL_OS_MEMORY_H_

#include <stdint.h>

// Allocates a block of memory specified by the block size and return the pointer to it
void *pal_os_malloc(uint32_t block_size);

// Allocates a block of memory specified by the block size and return the pointer to it
void *pal_os_calloc(uint32_t number_of_blocks, uint32_t block_size);

// Frees the block of memory specified by the block pointer
void pal_os_free(void *p_block);

// Copies data from source to destination
void pal_os_memcpy(void *p_destination, const void *p_source, uint32_t size);

// Sets the buffer with the given value
void pal_os_memset(void *p_buffer, uint32_t value, uint32_t size);

#endif  //_PROTECTED_UPDATE_PAL_OS_MEMORY_H_

/**
 * @}
 */