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
* \file example_optiga_init_deinit.c
*
* \brief   This file provides the API for initialization/Deinitialization of optiga.
*
* \ingroup grOptigaExamples
*
* @{
*/
#include "optiga_example.h"
#include "optiga/optiga_util.h"

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
extern optiga_lib_status_t pair_host_and_optiga_using_pre_shared_secret(void);
#endif

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

optiga_util_t * me_util_instance = NULL;

void example_optiga_init(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
    static uint8_t host_optiga_pairing_completed = FALSE;
#endif
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do
    {
        if (NULL == me_util_instance)
        {
            //Create an instance of optiga_util to open the application on OPTIGA.
            me_util_instance = optiga_util_create(0, optiga_lib_callback, NULL);
            if (NULL == me_util_instance)
            {
                break;
            }
        }
        
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_open_application(me_util_instance, 0);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);         
        
#ifdef OPTIGA_COMMS_SHIELDED_CONNECTION
        if(FALSE == host_optiga_pairing_completed)
        {
            OPTIGA_EXAMPLE_LOG_MESSAGE("pair_host_and_optiga_using_pre_shared_secret");
            // Generate the pre-shared secret on host and write it to OPTIGA
            return_status = pair_host_and_optiga_using_pre_shared_secret();              
            if (OPTIGA_LIB_SUCCESS != return_status)
            {
                //pairing of host and optiga failed
                break;
            }
            host_optiga_pairing_completed = TRUE;
        }
#endif         
    }while(FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
}

void example_optiga_deinit(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
    
    do
    {
        /**
         * Close the application on OPTIGA after all the operations are executed
         * using optiga_util_close_application
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_close_application(me_util_instance, 0);
            
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        // destroy util and crypt instances
        //lint --e{534} suppress "Error handling is not required so return value is not checked"
        optiga_util_destroy(me_util_instance);
        me_util_instance = NULL;
    }while(FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
}

#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

/**
* @}
*/
