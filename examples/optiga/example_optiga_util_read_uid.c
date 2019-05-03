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
* \file example_optiga_util_read_uid.c
*
* \brief   This file provides the example for reading uid from OPTIGA using
*          #optiga_util_read_data.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_util.h"

#ifdef OPTIGA_MINI_SHELL
#include "optiga/common/optiga_lib_logger.h"
#endif

extern void example_log_execution_status(const char_t* function, uint8_t status);
extern void example_log_function_name(const char_t* function);
/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}


/**
 * The below example demonstrates read data/metadata functionalities
 *
 * Example for #optiga_util_read_data and #optiga_util_read_metadata
 *
 */
void example_optiga_util_read_uid(void)
{
    uint16_t offset, bytes_to_read;
    uint16_t optiga_oid;
    uint8_t read_data_buffer[1024];
    optiga_lib_status_t return_status;
    optiga_util_t * me = NULL;
    uint8_t logging_status = 0;
    example_log_function_name(__FUNCTION__);

    do
    {
    	/**
		 * 1. Create OPTIGA Util Instance
		 */
		me = optiga_util_create(0, optiga_util_callback, NULL);
		if (NULL == me)
		{
			break;
		}

		//Read device end entity certificate from OPTIGA
		optiga_oid = 0xE0C2;
		offset = 0x00;
		bytes_to_read = sizeof(read_data_buffer);

		// OPTIGA Comms Shielded connection settings to enable the protection
		OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
		OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_RESPONSE_PROTECTION);

		/**
		 * 2. Read data from a data object (e.g. certificate data object)
		 *    using optiga_util_read_data.
		 */
		optiga_lib_status = OPTIGA_LIB_BUSY;
		return_status = optiga_util_read_data(me,
											  optiga_oid,
											  offset,
											  read_data_buffer,
											  &bytes_to_read);

		if (OPTIGA_LIB_SUCCESS != return_status)
		{
			break;
		}

		while (OPTIGA_LIB_BUSY == optiga_lib_status)
		{
			//Wait until the optiga_util_read_data operation is completed
		}

		if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
		{
			//Reading the data object failed.
			break;
		}

		logging_status = 1;

		#ifdef OPTIGA_MINI_SHELL
		        optiga_lib_print_string_with_newline("OPTIGA Trust Read Unique Identifier (UID):");
		        optiga_lib_print_hex_dump(read_data_buffer, bytes_to_read);
		        optiga_lib_print_string_with_newline("\n");

		        //optiga_lib_print_string_with_newline("Read device end entity certificate from OPTIGA:");
		        optiga_lib_print_string_with_newline("CIM Identifier:");
		        optiga_lib_print_hex_dump(&read_data_buffer[0], 1);

		        optiga_lib_print_string_with_newline("Platform Identifier:");
		        optiga_lib_print_hex_dump(&read_data_buffer[1], 1);

		        optiga_lib_print_string_with_newline("Model identifier:");
		        optiga_lib_print_hex_dump(&read_data_buffer[2], 1);

		        optiga_lib_print_string_with_newline("ID of ROM mask:");
		        optiga_lib_print_hex_dump(&read_data_buffer[3], 2);

		        optiga_lib_print_string_with_newline("Chip type:");
				optiga_lib_print_hex_dump(&read_data_buffer[5], 6);

				optiga_lib_print_string_with_newline("Batch number:");
				optiga_lib_print_hex_dump(&read_data_buffer[11], 6);

				optiga_lib_print_string_with_newline("Wafer Position:");
				optiga_lib_print_hex_dump(&read_data_buffer[17], 4);

				optiga_lib_print_string_with_newline("Firmware Identifier:");
				optiga_lib_print_hex_dump(&read_data_buffer[21], 4);

				optiga_lib_print_string_with_newline("ESW build number, BCD coded:");
				optiga_lib_print_hex_dump(&read_data_buffer[25], 2);
		#endif
    }while(0);

    if (me)
	{
		//Destroy the instance after the completion of usecase if not required.
		return_status = optiga_util_destroy(me);
	}
	example_log_execution_status(__FUNCTION__,logging_status);
}
