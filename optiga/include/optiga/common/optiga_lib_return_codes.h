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
* \file optiga_lib_return_codes.h
*
* \brief   This file defines the error codes for the all the layers and modules.<br>
* <b>OPTIGA host library return value Decoding</b><br>
* The definition of the Error code layering for a 2 byte error code explained below.<br>
* Each error code is represented as <b>[AB][CD]</b>, where each byte is represented as <b>[ ]</b> and individual alphabets represent a nibble.<br>
* <b>A  :</b> Error indication <b>Bit 15</b>. Set to 1 indicates error from OPTIGA security chip <br>
* <b>B  :</b> Layer information <b>Bits [14 - 8]</b>. This indicates the layer from which the error originated. E.g. OPTIGA, PAL, Comms, Util, Crypt etc.<br>
* <b>CD :</b> Return code <b>Bits [7 - 0]</b>. This indicates the actual nature of error.
*
* \ingroup  grOptigaLibCommon
*
* @{
*
*/


#ifndef _OPTIGA_LIB_RETURN_CODES_H_
#define _OPTIGA_LIB_RETURN_CODES_H_

#ifdef __cplusplus
extern "C" {
#endif

///OPTIGA host library API execution is successful
#define OPTIGA_LIB_SUCCESS                          (0x0000)
///OPTIGA host library in busy state
#define OPTIGA_LIB_BUSY                             (0x0001)

/**
 * OPTIGA device errors
 */
///OPTIGA Device Error to return OPTIGA security chip specific error codes (Refer Solution reference manual for details)
#define OPTIGA_DEVICE_ERROR                         (0x8000)

/**
 * OPTIGA comms module return values
 */
///OPTIGA comms API execution is successful
#define OPTIGA_COMMS_SUCCESS                        (0x0000)
///OPTIGA comms in busy state
#define OPTIGA_COMMS_BUSY                           (0x0001)
///OPTIGA comms API failed
#define OPTIGA_COMMS_ERROR                          (0x0102)
///OPTIGA comms API called with invalid inputs
#define OPTIGA_COMMS_ERROR_INVALID_INPUT            (0x0103)
///OPTIGA comms API called with insufficient memory buffer
#define OPTIGA_COMMS_ERROR_MEMORY_INSUFFICIENT      (0x0104)
/// Protocol stack memory insufficient
#define OPTIGA_COMMS_ERROR_STACK_MEMORY             (0x0105)
/// Protocol fatal error
#define OPTIGA_COMMS_ERROR_FATAL                    (0x0106)
/// Presentation layer handshake error
#define OPTIGA_COMMS_ERROR_HANDSHAKE                (0x0107)
/// Presentation layer session error
#define OPTIGA_COMMS_ERROR_SESSION                  (0x0108)

/**
 * OPTIGA command module return values
 */
///OPTIGA command API execution is successful
#define OPTIGA_CMD_SUCCESS                          (0x0000)
///OPTIGA command module in busy state
#define OPTIGA_CMD_BUSY                             (0x0001)
///OPTIGA command API failed
#define OPTIGA_CMD_ERROR                            (0x0202)
///OPTIGA command API called with invalid inputs
#define OPTIGA_CMD_ERROR_INVALID_INPUT              (0x0203)
///OPTIGA command API called with insufficient memory buffer
#define OPTIGA_CMD_ERROR_MEMORY_INSUFFICIENT        (0x0204)

/**
 * OPTIGA util module return values
 */
///OPTIGA util API execution is successful
#define OPTIGA_UTIL_SUCCESS                         (0x0000)
///OPTIGA util module in busy state
#define OPTIGA_UTIL_BUSY                            (0x0001)
///OPTIGA util API failed
#define OPTIGA_UTIL_ERROR                           (0x0302)
///OPTIGA util API called with invalid inputs
#define OPTIGA_UTIL_ERROR_INVALID_INPUT             (0x0303)
///OPTIGA util API called with insufficient memory buffer
#define OPTIGA_UTIL_ERROR_MEMORY_INSUFFICIENT       (0x0304)
///OPTIGA util API called when, a request of same instance is already in service
#define OPTIGA_UTIL_ERROR_INSTANCE_IN_USE           (0x0305)

/**
 * OPTIGA crypt module return values
 */
///OPTIGA crypt API execution is successful
#define OPTIGA_CRYPT_SUCCESS                        (0x0000)
///OPTIGA crypt module in busy state
#define OPTIGA_CRYPT_BUSY                           (0x0001)
///OPTIGA crypt API failed
#define OPTIGA_CRYPT_ERROR                          (0x0402)
///OPTIGA crypt API called with invalid inputs
#define OPTIGA_CRYPT_ERROR_INVALID_INPUT            (0x0403)
///OPTIGA crypt API called with insufficient memory buffer
#define OPTIGA_CRYPT_ERROR_MEMORY_INSUFFICIENT      (0x0404)
///OPTIGA crypt API called when, a request of same instance is already in service
#define OPTIGA_CRYPT_ERROR_INSTANCE_IN_USE          (0x0405)

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_LIB_RETURN_CODES_H_ */

/**
* @}
*/
