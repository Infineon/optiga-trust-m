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
* \file example_optiga_crypt_symmetric_encrypt_decrypt_ecb.c
*
* \brief   This file provides the example for symmetric encryption and decryption for ECB mode using
*          #optiga_crypt_symmetric_encrypt_ecb and #optiga_crypt_symmetric_decrypt_ecb.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#if defined (OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED) && defined (OPTIGA_CRYPT_SYM_DECRYPT_ENABLED)

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

extern optiga_lib_status_t generate_symmetric_key(void);

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
//lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_crypt_callback(void * context, optiga_lib_status_t return_status)
{
    optiga_lib_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

/**
 * The below example demonstrates the symmetric encryption and decryption for ECB mode using OPTIGA.
 *
 * Example for #optiga_crypt_symmetric_encrypt_ecb and #optiga_crypt_symmetric_decrypt_ecb
 *
 */
void example_optiga_crypt_symmetric_encrypt_decrypt_ecb(void)
{
    const uint8_t plain_data_buffer[] = {0x6b,0xc1,0xbe,0xe2,
                                         0x2e,0x40,0x9f,0x96,
                                         0xe9,0x3d,0x7e,0x11,
                                         0x73,0x93,0x17,0x2a};
    uint32_t plain_data_length = sizeof(plain_data_buffer);
    uint8_t encrypted_data_buffer[32] = {0};
    uint32_t encrypted_data_length = sizeof(encrypted_data_buffer);
    uint8_t decrypted_data_buffer[32] = {0};
    uint32_t decrypted_data_length = sizeof(decrypted_data_buffer);
    optiga_crypt_t * me = NULL;
    uint32_t time_taken = 0;
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
        me = optiga_crypt_create(0, optiga_util_crypt_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. Update AES 128 symmetric key using secure key update
         *
         */
        OPTIGA_EXAMPLE_LOG_MESSAGE("Symmetric key generation");
        return_status = generate_symmetric_key();
        if(OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        
        /**
         * 3. Encrypt the plain data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        
        START_PERFORMANCE_MEASUREMENT(time_taken);
        
        return_status = optiga_crypt_symmetric_encrypt_ecb(me,
                                                           OPTIGA_KEY_ID_SECRET_BASED,
                                                           plain_data_buffer,
                                                           plain_data_length,
                                                           encrypted_data_buffer,
                                                           &encrypted_data_length);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        if (encrypted_data_length != plain_data_length)
        {
            //Encrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }

        /**
         * 4. Decrypt the encrypted data from step 3
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_symmetric_decrypt_ecb(me,
                                                           OPTIGA_KEY_ID_SECRET_BASED,
                                                           encrypted_data_buffer,
                                                           encrypted_data_length,
                                                           decrypted_data_buffer,
                                                           &decrypted_data_length);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        READ_PERFORMANCE_MEASUREMENT(time_taken);
        
        // Compare the decrypted data with plain data
        if( OPTIGA_LIB_SUCCESS != memcmp(decrypted_data_buffer, plain_data_buffer, plain_data_length))
        {
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        if (decrypted_data_length != plain_data_length)
        {
            //Decrypted data length is incorrect
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        return_status = OPTIGA_LIB_SUCCESS;

    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);
    
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
}

#endif  //OPTIGA_CRYPT_SYM_ENCRYPT_ENABLED && OPTIGA_CRYPT_SYM_DECRYPT_ENABLED
/**
* @}
*/
