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
* \file cbor.c
*
* \brief   This file implements APIs, types and data structures used for cbor format coding.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include <stdio.h>
#include "cbor.h"


static int cbor_encode_data(unsigned char * buffer, unsigned int value, unsigned short * offset, unsigned char major_type)
{
	int status = 0;
	if (value <= CBOR_ADDITIONAL_TYPE_0x17)
	{
		buffer[*offset] = (major_type | value);
		(*offset)++;
	}
	else if ((value > CBOR_ADDITIONAL_TYPE_0x17) && (value <= 0xFF))
	{
		buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x18);
		(*offset)++;
		buffer[*offset] = value;
		(*offset)++;
	}
	else if ((value > 0xff) && (value <= 0xFFFF))
	{
		buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x19);
		(*offset)++;
		buffer[*offset] = value >> 8;
		(*offset)++;
		buffer[*offset] = value;
		(*offset)++;
	}
	else if ((value > 0xFFFF) && (value <= 0xFFFFFFFF))
	{
		buffer[*offset] = (major_type | CBOR_ADDITIONAL_TYPE_0x1A);
		(*offset)++;
		buffer[*offset] = value >> 24;
		(*offset)++;
		buffer[*offset] = value >> 16;
		(*offset)++;
		buffer[*offset] = value >> 8;
		(*offset)++;
		buffer[*offset] = value;
		(*offset)++;
	}
	else
	{
		status = -1;
	}
	return status;

}
int cbor_set_null(unsigned char * buffer, unsigned short * offset)
{
	buffer[*offset] = CBOR_MAJOR_TYPE_7;
	*offset += 1;
	return 1;
}

int cbor_set_array_of_data(unsigned char * buffer,unsigned int value ,unsigned short * offset)
{
	buffer[*offset] = (CBOR_MAJOR_TYPE_4 | (unsigned char)value);
	*offset += 1;
	return 1;
}

int cbor_set_unsigned_integer(unsigned char * buffer, unsigned int value, unsigned short * offset)
{
	return (cbor_encode_data(buffer, value, offset, CBOR_MAJOR_TYPE_0));
}

int cbor_set_signed_integer(unsigned char * buffer, signed int value, unsigned short * offset)
{
    /* adapted from code in RFC 7049 appendix C (pseudocode) */
    unsigned int temp_value = value >> 31;              /* extend sign to whole length */
    temp_value ^= value;                            /* complement negatives */
	return (cbor_encode_data(buffer, temp_value, offset, CBOR_MAJOR_TYPE_1));
}

int cbor_set_byte_string(unsigned char * buffer, unsigned int value, unsigned short * offset)
{
	return (cbor_encode_data(buffer, value, offset, CBOR_MAJOR_TYPE_2));
}

void cbor_set_map_tag(unsigned char * buffer, unsigned char map_number, unsigned short * offset)
{
	cbor_encode_data(buffer, map_number, offset, CBOR_MAJOR_TYPE_5);
}

int cbor_set_map_unsigned_type(unsigned char * buffer, unsigned int key_data_item, unsigned int value_data_item, unsigned short * offset)
{
	cbor_set_unsigned_integer(buffer, key_data_item, offset);
	cbor_set_unsigned_integer(buffer, value_data_item, offset);
	return 1;
}

int cbor_set_map_signed_type(unsigned char * buffer, unsigned int key_data_item, signed int value_data_item, unsigned short * offset)
{
	cbor_set_unsigned_integer(buffer, key_data_item, offset);
	cbor_set_signed_integer(buffer, value_data_item, offset);
	return 1;
}

int cbor_set_map_byte_string_type(unsigned char * buffer, unsigned int key_data_item, unsigned char * value_data_item, unsigned short value_data_item_len, unsigned short * offset)
{
	unsigned short index = 0;

	cbor_set_unsigned_integer(buffer, key_data_item, offset);
	cbor_set_byte_string(buffer, value_data_item_len, offset);
	for(index = 0; index < value_data_item_len; index++)
	{
		buffer[*offset] = *(((unsigned char *)value_data_item) + (value_data_item_len - index - 1));
		(*offset)++;
	}
	return 1;
}

/**
* @}
*/