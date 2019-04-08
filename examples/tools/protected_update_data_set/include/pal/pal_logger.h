/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
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
#include <stdio.h>
#include "protected_update_data_set.h"

// Prints messge
void pal_logger_print_message(const char * str);

// Prints hex data
void pal_logger_print_hex_data(unsigned char * data, unsigned short data_len);

// Prints protected update manifest
void pal_logger_print_manifest(protected_update_data_set_d * p_cbor_manifest);

//Prints protected update fragments
void pal_logger_print_fragments(protected_update_data_set_d * p_cbor_manifest);

/**
* @}
*/