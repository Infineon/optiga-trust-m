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
* \file pal_logger.c
*
* \brief   This file defines APIs, types and data structures used for cbor format coding.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include <stdio.h>
#include "protected_update_data_set.h"

#define PRINT_C_CODE_FORMAT_ENABLED	1

void pal_logger_print_byte(unsigned char datam)
{
#if PRINT_C_CODE_FORMAT_ENABLED
	printf("0x%02X, ", datam);
#else

	printf("%02X ", datam);
#endif

}

void pal_logger_print_message(const char * str)
{
	printf("%s\n", str);
}

void pal_logger_print_hex_data(unsigned char * data, 
								unsigned int data_len)
{
	unsigned int count = data_len;
	unsigned int index = 0;
	for (index = 0; index < count; index++)
	{
		pal_logger_print_byte(data[index]);
		if (0 == ((index + 1)) % 16)
		{
			printf("\n\t");
		}
	}
}

void pal_logger_print_variable_name(unsigned char * var_name, 
									unsigned char value)
{
#if PRINT_C_CODE_FORMAT_ENABLED
	if (NULL != var_name)
	{
		if (0 == value)
		{
			printf("%s %s%s", "unsigned char", var_name, "[] = \n");
		}
		else
		{
			printf("%s %s%s%02d%s", "unsigned char", var_name,"_", value,"[] = \n");
		}
		printf("\t%s", "{\n\t");
	}
	else
	{
		printf("\n\t%s", "};");
	}
#endif
}


void pal_logger_print_manifest(protected_update_data_set_d * p_cbor_manifest)
{
	unsigned short count = p_cbor_manifest->data_length;
	unsigned short index = 0;
	printf("Manifest Data , size : [%03d]\n\t", p_cbor_manifest->data_length);
	pal_logger_print_variable_name("manifest_data", 0);
	for (index = 0; index < count; index++)
	{
		pal_logger_print_byte(p_cbor_manifest->data[index]);
		if (0 == ((index + 1)) % 16)
		{
			printf("\n\t");
		}
	}
	pal_logger_print_variable_name(NULL,0);
	pal_logger_print_message("");
}

void pal_logger_print_fragments(protected_update_data_set_d * p_cbor_manifest)
{
	unsigned short count = p_cbor_manifest->fragments_length;
	unsigned short index = 0;
	unsigned short fragment_length;

	for (index = 0; index < count; index++)
	{
		if ((index % MAX_PAYLOAD_SIZE) == 0)
		{
			fragment_length = (p_cbor_manifest->fragments_length - index );
			fragment_length = (fragment_length > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : fragment_length;

			if (0 != index)	{ pal_logger_print_variable_name(NULL, 0); }
			printf("\n\nFragment number:[%02d], size:[%03d]\n\t", (index / MAX_PAYLOAD_SIZE)+1, fragment_length);
			pal_logger_print_variable_name("fragment", (index / MAX_PAYLOAD_SIZE) + 1);
		}
		pal_logger_print_byte(p_cbor_manifest->fragments[index]);
		if (0 == ((index + 1)) % 16)
		{
			printf("\n\t");
		}
	}
	if (count == index)	{ pal_logger_print_variable_name(NULL, 0); }
}

/**
* @}
*/
