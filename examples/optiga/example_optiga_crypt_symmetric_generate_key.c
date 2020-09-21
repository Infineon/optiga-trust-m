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
* \file example_optiga_crypt_symmetric_generate_key.c
*
* \brief   This file provides the example for generation of symmetric key using
*          optiga_crypt_symmetric_generate_key.
*
* \ingroup grOptigaExamples
*
* @{
*/
#include "optiga/optiga_util.h"
#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

uint32_t time_taken_to_generate_key = 0;

#define METADATA_TAG_KEY_ALGO_ID     (0xE0)
extern optiga_lib_status_t example_check_tag_in_metadata(const uint8_t * buffer, 
                                                         const uint8_t tag);

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

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
 * Sample metadata of 0xE200 
 */
const uint8_t E200_metadata[] = { 0x20, 0x06, 0xD0, 0x01, 0x00, 0xD3, 0x01, 0x00 };

/**
 * The below generates symmetric Key using optiga_crypt_symmetric_generate_key.
 *
 */
optiga_lib_status_t generate_symmetric_key(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t symmetric_key;
    uint16_t optiga_oid, bytes_to_read;
    uint8_t read_data_buffer[100];

    optiga_crypt_t * crypt_me = NULL;
    optiga_util_t * util_me = NULL;    

    do
    {
       /**
         * 1. Create OPTIGA Crypt Instance
         */
        crypt_me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == crypt_me)
        {
            break;
        }

        util_me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == util_me)
        {
            break;
        }
        
        /**
         * Read metadata of a data object (e.g. key data object 0xE200)
         * using optiga_util_read_metadata.
         */
        optiga_oid = 0xE200;
        bytes_to_read = sizeof(read_data_buffer);
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_metadata(util_me,
                                                  optiga_oid,
                                                  read_data_buffer,
                                                  &bytes_to_read);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        return_status = example_check_tag_in_metadata(read_data_buffer,
                                                      METADATA_TAG_KEY_ALGO_ID);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            return_status = OPTIGA_LIB_SUCCESS;
            break;
        }
        
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_oid = 0xE200;
        return_status = optiga_util_write_metadata(util_me,
                                                   optiga_oid,
                                                   E200_metadata,
                                                   sizeof(E200_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * 2. Generate symmetric key
         *       - Use AES-128 key type
         *       - OPTIGA_KEY_USAGE_ENCRYPTION as a Key Usage
         *       - Store the Symmetric key in OPTIGA Key store OID(E200)
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        symmetric_key = OPTIGA_KEY_ID_SECRET_BASED;
        
        START_PERFORMANCE_MEASUREMENT(time_taken_to_generate_key);
        
        return_status = optiga_crypt_symmetric_generate_key(crypt_me,
                                                            OPTIGA_SYMMETRIC_AES_128,
                                                            (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
                                                            FALSE,
                                                            &symmetric_key);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        READ_PERFORMANCE_MEASUREMENT(time_taken_to_generate_key);
        
        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);

    if (crypt_me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(crypt_me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    if (util_me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(util_me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    return return_status;
}

/**
 * The below example demonstrates the generation of
 * symmetric Key using optiga_crypt_symmetric_generate_key.
 *
 */
void example_optiga_crypt_symmetric_generate_key(void)
{
    optiga_lib_status_t return_status;
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        example_optiga_init();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
        
    return_status = generate_symmetric_key();
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken_to_generate_key, return_status);
}

#endif  //OPTIGA_CRYPT_SYM_GENERATE_KEY_ENABLED
/**
* @}
*/
