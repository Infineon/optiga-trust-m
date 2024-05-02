/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_cmd_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the OPTIGA Command unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef OPTIGA_CMD_UNIT_TEST
#define OPTIGA_CMD_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "optiga_cmd.h"
#include "optiga_lib_common.h"
#include "optiga_util.h"
#include "pal_os_memory.h"

#define OPTIGA_UTIL_COUNT_DATA_OBJECT (0x02)

/// To set protection level in cmd instance
#define OPTIGA_SET_PROTECTION_LEVEL (0x00)

#define OPTIGA_CRYTP_RANDOM_PARAM_PRE_MASTER_SECRET (0x04)

#define OPTIGA_HASH_TYPE_SHA_256 (0xE2)

/** @brief For hash input as start */
#define OPTIGA_CRYPT_HASH_START (0x00)
/** @brief For hash input as start and final */
#define OPTIGA_CRYPT_HASH_START_FINAL (0x01)
/** @brief For hash input as continue */
#define OPTIGA_CRYPT_HASH_CONTINUE (0x02)
/** @brief For hash input as final */
#define OPTIGA_CRYPT_HASH_FINAL (0x03)
/** @brief For hash input as hash oid */
#define OPTIGA_CRYPT_HASH_FOR_OID (0x10)

/** @brief Data is provided by host*/
#define OPTIGA_CRYPT_HOST_DATA (0x01)

#define OPTIGA_EXAMPLE_UTIL_RSA_NEGATIVE_INTEGER (0x7F)
#define OPTIGA_EXAMPLE_UTIL_DER_BITSTRING_TAG (0x03)
#define OPTIGA_EXAMPLE_UTIL_DER_NUM_UNUSED_BITS (0x00)

/** @brief For symetric encrypt, dectypt or hmac input as start */
#define OPTIGA_CRYPT_SYM_START (0x00)
/** @brief For symetric encrypt, dectypt or hmac input as start and final */
#define OPTIGA_CRYPT_SYM_START_FINAL (0x01)
/** @brief For symetric encrypt, dectypt or hmac input as continue */
#define OPTIGA_CRYPT_SYM_CONTINUE (0x02)
/** @brief For symetric encrypt, dectypt or hmac input as final */
#define OPTIGA_CRYPT_SYM_FINAL (0x03)

#define OPTIGA_CRYPT_SYMMETRIC_ENCRYPTION (0x01)
/// Symmetric Decryption
#define OPTIGA_CRYPT_SYMMETRIC_DECRYPTION (0x00)

/// ECDSA FIPS 186-3 without hash
#define OPTIGA_CRYPT_ECDSA_FIPS_186_3_WITHOUT_HASH (0x11)
/// Elliptic Curve Diffie-Hellman key agreement algorithm
#define OPTIGA_CRYPT_ECDH_KEY_AGREEMENT_ALGORITHM (0x01)

#define OPTIGA_EXAMPLE_UTIL_SET_DER_LENGTH(buffer, index, value) \
    { \
        uint16_t position = *index; \
        if (0xFF < value) { \
            buffer[position++] = (value & 0xFF00) >> 8; \
        } \
        buffer[position++] = (value & 0xFF); \
        *index = position; \
    }

#endif  // OPTIGA_CMD_UNIT_TEST
