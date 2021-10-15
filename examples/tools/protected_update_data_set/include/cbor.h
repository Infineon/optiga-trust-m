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
* \file cbor.h
*
* \brief   This file defines APIs, types and data structures used for cbor format coding.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/

#ifndef _PROTECTED_UPDATE_CBOR_H_
#define _PROTECTED_UPDATE_CBOR_H_

#include <stdint.h>

// Encodes cbor NULL
int32_t cbor_set_null(uint8_t * buffer, uint16_t * offset);
// Encodes cbor array
int32_t cbor_set_array_of_data(uint8_t * buffer, uint32_t value, uint16_t * offset);
// Encodes cbor unsiged integer
int32_t cbor_set_unsigned_integer(uint8_t * buffer, uint32_t value, uint16_t * offset);
// Encodes cbor unsiged integer
int32_t cbor_set_signed_integer(uint8_t * buffer, int32_t value, uint16_t * offset);
// Encodes cbor byte string
int32_t cbor_set_byte_string(uint8_t * buffer, uint32_t value, uint16_t * offset);
// Set the tag for map
void cbor_set_map_tag(uint8_t * buffer, uint8_t map_number, uint16_t * offset);
// Encodes cbor map for unsigned type
int32_t cbor_set_map_unsigned_type(uint8_t * buffer, uint32_t key_data_item, uint32_t value_data_item, uint16_t * offset);
// Encodes cbor map for signed type
int32_t cbor_set_map_signed_type(uint8_t * buffer, uint32_t key_data_item, int32_t value_data_item, uint16_t * offset);
// Encodes cbor map for byte array type
int32_t cbor_set_map_byte_string_type(uint8_t * buffer, uint32_t key_data_item, const uint8_t * value_data_item, uint16_t value_data_item_len, uint16_t * offset);

#endif //_PROTECTED_UPDATE_CBOR_H_
/**
* @}
*/
