/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file user_input_parser.h
 *
 * \brief   This file defines APIs, types and data structures used for protected update logger.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#ifndef _PROTECTED_UPDATE_USER_INPUT_PARSER_H_
#define _PROTECTED_UPDATE_USER_INPUT_PARSER_H_

#include <stdint.h>

//  Gets the user inputs
int32_t tool_get_user_inputs(int32_t argc, int8_t *argv[]);
//  Sets the user inputs
int32_t tool_set_user_inputs(manifest_t *manifest_data);
//  Clears all inputs
int32_t tool_clear_inputs(manifest_t *p_manifest_data);

#endif  //_PROTECTED_UPDATE_USER_INPUT_PARSER_H_

/**
 * @}
 */
