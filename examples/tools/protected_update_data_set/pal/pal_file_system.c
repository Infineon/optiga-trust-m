
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
* \file pal_file_system.c
*
* \brief   This file implements APIs, types and data structures used for protect update tool file system.
*
* \ingroup  grProtectedUpdateTool
*
* @{
*/
#include <stdio.h>
#include <stdlib.h>
#include "pal\pal_logger.h"

int pal_file_system_read_file_to_array(	const unsigned char * file_name, 
										unsigned char ** byte_array, 
										unsigned short * byte_array_length)
{
	int status = -1;

	FILE * fp = NULL;
	int filelen;
	unsigned char * buffer;

	do
	{
		if (NULL == file_name)
		{
			pal_logger_print_message(" FAILED : pal_file_system : file name is NULL");
			break;
		}
		fp = fopen(file_name, "rb");
		if (NULL == fp)
		{
			pal_logger_print_message(" FAILED : pal_file_system : fopen");
			break;
		}
		// Jump to the end of the file
		fseek(fp, 0, SEEK_END);      
		// Get the current byte offset in the file
		filelen = ftell(fp);       
		// Jump back to the beginning of the file
		rewind(fp);                      
		// Enough memory for file + \0
		buffer = (char *)malloc((filelen)*sizeof(char)); 
		// Read in the entire file
		fread(buffer, filelen, 1, fp); 
		*byte_array = buffer;
		*byte_array_length = filelen;
		status = 0;
	} while (0);
	if (NULL != fp)
	{
		fclose(fp); 
	}
	return status;
}


/**
* @}
*/

