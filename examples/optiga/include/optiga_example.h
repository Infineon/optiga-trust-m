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
* \file optiga_example.h
*
* \brief   This file defines APIs, types and data structures used in the OPTIGA example.
*
* \ingroup  grOptigaExamples
*
* @{
*/


#ifndef _OPTIGA_EXAMPLE_H_
#define _OPTIGA_EXAMPLE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "optiga_lib_config.h"
#include "optiga_lib_logger.h"

extern char performance_buffer_string[30];
extern void example_performance_measurement(uint32_t* time_value, uint8_t time_reset_flag);

#define START_TIMER                 (TRUE)

#define STOPTIMER_AND_CALCULATE     (FALSE)

// Macro to enable logger for Application
#define OPTIGA_LIB_ENABLE_EXAMPLE_LOGGING

// Logger levels
#define OPTIGA_EXAMPLE                     "[optiga example]  : "

// Logger color for different layers and data
#define OPTIGA_EXAMPLE_COLOR               OPTIGA_LIB_LOGGER_COLOR_DEFAULT

//Start timer for performance measurement
#define START_PERFORMANCE_MEASUREMENT(time_taken) example_performance_measurement(&time_taken, START_TIMER)

//Stop timer and calculate performance measurement
#define READ_PERFORMANCE_MEASUREMENT(time_taken) example_performance_measurement(&time_taken, STOPTIMER_AND_CALCULATE)

// Check return status
#define WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status)\
                            if (OPTIGA_LIB_SUCCESS != return_status)\
                            {\
                                break;\
                            }\
                            while (OPTIGA_LIB_BUSY == optiga_lib_status)\
                            { }\
                            if (OPTIGA_LIB_SUCCESS != optiga_lib_status)\
                            {\
                                return_status = optiga_lib_status;\
                                break;\
                            }\
                            
#if defined (OPTIGA_LIB_ENABLE_EXAMPLE_LOGGING)
/**
 * \brief Logs the message provided from Application layer
 *
 * \details
 * Logs the message provided from Application layer
 *
 * \pre
 *
 * \note
 * - None
 *
 * \param[in]      msg      Valid pointer to string to be logged
 *
 */
#define OPTIGA_EXAMPLE_LOG_MESSAGE(msg) \
{\
    optiga_lib_print_message(msg,OPTIGA_EXAMPLE,OPTIGA_EXAMPLE_COLOR);\
}

/**
 * \brief Logs the byte array buffer provided from Application layer in hexadecimal format
 *
 * \details
 * Logs the byte array buffer provided from Application layer in hexadecimal format
 *
 * \pre
 *
 * \note
 * - None
 *
 * \param[in]      array      Valid pointer to array to be logged
 * \param[in]      array_len  Length of array buffer
 *
 */
#define OPTIGA_EXAMPLE_LOG_HEX_DATA(array,array_len) \
{\
    optiga_lib_print_array_hex_format(array,array_len,OPTIGA_UNPROTECTED_DATA_COLOR);\
}

/**
 * \brief Logs the status info provided from Application layer
 *
 * \details
 * Logs the status info provided from Application layer
 *
 * \pre
 *
 * \note
 * - None
 *
 * \param[in]      return_value      Status information Application layer
 *
 */
#define OPTIGA_EXAMPLE_LOG_STATUS(return_value) \
{ \
    if (OPTIGA_LIB_SUCCESS != return_value) \
    { \
        optiga_lib_print_status(OPTIGA_EXAMPLE,OPTIGA_ERROR_COLOR,return_value); \
    } \
    else\
    { \
        optiga_lib_print_status(OPTIGA_EXAMPLE,OPTIGA_EXAMPLE_COLOR,return_value); \
    } \
}
#else

#define OPTIGA_EXAMPLE_LOG_MESSAGE(msg)
#define OPTIGA_EXAMPLE_LOG_HEX_DATA(array, array_len)
#define OPTIGA_EXAMPLE_LOG_STATUS(return_value)

#endif

#define OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken,return_value) \
{   \
    if(OPTIGA_LIB_SUCCESS == return_value)  \
    {   \
        sprintf(performance_buffer_string, "Example takes %d msec", (int)time_taken);    \
        OPTIGA_EXAMPLE_LOG_MESSAGE(performance_buffer_string);  \
    }   \
}

#define OPTIGA_EXAMPLE_PROTECTED_UPDATE_PERFORMANCE_VALUE(time_taken) \
{   \
        sprintf(performance_buffer_string, "Execution time %d msec", (int)time_taken);    \
        OPTIGA_EXAMPLE_LOG_MESSAGE(performance_buffer_string);  \
}
#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_EXAMPLE_H_ */

/**
* @}
*/
