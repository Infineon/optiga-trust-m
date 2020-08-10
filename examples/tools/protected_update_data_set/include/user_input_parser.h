/**
* \copyright
* MIT License
*
* Copyright (c) 2020 Infineon Technologies AG
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
int32_t tool_set_user_inputs(manifest_t* manifest_data);
//  Clears all inputs
int32_t tool_clear_inputs(manifest_t* p_manifest_data);

#endif //_PROTECTED_UPDATE_USER_INPUT_PARSER_H_

/**
* @}
*/
