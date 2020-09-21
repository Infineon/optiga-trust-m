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
* \file example_optiga_crypt_hmac.c
*
* \brief   This file provides the example for HMAC generation  using
*          optiga_crypt_hmac_start, optiga_crypt_hmac_update and optiga_crypt_hmac_finalize APIs.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#if defined OPTIGA_CRYPT_HMAC_ENABLED

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;

static optiga_lib_status_t write_input_secret_to_oid(void);
static optiga_lib_status_t write_metadata(optiga_util_t * me);

//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_crypt_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

// Write metadata
static optiga_lib_status_t write_metadata(optiga_util_t * me)
{
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;
    const uint8_t input_secret_oid_metadata[] = {0x20, 0x06, 0xD3, 0x01, 0x00, 0xE8, 0x01, 0x21};
    do
    {
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me,
                                                   0xF1D0,
                                                   input_secret_oid_metadata,
                                                   sizeof(input_secret_oid_metadata));
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
    } while (FALSE);

    return(return_status);
}

// Write input secret to OID
static optiga_lib_status_t write_input_secret_to_oid()
{
    optiga_lib_status_t return_status = OPTIGA_UTIL_ERROR;
    optiga_util_t * me_util = NULL;
    const uint8_t input_secret[] = {0x8d,0xe4,0x3f,0xff,
                                    0x65,0x2d,0xa0,0xa7,
                                    0xf0,0x4e,0x8f,0x22,
                                    0x84,0xa4,0x28,0x3b};
    do
    {
        me_util = optiga_util_create(0, optiga_util_crypt_callback, NULL);
        if (NULL == me_util)
        {
            break;
        }
        /**
         * Precondition 1 :
         * Metadata for 0xF1D0 :
         * Execute access condition = Always
         * Data object type  =  Pre-shared secret
         */
        return_status = write_metadata(me_util);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }


        /**
        *  Precondition 2 :
        *  Write secret in OID 0xF1D0
        */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me_util,
                                               0xF1D0,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               input_secret,
                                               sizeof(input_secret));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
    } while (FALSE);
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
    return (return_status);
}

/**
 * The below example demonstrates HMAC-SHA256 generation using OPTIGA.
 *
 * Example for 
 *  #optiga_crypt_hmac_start,
 *  #optiga_crypt_hmac_update and 
 *  #optiga_crypt_hmac_finalize for HMAC-SHA256 generation
 */
void example_optiga_crypt_hmac(void)
{
    const uint8_t input_data_buffer_start[] = {0x6b, 0xc1, 0xbe, 0xe2,
                                               0x2e, 0x40, 0x9f, 0x96,
                                               0xe9, 0x3d, 0x7e, 0x11,
                                               0x73, 0x93, 0x17, 0x2a};
    const uint8_t input_data_buffer_update[] = {0x7c,0xd2,0xcf,0xf3,
                                                0x3f,0x51,0xa0,0xa7,
                                                0xf0,0x4e,0x8f,0x22,
                                                0x84,0xa4,0x28,0x3b};
    const uint8_t input_data_buffer_final[] = {0x5a,0xb0,0xad,0xd1,
                                               0x1d,0x3f,0x8e,0x85,
                                               0xd8,0x2c,0x6d,0xf1,
                                               0x62,0x82,0x06,0x19};
    uint8_t mac_buffer[32] = {0};
    uint32_t mac_buffer_length = sizeof(mac_buffer);
    uint32_t time_taken = 0;
    optiga_crypt_t * me_crypt = NULL;
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;

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
         * 1. Create OPTIGA Crypt Instance
         *
         */
        me_crypt = optiga_crypt_create(0, optiga_util_crypt_callback, NULL);
        if (NULL == me_crypt)
        {
            break;
        }

        /**
         * 2. Update input secret in 0xF1D0
         *
         */
        return_status = write_input_secret_to_oid();
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            // Update of secret failed
            break;
        }
        /**
         * 3. Start HMAC operation 
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        
        START_PERFORMANCE_MEASUREMENT(time_taken);
        
        return_status = optiga_crypt_hmac_start(me_crypt,
                                                OPTIGA_HMAC_SHA_256,
                                                0xF1D0,
                                                input_data_buffer_start,
                                                sizeof(input_data_buffer_start));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 4. Continue HMAC operation on input data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hmac_update(me_crypt,
                                                 input_data_buffer_update,
                                                 sizeof(input_data_buffer_update));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        /**
         * 5. End HMAC sequence and return the MAC generated
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hmac_finalize(me_crypt,
                                                   input_data_buffer_final,
                                                   sizeof(input_data_buffer_final),
                                                   mac_buffer,
                                                   &mac_buffer_length);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        READ_PERFORMANCE_MEASUREMENT(time_taken);
                                                   
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if (me_crypt)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me_crypt);
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
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);
    
}

#endif  //OPTIGA_CRYPT_HMAC_ENABLED
/**
* @}
*/
