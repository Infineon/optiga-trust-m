/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the PAL unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef PAL_UNIT_TEST
#define PAL_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "optiga_lib_types.h"
#include "pal.h"
#include "pal_gpio.h"
#include "pal_i2c.h"
#include "pal_logger.h"
#include "pal_os_datastore.h"
#include "pal_os_event.h"
#include "pal_os_lock.h"
#include "pal_os_memory.h"
#include "pal_os_timer.h"

/* Go beyond Max bitrate */
#define PAL_I2C_MASTER_MAX_BITRATE_OVERFLOW 101

#endif  // PAL_UNIT_TEST
