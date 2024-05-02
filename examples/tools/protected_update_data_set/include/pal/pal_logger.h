/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_logger.h
 *
 * \brief   This file defines APIs, types and data structures used for protected update logger.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */
#ifndef _PROTECTED_UPDATE_PAL_LOGGER_H_
#define _PROTECTED_UPDATE_PAL_LOGGER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Prints byte of data
void pal_logger_print_byte(uint8_t datam);

// Prints messge
void pal_logger_print_message(const int8_t *str);

// Prints hex data
void pal_logger_print_hex_data(const uint8_t *data, uint16_t data_len);

// Prints varibale name
void pal_logger_print_variable_name(uint8_t *var_name, uint8_t value);

// Prints data in file
void pal_logger_print_to_file(int8_t *byte_array, uint16_t size);

#endif  //_PROTECTED_UPDATE_PAL_LOGGER_H_

/**
 * @}
 */