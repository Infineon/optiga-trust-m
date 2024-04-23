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
* \file example_optiga_crypt_clear_auto_state.c
*
* \brief   This file provides an example for clear auto state.
*
* \ingroup grUseCases
*
* @{
*/

#include "optiga_util.h"
#include "optiga_crypt.h"

#include "pal_os_memory.h"
#include "pal_crypt.h"
#include "optiga_example.h"
#include "mbedtls/ccm.h"
#include "mbedtls/md.h"
#include "mbedtls/ssl.h"
#if defined (OPTIGA_CRYPT_GENERATE_AUTH_CODE_ENABLED) && defined (OPTIGA_CRYPT_HMAC_VERIFY_ENABLED) && defined (OPTIGA_CRYPT_CLEAR_AUTO_STATE_ENABLED)

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

//lint --e{526} suppress "CalcHMAC" it's define in other file"
extern pal_status_t CalcHMAC(const uint8_t * secret_key,
                             uint16_t secret_key_len,
                             const uint8_t * input_data,
                             uint32_t input_data_length,
                             uint8_t * hmac);
/**
 * Metadata for Secret OID :
 * Execute access condition = Always
 * Data object type  =  Auto Ref
 */
static const uint8_t secret_oid_metadata[] = 
{
    0x20, 0x06,
          0xD3, 0x01, 0x00,
          0xE8, 0x01, 0x31
};

/**
 * Shared secret data
 */
static const uint8_t user_secret[] = 
{
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6
};

/**
 * Optional data
 */
static const uint8_t optional_data[] = 
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 
};

/**
 * random data
 */
static uint8_t random_data[32] = {0x00};

/**
 * Input data
 */
static uint8_t input_data_buffer[100] = {0x00};
/**
 * Arbitrary data
 */
static uint8_t arbitrary_data[16] = {0x00};

/**
 * Generated hmac
 */
static uint8_t hmac_buffer[32] = {0x00};
static const uint32_t hmac_length = sizeof(hmac_buffer);
/**
 * Callback when optiga_util_xxxx/optiga_crypt_xxxx operation is completed asynchronously
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

/**
 * The below example demonstrates of clear auto state functionality.
 */
void example_optiga_crypt_clear_auto_state(void)
{
    optiga_lib_status_t return_status = OPTIGA_CRYPT_ERROR;
    pal_status_t pal_return_status;
    optiga_util_t * me_util = NULL;
    optiga_crypt_t * me_crypt = NULL;
    uint32_t time_taken = 0;

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
         * 1. Create OPTIGA crypt and util Instances
         */
        me_crypt = optiga_crypt_create(0, optiga_lib_callback, NULL);
        if (NULL == me_crypt)
        {
            break;
        }
        
        me_util = optiga_util_create(0, optiga_lib_callback, NULL);
        if (NULL == me_util)
        {
            break;
        }
        /**
         * 2. Initialize the protection level and protocol version for the instances
         */
        OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(me_util,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(me_util,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me_crypt,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me_crypt,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
        
        /**
         * 3. Set the metadata of secret OID(0xF1D0) using optiga_util_write_metadata.
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   0xF1D0,
                                                   secret_oid_metadata,
                                                   sizeof(secret_oid_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
        *  4. Write shared secret in OID 0xF1D0
        */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me_util,
                                               0xF1D0,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               user_secret,
                                               sizeof(user_secret));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * 5. Call generate auth code with optional data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_generate_auth_code(me_crypt,
                                                        OPTIGA_RNG_TYPE_TRNG,
                                                        optional_data,
                                                        sizeof(optional_data),
                                                        random_data,
                                                        32);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        /**
         * 6. Calculate HMAC on host
         */
        pal_os_memset(arbitrary_data, 0x5A, sizeof(arbitrary_data));
        pal_os_memcpy(input_data_buffer, optional_data, sizeof(optional_data));
        pal_os_memcpy(&input_data_buffer[sizeof(optional_data)], random_data, sizeof(random_data));
        pal_os_memcpy(&input_data_buffer[sizeof(optional_data) + sizeof(random_data)], arbitrary_data, sizeof(arbitrary_data));
        
        pal_return_status = CalcHMAC(user_secret,
                                     sizeof(user_secret),
                                     input_data_buffer,
                                     sizeof(input_data_buffer),
                                     hmac_buffer);

        if (PAL_STATUS_SUCCESS != pal_return_status)
        {
            // HMAC calculation on host failed
            return_status = pal_return_status;
            break;
        }
        /**
         * 7. Perform HMAC verification using OPTIGA
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hmac_verify(me_crypt,
                                                 OPTIGA_HMAC_SHA_256,
                                                 0xF1D0,
                                                 input_data_buffer,
                                                 sizeof(input_data_buffer),
                                                 hmac_buffer,
                                                 hmac_length);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        /**
         * 8. Perform clear auto state using OPTIGA
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        
        START_PERFORMANCE_MEASUREMENT(time_taken);
        
        return_status = optiga_crypt_clear_auto_state(me_crypt,
                                                      0xF1D0);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        READ_PERFORMANCE_MEASUREMENT(time_taken);
        
        return_status = OPTIGA_LIB_SUCCESS;

    } while(FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);
    
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
    if(me_crypt)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me_crypt);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
    
}
#endif //(OPTIGA_CRYPT_GENERATE_AUTH_CODE_ENABLED) && (OPTIGA_CRYPT_HMAC_VERIFY_ENABLED) && (OPTIGA_CRYPT_CLEAR_AUTO_STATE_ENABLED)
/**
* @}
*/
