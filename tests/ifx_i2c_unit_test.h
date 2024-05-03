/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_unit_test.h
 *
 * \brief   This file defines APIs, types and data structures used in the infineon I2C unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef IFX_I2C_UNIT_TEST
#define IFX_I2C_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ifx_i2c.h"
#include "ifx_i2c_presentation_layer.h"
#include "pal_ifx_i2c_config.h"
#include "pal_os_event.h"

#define I2C_SEND_FRAMES (5)
#define I2C_RECV_FRAMES (5)

#define UT_SLAVE_ADDR (0x10)
#define PL_REG_BASE_ADDR_PERSISTANT (0x80)
#define PL_REG_BASE_ADDR_VOLATILE (0x00)

#define PRL_STATE_IDLE (0x01)

#define IFX_I2C_STATE_IDLE (0x02)
#define IFX_I2C_STATE_RESET_INIT (0xB3)

#endif  // IFX_I2C_UNIT_TEST
