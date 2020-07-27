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
 * \file example_optiga_util_write_data.c
 *
 * \brief   This file provides the example for writing data/metadata to OPTIGA using
 *          #optiga_util_write_data and #optiga_util_write_metadata.
 *
 * \ingroup grOptigaExamples
 *
 * @{
 */

#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_timer.h"
#include "optiga_example.h"

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
 * The below example demonstrates hibernate and restore functionalities
 *
 * Example for #optiga_util_open_application and #optiga_util_close_application
 *
 */
void example_optiga_util_hibernate_restore(void)
{
    optiga_lib_status_t return_status;
    optiga_util_t * me = NULL;
    uint8_t logging_status = 0;
    uint8_t security_event_counter = 0;
    uint16_t bytes_to_read = 1;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
    
    do
    {
        //Create an instance of optiga_util to open the application on OPTIGA.
        me = optiga_util_create(0, optiga_util_callback, NULL);
    
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_open_application(me, 0);
    
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        while (optiga_lib_status == OPTIGA_LIB_BUSY)
        {
            //Wait until the optiga_util_open_application is completed
        }
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga_util_open_application failed
            break;
        }

                /**
         * To perform the hibernate, Security Event Counter(SEC) must be 0.
                 * Read SEC data object (0xE0C5) and wait until SEC = 0
         */
        do
        {
            optiga_lib_status = OPTIGA_LIB_BUSY;
            return_status = optiga_util_read_data(me,
                                                  0xE0C5,
                                                  0x00,
                                                  &security_event_counter,
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
            pal_os_timer_delay_in_milliseconds(1000);
        } while (0 != security_event_counter);

        /**
         * Hibernate the application on OPTIGA
         * using optiga_util_close_application with perform_hibernate parameter as true
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me, 1);
    
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
    
        while (optiga_lib_status == OPTIGA_LIB_BUSY)
        {
            //Wait until the optiga_util_close_application is completed
        }
    
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga_util_close_application failed
            break;
        }
    
        /**
         * Restore the application on OPTIGA
         * using optiga_util_open_application with perform_restore parameter as true
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_open_application(me, 1);
    
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_open_application is completed
        }
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga_util_open_application failed
            break;
        }
    
        /**
         * Close the application on OPTIGA without hibernating
         * using optiga_util_close_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me, 0);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
    
        while (optiga_lib_status == OPTIGA_LIB_BUSY)
        {
            //Wait until the optiga_util_close_application is completed
        }
    
        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //optiga_util_close_application failed
            break;
        }
        logging_status = 1;
    
    } while (FALSE);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me);
    }
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
}

/**
 * @}
 */
