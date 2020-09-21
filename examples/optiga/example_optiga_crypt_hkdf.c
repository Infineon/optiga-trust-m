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
* \file example_optiga_crypt_hkdf.c
*
* \brief   This file provides the example for key derivation (HKDF-SHA256) using
*          #optiga_crypt_hkdf.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_HKDF_ENABLED

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
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
 * Sample metadata
 */
static const uint8_t metadata [] = {
    //Metadata tag in the data object
    0x20, 0x06,
        //Data object type set to PRESSEC
        0xE8, 0x01, 0x21,
        0xD3, 0x01, 0x00,
};

/**
 * Default metadata
 */
static const uint8_t default_metadata [] = {
    //Metadata tag in the data object
    0x20, 0x06,
        //Data object type set to BSTR
        0xE8, 0x01, 0x00,
        0xD3, 0x01, 0xFF,
};

static const uint8_t info [] = {0x8D,0xE4,0x3F,0xFF,
                          0x65,0x2D,0xA0,0xA7,
                          0xF0,0x4E,0x8F,0x22,
                          0x84,0xA4,0x28,0x3B};

static const uint8_t salt [] = {
    0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A,
    0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82,
    0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F,
    0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
};

// Secret to be written to data object which will be
// later used as part of TLS PRF SHA256 key derivation
static const uint8_t secret_to_be_written [] = {
    0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 0x61, 0xC7,
    0x04, 0xE0, 0x0D, 0x82, 0x8B, 0x7A, 0x36, 0x41,
    0xD5, 0xCD, 0x7A, 0x38, 0x46, 0xDE, 0xF9, 0x0F,
    0x21, 0x42, 0x40, 0x25, 0x0A, 0xAF, 0x9C, 0x2E,
};

/**
 * The below example demonstrates the key derivation using #optiga_crypt_hkdf.
 *
 */
void example_optiga_crypt_hkdf(void)
{
    uint8_t decryption_key [16] = {0};

    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    uint32_t time_taken = 0;
    optiga_crypt_t * me = NULL;
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
        
        me_util = optiga_util_create(0, optiga_lib_callback, NULL);
        if (NULL == me_util)
        {
            break;
        }

        /**
         * 1. Write the shared secret to the Arbitrary data object F1D0
         *       - This is typically a one time activity and
         *       - use the this OID as input secret to derive keys further
         * 2. Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
         *    to clear the remaining data in the object
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me_util,
                                               0xF1D0,
                                               OPTIGA_UTIL_ERASE_AND_WRITE ,
                                               0x00,
                                               secret_to_be_written,
                                               sizeof(secret_to_be_written));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 2. Change data object type to PRESSEC
         *
         */

        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   0xF1D0,
                                                   metadata,
                                                   sizeof(metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 3. Create OPTIGA Crypt Instance
         *
         */
        me = optiga_crypt_create(0, optiga_lib_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 4. Derive key (e.g. decryption key) using optiga_crypt_hkdf with protected I2C communication.
         *       - Use shared secret from F1D0 data object
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_COMMAND_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        START_PERFORMANCE_MEASUREMENT(time_taken);
        
        return_status = optiga_crypt_hkdf(me,
                                          OPTIGA_HKDF_SHA_256,
                                          0xF1D0, /* Input secret OID */
                                          salt,
                                          sizeof(salt),
                                          info,
                                          sizeof(info),
                                          sizeof(decryption_key),
                                          TRUE,
                                          decryption_key);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        READ_PERFORMANCE_MEASUREMENT(time_taken);
        
        /**
         * 5. Change meta data to default value
         *
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   0xF1D0,
                                                   default_metadata,
                                                   sizeof(default_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_crypt_destroy(me);
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }

    if (me_util)
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
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);
    
}

#endif //OPTIGA_CRYPT_HKDF_ENABLED
/**
* @}
*/
