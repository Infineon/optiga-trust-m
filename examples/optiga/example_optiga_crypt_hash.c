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
* \file example_optiga_crypt_hash.c
*
* \brief    This file provides the example for hashing operations using 
*           #optiga_crypt_hash_start, #optiga_crypt_hash_update and 
*           #optiga_crypt_hash_finalize.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"

#ifdef OPTIGA_CRYPT_HASH_ENABLED

extern void example_log_execution_status(const char_t* function, uint8_t status);
extern void example_log_function_name(const char_t* function);

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

/**
 * Prepare the hash context
 */
#define OPTIGA_HASH_CONTEXT_INIT(hash_context,p_context_buffer,context_buffer_size,hash_type) \
{                                                               \
    hash_context.context_buffer = p_context_buffer;             \
    hash_context.context_buffer_length = context_buffer_size;   \
    hash_context.hash_algo = hash_type;                         \
}

/**
 * The below example demonstrates the generation of digest using
 * optiga_crypt_hash_xxxx operations.
 *
 * Example for #optiga_crypt_hash_start, #optiga_crypt_hash_update, 
 * #optiga_crypt_hash_finalize
 *
 */
void example_optiga_crypt_hash(void)
{
    optiga_lib_status_t return_status;

    uint8_t hash_context_buffer [130];
    optiga_hash_context_t hash_context;

    uint8_t data_to_hash [] = {"OPITGA, Infineon Technologies AG"};
    hash_data_from_host_t hash_data_host;

    uint8_t digest [32];

    optiga_crypt_t * me = NULL;
    uint8_t logging_status = 0;
    example_log_function_name(__FUNCTION__);

    do
    {
        /**
         * 1. Create OPTIGA Crypt Instance
         */
        me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. Initialize the Hash context
         */
        OPTIGA_HASH_CONTEXT_INIT(hash_context,hash_context_buffer,  \
                                 sizeof(hash_context_buffer),(uint8_t)OPTIGA_HASH_TYPE_SHA_256);

        optiga_lib_status = OPTIGA_LIB_BUSY;
        
        /**
         * 3. Initialize the hashing context at OPTIGA
         */
        return_status = optiga_crypt_hash_start(me, &hash_context);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) 
        {            
            //Wait until the optiga_crypt_hash_start operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            break;
        }

        /**
         * 4. Continue hashing the data
         */
        hash_data_host.buffer = data_to_hash;
        hash_data_host.length = sizeof(data_to_hash);

        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hash_update(me, 
                                                 &hash_context,
                                                 OPTIGA_CRYPT_HOST_DATA,
                                                 &hash_data_host);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) 
        {
            //Wait until the optiga_crypt_hash_update operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            break;
        }

        /**
         * 5. Finalize the hash
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hash_finalize(me,
                                                   &hash_context,
                                                   digest);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) 
        {
            //Wait until the optiga_crypt_hash_finalize operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            break;
        }
        logging_status = 1;

    } while (FALSE);

    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
    }
    example_log_execution_status(__FUNCTION__,logging_status);

}

#endif  //OPTIGA_CRYPT_HASH_ENABLED
/**
* @}
*/
