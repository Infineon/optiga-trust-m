/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_crypt_mbedtls_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the OPTIGA PAL Crypt mbedtls Unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef PAL_CRYPT_MBEDTLS_UNIT_TEST
#define PAL_CRYPT_MBEDTLS_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mbedtls/version.h"
#include "optiga_lib_types.h"
#include "pal.h"
#include "pal_crypt.h"
#include "pal_gpio.h"
#include "pal_i2c.h"
#include "pal_logger.h"
#include "pal_os_datastore.h"
#include "pal_os_event.h"
#include "pal_os_lock.h"
#include "pal_os_timer.h"

#define RANDOM_KEY_LENGTH (32)
#define SESSION_KEY_LENGTH (16)
#define AES_BLOCK_SIZE (16)
#define NONCE_SIZE (12)
#define IFX_PRL_MAC_SIZE (8)

void print_hex(const unsigned char *data, size_t len);

void pal_crypt_version_unit_test(void);

void pal_crypt_tls_prf_sha256_unit_test(void);

void pal_crypt_encrypt_decrypt_aes128_ccm_unit_test(void);

#endif  // PAL_CRYPT_MBEDTLS_UNIT_TEST