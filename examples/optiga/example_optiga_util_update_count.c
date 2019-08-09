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
* \file example_optiga_util_update_count.c
*
* \brief   This file provides the example to update the counter object value in OPTIGA using
*          #optiga_util_update_count.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga_example.h"

/**
 * Initialize the counter object with a threshold value 0x0A
 */
static uint8_t initial_counter_object_data [] =
{
    //Initial counter value
    0x00, 0x00, 0x00, 0x00,
    //Threshold value
    0x00, 0x00, 0x00, 0x0A,
};
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
 * The below example demonstrates update counter functionalities
 *
 * Example for #optiga_util_update_count
 *
 */
void example_optiga_util_update_count(void)
{
    uint16_t optiga_counter_oid;
    uint8_t offset;

    optiga_lib_status_t return_status = 0;
    optiga_util_t * me = NULL;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

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

        /**
         * Pre-condition
         * Any data object can be converted to counter data object by changing metadata as mentioned below:
         * - As precondition  write access should be always or LCSO < OPERATIONAL
         * - Write metadata as "0xD0, 0x01, 0x01" using #optiga_util_write_metadata
        */

        /**
         * Write default count and threshold value to counter data object (e.g. E120)
         * using optiga_util_write_data.
         *
         * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
         * in order to correctly update the used length of the object.
         */
        optiga_counter_oid = 0xE120;
        offset = 0x00;
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me,
                                               optiga_counter_oid,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               offset,
                                               initial_counter_object_data,
                                               sizeof(initial_counter_object_data));

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_write_data operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //writing data to a data object failed.
            return_status = optiga_lib_status;
            break;
        }

        // In this example, the counter is update by 5 and the final count would be 15
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_update_count(me,
                                                 optiga_counter_oid,
                                                 0x05);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_update_count operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Updating count of a counter data object failed.
            return_status = optiga_lib_status;
            break;
        }
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}

/**
* @}
*/
