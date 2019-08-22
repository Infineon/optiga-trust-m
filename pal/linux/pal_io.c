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
* \file pal_io.c
*
* \brief   This file implements the platform abstraction layer APIs for output.
*
* \ingroup grPAL
* @{
*/

#include "optiga/common/optiga_lib_return_codes.h"
#include "optiga/common/optiga_lib_logger.h"
#include "optiga/pal/pal_uart.h"

/**
* Rend Data
*
*/
//lint --e{714} suppress "Functions are only used for testing purpose"
int32_t uart_read_data (uint32_t PdwUartPort, uint8_t* PprgbBuf, uint32_t PdwDataLen)
{
    int32_t i4Status = 0xFFFF;
    return i4Status;
}

/**
* Print out Data
*
*/
//lint --e{714} suppress "Functions are only used for testing purpose"
int32_t uart_write_data(uint32_t PdwUartPort, uint8_t* PprgbBuf, uint32_t PdwDataLen)
{
    int32_t i4Status=0;
    printf("%s", PprgbBuf);
    return i4Status;
}
/// @endcond


/**
* @}
*/

