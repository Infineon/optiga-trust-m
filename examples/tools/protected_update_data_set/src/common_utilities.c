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
* \file common_utilities.c
*
* \brief   This file implements commonly used functions.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void protected_tool_common_set_uint16 (uint8_t * p_output_buffer,uint16_t * offset,uint16_t two_byte_value)
{
    *p_output_buffer = (uint8_t)(two_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint24 (uint8_t * p_output_buffer,uint16_t * offset, uint32_t three_byte_value)
{
    *p_output_buffer = (uint8_t)(three_byte_value >> 16);
    (*offset)++;
    *(p_output_buffer + 1)= (uint8_t)(three_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 2) = (uint8_t)(three_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint32 (uint8_t* p_output_buffer, uint16_t * offset, uint32_t four_byte_value)
{
    *(p_output_buffer) = (uint8_t)(four_byte_value >> 24);
    (*offset)++;
    *(p_output_buffer + 1) = (uint8_t)(four_byte_value >> 16);
    (*offset)++;
    *(p_output_buffer + 2) = (uint8_t)(four_byte_value >> 8);
    (*offset)++;
    *(p_output_buffer + 3) = (uint8_t)(four_byte_value);
    (*offset)++;
}

void protected_tool_common_set_uint16_without_offset (uint8_t * p_output_buffer, uint16_t two_byte_value)
{
    *p_output_buffer = (uint8_t)(two_byte_value >> 8);
    *(p_output_buffer + 1) = (uint8_t)(two_byte_value);
}

/**
* @}
*/

