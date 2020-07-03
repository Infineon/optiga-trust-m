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
* \file optiga_lib_config.h
*
* \brief   This file is defines the compilation switches to build code with required features.
*
* \ingroup grOptigaLibCommon
*
* @{
*/


#ifndef _OPTIGA_LIB_CONFIG_H_
#define _OPTIGA_LIB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef OPTIGA_LIB_EXTERNAL
    #include "optiga_lib_config_external.h"
#else //Default Configuration
    // Default Configuration for OPTIGA Trust M V1
    #ifdef OPTIGA_TRUST_M_V1
        #include "optiga_lib_config_m_v1.h"
    #else // Default Configuration for OPTIGA Trust M V3
        #include "optiga_lib_config_m_v3.h"
    #endif
#endif // OPTIGA_LIB_EXTERNAL

#ifdef __cplusplus
}
#endif

#endif /* _OPTIGA_LIB_CONFIG_H_*/

/**
* @}
*/
