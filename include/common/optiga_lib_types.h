/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_types.h
 *
 * \brief   This file contains the type definitions for the fundamental data types.
 *
 * \ingroup grOptigaLibCommon
 *
 * @{
 */

#ifndef _OPTIGA_LIB_TYPES_H_
#define _OPTIGA_LIB_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @cond hidden
#ifdef _OPTIGA_EXPORTS_DLLEXPORT_H_

#ifdef OPTIGA_LIB_EXPORTS
#define LIBRARY_EXPORTS __declspec(dllexport)
#else
#define LIBRARY_EXPORTS __declspec(dllimport)
#endif  // OPTIGA_LIB_EXPORTS

#else
#define LIBRARY_EXPORTS
#endif /*_OPTIGA_EXPORTS_DLLEXPORT_H_*/
/// @endcond

/// Definition for false
#ifndef FALSE
#define FALSE (0U)
#endif

/// Definition for true
#ifndef TRUE
#define TRUE (1U)
#endif

/// Typedef for one byte integer
typedef char char_t;

/// Typedef for a double word
typedef double double_t;

/// Typedef for a float
typedef float float_t;

/// Typedef for a boolean
typedef uint8_t bool_t;

/// typedef for handles
typedef void *hdl_t;

/// typedef for OPTIGA host library status
typedef uint16_t optiga_lib_status_t;

/// Typedef for a void
typedef void Void;

/// typedef for application event handler
typedef void (*upper_layer_callback_t)(void *upper_layer_ctx, optiga_lib_status_t event);

/// typedef for event callback handler
typedef void (*callback_handler_t)(void *callback_ctx, optiga_lib_status_t event);

/**
 * \brief Structure to specify a byte stream consisting of length and data pointer.
 */
typedef struct data_blob {
    /// Pointer to byte array which contains the data stream
    uint8_t *data_ptr;
    /// Length of the byte stream
    uint16_t length;
} data_blob_t;

#ifndef _NO_STATIC_H
#define _STATIC_H static
#else
#define _STATIC_H
#endif

#ifndef _STATIC_INLINE
#define _STATIC_INLINE static inline
#endif

#ifdef __cplusplus
}
#endif

#endif /* _OPTIGA_LIB_TYPES_H_  */

/**
 * @}
 */
