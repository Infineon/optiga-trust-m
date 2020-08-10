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
* \brief   This file provides the prototype declarations of PAL OS MEMORY.
*
* \ingroup  grPAL
*
* @{
*/


#ifndef _PAL_OS_MEMORY_H_
#define _PAL_OS_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"


/**
 * \brief Allocates a block of memory specified by the block size and return the pointer to it.
 *
 * <br>
 *
 * \details
 * - Allocates a block of memory specified by the block size and return the pointer to it
 * 
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] block_size         Size of the block
 *
 * \retval  Block Pointer  Memory allocation is successful
 * \retval  NULL           Memory allocation is not successful
 */
LIBRARY_EXPORTS void * pal_os_malloc(uint32_t block_size);

/**
 * @brief Allocates a block of memory specified by the block size and return the pointer to it.
 *
 * <br>
 *
 * \details
 * - Allocates a block of memory specified by the block size and return the pointer to it
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] number_of_blocks      Number of block to allocate
 * \param[in] block_size            Size of the block
 *
 * \retval  Block Pointer  Memory allocation is successful
 * \retval  NULL           Memory allocation is not successful
 */
LIBRARY_EXPORTS void * pal_os_calloc(uint32_t number_of_blocks , uint32_t block_size);

/**
 * @brief Frees the block of memory specified by the block pointer.
 *
 * <br>
 *
 * \details
 * - Frees the block of memory specified by the block pointer
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] block      Pointer to memory block to be freed
 *
 */
LIBRARY_EXPORTS void pal_os_free(void * block);

/**
 * @brief Copies data from source to destination.
 *
 * <br>
 *
 * \details
 * - Copies data from source to destination
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_destination      Source to copy the data from
 * \param[in] p_source           Destination to copy the data to
 * \param[in] size               Size of the data to copy
 *
 */
LIBRARY_EXPORTS void pal_os_memcpy(void * p_destination, const void * p_source, uint32_t size);

/**
 * @brief Sets the buffer with the given value.
 *
 * <br>
 *
 * \details
 * - Sets the buffer with the given value
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_buffer      Pointer to buffer
 * \param[in] value         Value to be set in the buffer
 * \param[in] size          Size of the buffer
 *
 */
LIBRARY_EXPORTS void pal_os_memset(void * p_buffer, uint32_t value, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_OS_MEMORY_H_ */

/**
* @}
*/

