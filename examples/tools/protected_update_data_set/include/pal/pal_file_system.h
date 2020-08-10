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
* \file pal_file_system.h
*
* \brief   This file defines APIs, types and data structures used for protect update tool file system.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/


#ifndef _PROTECTED_UPDATE_PAL_FILE_SYSTEM_
#define _PROTECTED_UPDATE_PAL_FILE_SYSTEM_

#include <stdint.h>

// Read file to a byte array
int32_t pal_file_system_read_file_to_array( const int8_t * file_name,
                                        uint8_t ** byte_array,
                                        uint16_t * byte_array_length);

int32_t pal_file_system_read_file_to_array_in_hex(  const int8_t * file_name, 
                                                uint8_t ** byte_array, 
                                                uint16_t * byte_array_length);

int32_t pal_file_system_write_to_file( const int8_t * file_name, 
                                                int8_t * byte_array, 
                                                uint16_t byte_array_length);
#endif //_PROTECTED_UPDATE_PAL_FILE_SYSTEM_

/**
* @}
*/