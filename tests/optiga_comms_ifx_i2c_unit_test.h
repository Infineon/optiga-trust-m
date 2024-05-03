/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_comms_ifx_i2c_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the OPTIGA comms infineon I2C unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef OPTIGA_COMMS_IFX_I2C_UNIT_TEST
#define OPTIGA_COMMS_IFX_I2C_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ifx_i2c.h"
#include "ifx_i2c_presentation_layer.h"
#include "optiga_comms.h"
#include "optiga_lib_return_codes.h"
#include "pal.h"
#include "pal_os_event.h"

#define IFX_I2C_STATE_IDLE (0x02)
#define PRL_STATE_IDLE (0x01)

#define OPTIGA_COMMS_INUSE (0x01)
#define OPTIGA_COMMS_FREE (0x00)

void optiga_comms_ifx_i2c_unit_test(void);

#endif  // OPTIGA_COMMS_IFX_I2C_UNIT_TEST