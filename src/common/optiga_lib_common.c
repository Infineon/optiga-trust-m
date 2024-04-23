/**
* \copyright
* MIT License
*
* Copyright (c) 2021 Infineon Technologies AG
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
* \file optiga_lib_common.c
*
* \brief   This file implements the commonly used functions by the OPTIGA Library .
*
* \ingroup  grOptigaLibCommon
*
* @{
*/

#include "optiga_lib_types.h"
#include "optiga_lib_common.h"

void optiga_common_set_uint16 (uint8_t * p_output_buffer,uint16_t two_byte_value)
{
    *p_output_buffer = (uint8_t)(two_byte_value>>8);
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
}

void optiga_common_set_uint32 (uint8_t* p_output_buffer,uint32_t four_byte_value)
{
    *(p_output_buffer) = (uint8_t)(four_byte_value>>24);
    *(p_output_buffer + 1) = (uint8_t)(four_byte_value>>16);
    *(p_output_buffer + 2) = (uint8_t)(four_byte_value>>8);
    *(p_output_buffer + 3) = (uint8_t)(four_byte_value);
}

uint32_t optiga_common_get_uint32 (const uint8_t* p_input_buffer)
{
    uint32_t four_byte_value;
    four_byte_value = ((uint32_t)(*p_input_buffer))<< 24 | ((uint32_t)(*(p_input_buffer + 1))<< 16 |
                      ((uint32_t)(*(p_input_buffer + 2)))<< 8 | (uint32_t)(*(p_input_buffer + 3)));

    return (four_byte_value);
}

void optiga_common_get_uint16 (const uint8_t * p_input_buffer,uint16_t* p_two_byte_value)
{
    *p_two_byte_value = (uint16_t)(*p_input_buffer << 8);
    *p_two_byte_value |= (uint16_t)(*(p_input_buffer+1));
}

/**
* @}
*/
