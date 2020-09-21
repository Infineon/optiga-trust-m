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
* \file example_read_coprocessor_id.c
*
* \brief   This file provides an example for reading the Coprocessor ID and displaying it's fields.
*
* \ingroup grUseCases
*
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga_example.h"
#include "optiga/pal/pal_logger.h"
#include "optiga/pal/pal_os_memory.h"

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

extern pal_logger_t logger_console;

/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_lib_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

#if defined (OPTIGA_LIB_ENABLE_EXAMPLE_LOGGING)
/*Convert Byte to HexString */
#define OPTIGA_LOGGER_CONVERT_BYTE_TO_HEX(hex_byte, p_hex_string, index) \
{ \
    uint8_t nibble;                       \
    nibble = (hex_byte & 0xF0)>>4;         \
    p_hex_string [index++] = ((nibble > 0x09)) ? (nibble + 0x37) : (nibble + 0x30); \
    nibble = hex_byte & 0x0F;         \
    p_hex_string [index++] = ((nibble > 0x09)) ? (nibble + 0x37) : (nibble + 0x30); \
}

/* Converts the uint8 array to hex string format */
static void optiga_lib_byte_to_hex_string(const uint8_t * p_array_buffer,
                                             uint8_t * p_hex_string,
                                             uint32_t length,
                                             bool_t is_input_byte_array)
{
    uint32_t loop = 0;
    uint8_t hex_byte = 0, index = 0;

    do
    {
        if ((NULL == p_array_buffer) || (NULL == p_hex_string))
        {
            return;
        }

        for (loop = 0; loop < length; loop++)
        {
            index = 0;
            hex_byte = p_array_buffer[loop];
            if (TRUE == is_input_byte_array)
            {
                p_hex_string [index++] = '0';
                p_hex_string [index++] = 'x';
            }

            /*Convert Byte to HexString */
            OPTIGA_LOGGER_CONVERT_BYTE_TO_HEX(hex_byte,p_hex_string, index);
            
            p_hex_string [index++] = ' ';
            p_hex_string += index;
        }
        *p_hex_string = 0x00;
    } while(0);
}

static void optiga_lib_print_coprocessor_data(const uint8_t * p_log_string,
                                              uint16_t length,
                                              const char_t * p_log_color)
{
    uint8_t temp_buffer[100];
    char_t output_buffer[100];
    uint16_t index;
    uint16_t temp_length;
    uint8_t new_line_characters[2] = {0x0D, 0x0A};
    uint8_t buffer_window = 32; // Alignment of 16 bytes per line
    
    //Logging the arrays in chunks of 16 bytes through chaining
    for (index = 0; index < length; index+=buffer_window)
    {
        temp_length = buffer_window;
        if ((length - index) < buffer_window)
        {
            temp_length =  length - index;
        }

        pal_os_memset(temp_buffer, 0x00, sizeof(temp_buffer));
        pal_os_memset(output_buffer, 0x00, sizeof(output_buffer));

        optiga_lib_byte_to_hex_string((uint8_t*)(p_log_string + index), temp_buffer, temp_length, FALSE);

        sprintf((char_t *)output_buffer, "%s%s%s", p_log_color, temp_buffer, OPTIGA_LIB_LOGGER_COLOR_DEFAULT);

        // New line characted entered at the end of each segment
        output_buffer[strlen(output_buffer)] = (char_t)new_line_characters[0];
        output_buffer[strlen(output_buffer)+1] = (char_t)new_line_characters[1];
        //lint --e{534} The return value is not used hence not checked*/
        pal_logger_write(&logger_console, (const uint8_t *)output_buffer, strlen(output_buffer) + 2);
    }
}

static void optiga_lib_print_coprocessor_components(const char_t * p_log_string,
                                                    const uint8_t * p_log_array,
                                                    uint16_t length)
{
    char_t color_buffer[100];

    OPTIGA_LIB_LOGGER_PRINT_INFO(color_buffer, p_log_string, OPTIGA_EXAMPLE, OPTIGA_EXAMPLE_COLOR);
    //lint --e{534} The return value is not used hence not checked*/
    pal_logger_write(&logger_console, (const uint8_t *)color_buffer, strlen(color_buffer));
    optiga_lib_print_coprocessor_data(p_log_array, length, OPTIGA_EXAMPLE_COLOR);
}

#define OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO(msg, array, length) \
                                               optiga_lib_print_coprocessor_components(msg, array, length)
#else
#define OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO(msg, array, length)
#endif

/**
 * The below example demonstrates reading of Coprocessor ID and displaying it's individual components.
 *
 * Preconditions: The optiga_util_open_application must be executed before invoking the below example.
 *
 */
void example_read_coprocessor_id(void)
{
    uint16_t bytes_to_read;
    uint8_t coprocessor_uid[32];
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_util_t * me_util = NULL;

    

    do
    {
        
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        example_optiga_init();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
        
        OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
        /**
         * 1. Create OPTIGA Util Instance
         */
        me_util = optiga_util_create(0, optiga_lib_callback, NULL);
        if (NULL == me_util)
        {
            break;
        }

        /**
         * 2. Read Coprocessor UID (0xE0C2) data object from OPTIGA
         *    using optiga_util_read_data.
         */
        bytes_to_read = sizeof(coprocessor_uid);
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_data(me_util,
                                              0xE0C2,
                                              0x0000,
                                              coprocessor_uid,
                                              &bytes_to_read);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        return_status = OPTIGA_LIB_SUCCESS;
        OPTIGA_EXAMPLE_LOG_MESSAGE("Coprocessor UID components are mentioned below:\n");
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("CIM Identifier                       : ", &coprocessor_uid[0], 0x01);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Platform Identifier                  : ", &coprocessor_uid[1], 0x01);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Model Identifier                     : ", &coprocessor_uid[2], 0x01);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("ROM mask ID                          : ", &coprocessor_uid[3], 0x02);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Chip type                            : ", &coprocessor_uid[5], 0x06);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Batch number                         : ", &coprocessor_uid[11], 0x06);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Chip position on wafer: X-coordinate : ", &coprocessor_uid[17], 0x02);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Chip position on wafer: Y-coordinate : ", &coprocessor_uid[19], 0x02);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("Firmware Identifier                  : ", &coprocessor_uid[21], 0x04);
        OPTIGA_EXAMPLE_LOG_COPROCESSOR_ID_INFO("ESW build number, BCD coded          : ", &coprocessor_uid[25], 0x02);
        optiga_lib_print_string_with_newline("");
    } while(FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if(me_util)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me_util);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
}

/**
* @}
*/
