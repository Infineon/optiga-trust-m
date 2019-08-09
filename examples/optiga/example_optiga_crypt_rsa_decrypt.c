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
* \file example_optiga_crypt_rsa_decrypt.c
*
* \brief   This file provides the example for RSA decrypt functionality
*          example_optiga_crypt_rsa_decrypt_and_export and example_optiga_crypt_rsa_decrypt_and_store.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga_example.h"

#ifdef OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

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
 * The below example demonstrates RSA decryption
 * #optiga_crypt_rsa_decrypt_and_export where message is provided by user
 *
 */
void example_optiga_crypt_rsa_decrypt_and_export(void)
{
    optiga_lib_status_t return_status = 0;
    optiga_key_id_t optiga_key_id;
    optiga_rsa_encryption_scheme_t encryption_scheme;
    public_key_from_host_t public_key_from_host;
    uint8_t encrypted_message[128];
    uint8_t message[] = {"RSA PKCS1_v1.5 Encryption of user message"};
    uint16_t message_length = sizeof(message);
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    uint8_t public_key [150];
    uint8_t decrypted_message[150];
    uint16_t decrypted_message_length = sizeof(decrypted_message);
    uint16_t public_key_length = sizeof(public_key);

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
         * 2. Generate RSA Key pair
         *       - Use 1024 or 2048 bit RSA key
         *       - Specify the Key Usage
         *       - Store the Private key in OPTIGA Key store
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_E0FC;
        return_status = optiga_crypt_rsa_generate_keypair(me,
                                                          OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                                          (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
                                                          FALSE,
                                                          &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_generate_keypair operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Key pair generation failed
            return_status = optiga_lib_status;
            break;
        }
        /**
         * 3. RSA encryption
         */
        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        public_key_from_host.public_key = public_key;
        public_key_from_host.length = public_key_length;
        public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;
        optiga_lib_status = OPTIGA_LIB_BUSY;

        return_status = optiga_crypt_rsa_encrypt_message(me,
                                                         encryption_scheme,
                                                         message,
                                                         message_length,
                                                         NULL,
                                                         0,
                                                         OPTIGA_CRYPT_HOST_DATA,
                                                         &public_key_from_host,
                                                         encrypted_message,
                                                         &encrypted_message_length);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_encrypt_message operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Encryption failed
            return_status = optiga_lib_status;
            break;
        }
        /**
         * 4. RSA decryption
         */
        // OPTIGA Comms Shielded connection settings to enable the protection
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_RESPONSE_PROTECTION);

        optiga_lib_status = OPTIGA_LIB_BUSY;
        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        return_status = optiga_crypt_rsa_decrypt_and_export(me,
                                                            encryption_scheme,
                                                            encrypted_message,
                                                            sizeof(encrypted_message),
                                                            NULL,
                                                            0,
                                                            optiga_key_id,
                                                            decrypted_message,
                                                            &decrypted_message_length);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_decrypt_and_export operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //RSA Decryption failed
            return_status = optiga_lib_status;
            break;
        }
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

/**
 * The below example demonstrates RSA decryption
 * #optiga_crypt_rsa_decrypt_and_store where decrypted message get stored in session context
 *
 */
void example_optiga_crypt_rsa_decrypt_and_store(void)
{
    optiga_lib_status_t return_status = 0;
    optiga_key_id_t optiga_key_id;
    optiga_rsa_encryption_scheme_t encryption_scheme;
    public_key_from_host_t public_key_from_host;
    uint8_t encrypted_message[128];
    uint16_t encrypted_message_length = sizeof(encrypted_message);
    uint8_t public_key [150];
    uint16_t public_key_length = sizeof(public_key);
    const uint8_t optional_data[] = {0x01, 0x02};

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
         * 2. Generate RSA Key pair
         *       - Use 1024 or 2048 bit RSA key
         *       - Specify the Key Usage
         *       - Store the Private key in OPTIGA Key store
         *       - Export Public Key
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        optiga_key_id = OPTIGA_KEY_ID_E0FC;
        return_status = optiga_crypt_rsa_generate_keypair(me,
                                                          OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL,
                                                          (uint8_t)OPTIGA_KEY_USAGE_ENCRYPTION,
                                                          FALSE,
                                                          &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_generate_keypair operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Key pair generation failed
            return_status = optiga_lib_status;
            break;
        }
        /**
         * 3. Generate 0x46 byte RSA Pre master secret which is stored in acquired session OID
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_rsa_generate_pre_master_secret(me,
                                                                    optional_data,
                                                                    sizeof(optional_data),
                                                                    30);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_random operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            return_status = optiga_lib_status;
            break;
        }

        optiga_lib_status = OPTIGA_LIB_BUSY;

        /**
         * 4. Encrypt(RSA) the data stored in session OID
         */

        // OPTIGA Comms Shielded connection settings to enable the protection
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_COMMAND_PROTECTION);

        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        public_key_from_host.public_key = public_key;
        public_key_from_host.length = public_key_length;
        public_key_from_host.key_type = (uint8_t)OPTIGA_RSA_KEY_1024_BIT_EXPONENTIAL;

        return_status = optiga_crypt_rsa_encrypt_session(me,
                                                         encryption_scheme,
                                                         NULL,
                                                         0,
                                                         OPTIGA_CRYPT_HOST_DATA,
                                                         &public_key_from_host,
                                                         encrypted_message,
                                                         &encrypted_message_length);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_encrypt_session operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //Encryption failed
            return_status = optiga_lib_status;
            break;
        }
        /**
         * 5. RSA decryption
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        encryption_scheme = OPTIGA_RSAES_PKCS1_V15;
        return_status = optiga_crypt_rsa_decrypt_and_store(me,
                                                           encryption_scheme,
                                                           encrypted_message,
                                                           encrypted_message_length,
                                                           NULL,
                                                           0,
                                                           optiga_key_id);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_crypt_rsa_decrypt_and_store operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //RSA Decryption failed
            return_status = optiga_lib_status;
            break;
        }

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

#endif  // OPTIGA_CRYPT_RSA_DECRYPT_ENABLED

/**
* @}
*/
