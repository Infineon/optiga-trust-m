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
* \file example_optiga_crypt_rsa_encrypt_session.c
*
* \brief    This file provides the example for RSA Encryption using #optiga_crypt_rsa_encrypt_session.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

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
 * The below example demonstrates RSA encryption
 * #optiga_crypt_rsa_encrypt_session where session data is encrypted
 *
 */
void example_optiga_crypt_rsa_encrypt_session(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_key_id_t optiga_key_id;
    optiga_rsa_encryption_scheme_t encryption_scheme;

    uint8_t public_key [150];
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    uint16_t public_key_length = sizeof(public_key);
    public_key_from_host_t public_key_from_host;
    const uint8_t optional_data[2] = {0x01, 0x02};
    uint16_t optional_data_length = sizeof(optional_data);

    optiga_crypt_t * me = NULL;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do
    {
        /**
         * 1. Create OPTIGA Crypt Instance
         *
         */
        me = optiga_crypt_create(0, optiga_crypt_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
         * 2. Generate 1024 bit RSA Key pair
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_E0FC;
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_NO_PROTECTION);
        return_status = optiga_crypt_rsa_generate_keypair(me,
                                                          OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                                          (uint8_t)OPTIGA_KEY_USAGE_SIGN,
                                                          FALSE,
                                                          &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * 3. Generate 48 byte RSA Pre master secret in acquired session OID
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_generate_pre_master_secret(me,
                                                                    optional_data,
                                                                    optional_data_length,
                                                                    48);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        optiga_lib_status = OPTIGA_LIB_BUSY;

        /**
         * 4. Encrypt (RSA) the data in session OID
         */

        // OPTIGA Comms Shielded connection settings to enable the protection
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        public_key_from_host.public_key = public_key;
        public_key_from_host.length = public_key_length;
        public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;
        // Default protection level for this API is OPTIGA_COMMS_COMMAND_PROTECTION
        return_status = optiga_crypt_rsa_encrypt_session(me,
                                                         encryption_scheme,
                                                         NULL,
                                                         0,
                                                         OPTIGA_CRYPT_HOST_DATA,
                                                         &public_key_from_host,
                                                         encrypted_message,
                                                         &encrypted_message_length);

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
}

#endif  // OPTIGA_CRYPT_RSA_ENCRYPT_ENABLED

/**
* @}
*/
