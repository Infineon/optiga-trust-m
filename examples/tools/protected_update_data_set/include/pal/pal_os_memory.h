/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
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
void * pal_os_malloc(uint32_t block_size);

// Allocates a block of memory specified by the block size and return the pointer to it
void * pal_os_calloc(uint32_t number_of_blocks , uint32_t block_size);

// Frees the block of memory specified by the block pointer
void pal_os_free(void * p_block);

// Copies data from source to destination
void pal_os_memcpy(void * p_destination, const void * p_source, uint32_t size);

// Sets the buffer with the given value
void pal_os_memset(void * p_buffer, uint32_t value, uint32_t size);

#endif //_PROTECTED_UPDATE_PAL_OS_MEMORY_H_

/**
* @}
*/