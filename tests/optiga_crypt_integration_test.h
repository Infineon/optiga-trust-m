/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_crypt_integration_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the OPTIGA crypt Integration tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef OPTIGA_CRYPT_UNIT_TEST
#define OPTIGA_CRYPT_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/ccm.h"
#include "mbedtls/md.h"
#include "mbedtls/ssl.h"
#include "optiga_cmd.h"
#include "optiga_crypt.h"
#include "optiga_lib_common.h"
#include "optiga_util.h"
#include "pal_os_memory.h"

#define OPTIGA_EXAMPLE_UTIL_RSA_NEGATIVE_INTEGER (0x7F)
#define OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG (0x03)
#define OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS (0x00)

#define OPTIGA_HASH_TYPE_SHA_256 (0xE2)

#define OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(buffer, index, value) \
    { \
        uint16_t position = *index; \
        if (0xFF < value) { \
            buffer[position++] = (value & 0xFF00) >> 8; \
        } \
        buffer[position++] = (value & 0xFF); \
        *index = position; \
    }

/**
 * Prepare the hash context
 */
#define OPTIGA_HASH_CONTEXT_INIT(hash_context, p_context_buffer, context_buffer_size, hash_type) \
    { \
        hash_context.context_buffer = p_context_buffer; \
        hash_context.context_buffer_length = context_buffer_size; \
        hash_context.hash_algo = hash_type; \
    }

#endif  // OPTIGA_CRYPT_UNIT_TEST
