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

#else

#define OPTIGA_LOGGER_NEW_LINE_CHAR          0x0D, 0x0A
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

#endif

/**
* @}
*/
