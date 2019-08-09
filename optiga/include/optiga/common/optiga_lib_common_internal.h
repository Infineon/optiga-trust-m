/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
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
* \file optiga_lib_common_internal.h
*
* \brief   This file provides the prototypes for the commonly used internal functions and structures of OPTIGA Library.
*
* \ingroup  grOptigaLibCommon
*
* @{
*/

#ifndef _OPTIGA_LIB_COMMON_INTERNAL_H_
#define _OPTIGA_LIB_COMMON_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/optiga_lib_config.h"

/// Determines the minimum of two values
#define MIN(a,b) (((a)<(b))?(a):(b))

/** @brief For hash input as start */
#define OPTIGA_CRYPT_HASH_START                         (0x00)
/** @brief For hash digest out */
#define OPTIGA_CRYPT_HASH_DIGEST_OUT                    (0x01)
/** @brief For hash input as continue */
#define OPTIGA_CRYPT_HASH_CONTINUE                      (0x02)
/** @brief For hash input as final */
#define OPTIGA_CRYPT_HASH_FINAL                         (0x03)
/** @brief For hash input as intermediate hash context */
#define OPTIGA_CRYPT_INTERMEDIATE                       (0x06)
/** @brief For hash input as hash context out */
#define OPTIGA_CRYPT_HASH_CONTX_OUT                     (0x07)
/** @brief For hash input as hash oid */
#define OPTIGA_CRYPT_HASH_FOR_OID                       (0x10)

/** @brief To restore saved secure session */
#define OPTIGA_COMMS_SESSION_CONTEXT_RESTORE            (0x11)
/** @brief To store active secure session */
#define OPTIGA_COMMS_SESSION_CONTEXT_SAVE               (0x22)
/** @brief For no manage context operation */
#define OPTIGA_COMMS_SESSION_CONTEXT_NONE               (0x33)

/// @cond hidden
/// Option to write count value to the data object
#define OPTIGA_UTIL_COUNT_DATA_OBJECT                   (0x02)
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
/// To set protection level in cmd instance
#define OPTIGA_SET_PROTECTION_LEVEL                     (0x00)
/// To set protocol version in cmd instance
#define OPTIGA_SET_PROTECTION_VERSION                   (0x01)
/// To set manage context in cmd instance
#define OPTIGA_SET_MANAGE_CONTEXT                       (0x02)
    /** @brief Enables the protected I2C communication with OPTIGA for command instances */
    #define OPTIGA_PROTECTION_ENABLE(p_mycmd, p_protection_mode)  { optiga_cmd_set_shielded_connection_option(p_mycmd, p_protection_mode->protection_level,OPTIGA_SET_PROTECTION_LEVEL) ;}
    /** @brief Select the protocol version required for the I2C protected communication for command instances */
    #define OPTIGA_PROTECTION_SET_VERSION(p_mycmd, p_version) {optiga_cmd_set_shielded_connection_option(p_mycmd , p_version->protocol_version,OPTIGA_SET_PROTECTION_VERSION);}
    /** @brief Select the OPTIGA comms session context save and restore option for command instances */
    #define OPTIGA_PROTECTION_MANAGE_CONTEXT(p_mycmd, operation) {optiga_cmd_set_shielded_connection_option(p_mycmd ,operation,OPTIGA_SET_MANAGE_CONTEXT);}
#else
    #define OPTIGA_PROTECTION_ENABLE(p_instance, p_protection_mode)  { }
    #define OPTIGA_PROTECTION_SET_VERSION(p_instance, p_version) {}
    #define OPTIGA_PROTECTION_MANAGE_CONTEXT(p_instance, p_operation) {}
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_LIB_COMMON_INTERNAL_H_ */

/**
* @}
*/
