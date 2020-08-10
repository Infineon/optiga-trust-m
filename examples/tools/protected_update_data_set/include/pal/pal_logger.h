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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Prints byte of data
void pal_logger_print_byte(uint8_t datam);

// Prints messge
void pal_logger_print_message(const int8_t * str);

// Prints hex data
void pal_logger_print_hex_data(const uint8_t * data, uint16_t data_len);

// Prints varibale name
void pal_logger_print_variable_name(uint8_t * var_name, uint8_t value);

// Prints data in file
void pal_logger_print_to_file(int8_t * byte_array, uint16_t size);

#endif //_PROTECTED_UPDATE_PAL_LOGGER_H_

/**
* @}
*/