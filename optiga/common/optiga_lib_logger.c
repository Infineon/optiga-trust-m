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
* \file optiga_lib_logger.c
*
* \brief   This file implements the APIs for the OPTIGA library logger.
*
* \ingroup  grOptigaLibCommon
*
* @{
*/

#include "optiga/common/optiga_lib_common.h"
#include "optiga/pal/pal_logger.h"

#ifndef OPTIGA_LIB_ENABLE_LOGGING

#define optiga_lib_print_string
#define optiga_lib_print_string_with_newline
#define optiga_lib_print_hex_dump_no_cr
#define optiga_lib_print_hex_dump
#define optiga_lib_print_hexdump_ascii_

#else

#define OPTIGA_LOGGER_NEW_LINE_CHAR          0x0D, 0x0A
/// How many numbers per line in hexdump
#define HEXDUMP_COLS    		  16

void optiga_lib_print_string(const char_t * p_log_string)
{
    if (NULL == p_log_string)
    {
        return;
    }

    //lint --e{534} The return value is not used hence not checked*/
    pal_logger_write((const uint8_t *)p_log_string, strlen(p_log_string));
}

void optiga_lib_print_string_with_newline(const char_t * p_log_string)
{
    uint8_t buffer [2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};

    if (NULL == p_log_string)
    {
        return;
    }

    //lint --e{534} The return value is not used hence not checked*/
    pal_logger_write((const uint8_t *)p_log_string, strlen(p_log_string));
    //lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(buffer, 2);
}

/*-------------------------------------------------------------------------
   Hex dump data to screen (in debug mode) and/or log file (if open)
   (without address)
   1 - message
   2 - parameter: pointer to dumped data
   3 - parameter: dumped data length
  -------------------------------------------------------------------------*/
void optiga_lib_print_hex_dump_no_cr(uint8_t *data, int len)
{
	int i, j, slen;
    char str[200] = { 0 };
    uint8_t buffer [2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};

	for (i=0; i < len; i++, data++)
	{
		if ((i % 16) == 0 && i != 0)
		{
		    //lint --e{534} The return value is not used hence not checked*/
		    pal_logger_write((const uint8_t *)str, strlen(str));
		    pal_logger_write(buffer, 2);
        }
		sprintf(str + (i % 16) * 3, "%.2X ", *data);

	}
	//lint --e{534} The return value is not used hence not checked*/
	pal_logger_write((const uint8_t *)str, strlen(str));
	asm("nop");
}
//------------------------------------------------
void optiga_lib_print_hex_dump(uint8_t *data, int len)
{
	uint8_t buffer [2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};
	optiga_lib_print_hex_dump_no_cr(data, len);
    pal_logger_write(buffer, 2);
}

/**
 *
 * Printout data in a standard hex view
 *
 * @param[in] p_buf		Pointer to data which should be printed out.
 * @param[in] l_len		Length of a data
 *
 * @retval  None
 * @example
 0x000000: 2e 2f 68 65 78 64 75 6d ./hexdum
 0x000008: 70 00 53 53 48 5f 41 47 p.SSH_AG
 0x000010: 45 4e 54 5f             ENT_
 */
void optiga_lib_print_hexdump_ascii_(const void* p_buf, uint32_t l_len) {
	unsigned int i, j;
	uint8_t buffer [2] = {OPTIGA_LOGGER_NEW_LINE_CHAR};
	char str[255];

	for (i = 0;	i < l_len + ((l_len % HEXDUMP_COLS) ?
					( HEXDUMP_COLS - l_len % HEXDUMP_COLS) : 0);
			i++) {
		/* print offset */
		if (i % HEXDUMP_COLS == 0) {
			sprintf(str, "0x%06x: ", i);
			pal_logger_write((const uint8_t *)str, strlen(str));
		}

		/* print hex data */
		if (i < l_len) {
			sprintf(str, "%02x ", 0xFF & ((char*) p_buf)[i]);
			pal_logger_write((const uint8_t *)str, strlen(str));
		} else /* end of block, just aligning for ASCII dump */
		{
			sprintf(str, "   ");
			pal_logger_write((const uint8_t *)str, strlen(str));
		}

		/* print ASCII dump */
		if (i % HEXDUMP_COLS == ( HEXDUMP_COLS - 1)) {
			for (j = i - ( HEXDUMP_COLS - 1); j <= i; j++) {
				if (j >= l_len) /* end of block, not really printing */
				{
					sprintf(str, " ");
					pal_logger_write((const uint8_t *)str, strlen(str));
				} else if (isprint((int) ((char*) p_buf)[j])) /* printable char */
				{
					sprintf(str, "%c",((char*) p_buf)[j]);
					pal_logger_write((const uint8_t *)str, strlen(str));
				} else /* other char */
				{
					sprintf(str, ".");
					pal_logger_write((const uint8_t *)str, strlen(str));
				}
			}
			//lint --e{534} The return value is not used hence not checked*/
			pal_logger_write(buffer, 2);
		}
	}
}

#endif

/**
* @}
*/
