/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_data_layer_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in infineon i2c data layer unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef IFX_I2C_DATA_LAYER_UNIT_TEST
#define IFX_I2C_DATA_LAYER_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ifx_i2c.h"
#include "ifx_i2c_data_link_layer.h"
#include "ifx_i2c_physical_layer.h"
#include "pal_ifx_i2c_config.h"

#define PL_SEND_FRAMES (5)
#define PL_RECV_FRAMES (5)

#define PL_STATE_READY (0x02)
#define DL_STATE_IDLE (0x01)

#define UT_SLAVE_ADDR (0x10)

#endif  // IFX_I2C_DATA_LAYER_UNIT_TEST
