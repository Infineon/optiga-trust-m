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
* \file example_optiga_hmac_verify_with_authorization_reference.c
*
* \brief   This file provides an example for HMAC verify with authorization reference.
*
* \ingroup grUseCases
*
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga/optiga_crypt.h"

#include "optiga/pal/pal_os_memory.h"
#include "optiga/pal/pal_crypt.h"
#include "optiga_example.h"
#include "mbedtls/ccm.h"
#include "mbedtls/md.h"
#include "mbedtls/ssl.h"
#if defined (OPTIGA_CRYPT_GENERATE_AUTH_CODE_ENABLED) && defined (OPTIGA_CRYPT_HMAC_VERIFY_ENABLED)

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

/**
 * Metadata for Secret OID :
 * Execute access condition = Always
 * Data object type  =  Auto Ref
 */
const uint8_t secret_oid_metadata[] = 
{
    0x20, 0x06,
          0xD3, 0x01, 0x00,
          0xE8, 0x01, 0x31
};

/**
 * Metadata for arbitrary OID :
 * Read access condition = Auto Ref
 * Data object type  =  Auto Ref
 */
const uint8_t arbitrary_oid_metadata[] = 
{
    0x20, 0x05,
          0xD1, 0x03, 0x23, 0xF1, 0xD0
};

/**
 * Metadata for arbitrary OID :
 * Read access condition = Always
 */
const uint8_t arbitrary_oid_metadata_default[] = 
{
    0x20, 0x03,
          0xD1, 0x01, 0x00
};

/**
 * Shared secret data
 */
const uint8_t user_secret[] = 
{
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6
};

/**
 * Read OID data
 */
const uint8_t read_oid_data[] = 
{
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27,
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6 
};

/**
 * Optional data
 */
const uint8_t optional_data[] = 
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 
};

/**
 * random data
 */
uint8_t random_data[32] = {0x00};

/**
 * Arbitrary data
 */
const uint8_t arbitrary_data[] = 
{
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF 
};

/**
 * Input data
 */
uint8_t input_data_buffer[64] = {0x00};

/**
 * Generated hmac
 */
uint8_t hmac_buffer[32] = {0x00};

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

//lint --e{818, 715, 830} suppress "argument "p_pal_crypt" is not used in the implementation but kept for future use"
/**
 * \brief Calculates the HMAC on the input data with the provided secret key.
 *
 * \details
 * Calculates the HMAC on the input data with the provided secret key and returns HMAC.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in]           p_pal_crypt                 Crypt context
 * \param[in]           hmac_type                   HMAC of type #optiga_hmac_type_t.
 * \param[in]           secret_key                  HMAC secret key.
 * \param[in]           secret_key_len              Length of HMAC secret key.
 * \param[in]           input_data                  Pointer to input data for HMAC generation .
 * \param[in]           input_data_length           Length of input data for HMAC generation.
 * \param[in,out]       hmac                        Pointer to buffer to store generated HMAC.
 *
 * \retval              PAL_STATUS_SUCCESS          In case of success
 * \retval              PAL_STATUS_FAILURE          In case of failure
 */ 
static pal_status_t pal_crypt_hmac(pal_crypt_t* p_pal_crypt,
                                   uint16_t hmac_type,
                                   const uint8_t * secret_key,
                                   uint16_t secret_key_len,
                                   const uint8_t * input_data,
                                   uint32_t input_data_length,
                                   uint8_t * hmac)
{
    pal_status_t return_value = PAL_STATUS_FAILURE;

    const mbedtls_md_info_t * hmac_info;
    mbedtls_md_type_t digest_type;
    
    do
    {
#ifdef OPTIGA_LIB_DEBUG_NULL_CHECK
        if ((NULL == input_data) || (NULL == hmac))
        {
            break;
        }
#endif  //OPTIGA_LIB_DEBUG_NULL_CHECK

        digest_type = (((uint16_t)OPTIGA_HMAC_SHA_256 == hmac_type)? MBEDTLS_MD_SHA256: MBEDTLS_MD_SHA384);
        
        hmac_info = mbedtls_md_info_from_type(digest_type);

        if (0 != mbedtls_md_hmac(hmac_info, secret_key, secret_key_len, input_data, input_data_length, hmac))
        {
            break;
        }
        
        return_value = PAL_STATUS_SUCCESS;

    } while (FALSE);

    return return_value;
}

pal_status_t CalcHMAC(const uint8_t * secret_key,
                           uint16_t secret_key_len,
                           const uint8_t * input_data,
                           uint32_t input_data_length,
                           uint8_t * hmac)
{
    return(pal_crypt_hmac(NULL,
                          (uint16_t)OPTIGA_HMAC_SHA_256,
                          secret_key,
                          secret_key_len,
                          input_data,
                          input_data_length,
                          hmac));
}

static pal_status_t GetAutoValue(optiga_util_t * me_util, uint16_t secret_oid)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    
    do
    {
        /**
         * 1. Set the metadata of secret OID(0xF1D0) using optiga_util_write_metadata.
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   secret_oid,
                                                   secret_oid_metadata,
                                                   sizeof(secret_oid_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
        *  2. Write shared secret in OID 0xF1D0
        */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me_util,
                                               secret_oid,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               user_secret,
                                               sizeof(user_secret));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
    } while(FALSE);
    
    return return_status;
}

/**
 * The below example demonstrates HMAC verify with authorization reference.
 */
void example_optiga_hmac_verify_with_authorization_reference(void)
{
    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    pal_status_t pal_return_status;
    uint16_t offset, bytes_to_read;
    uint8_t read_data_buffer[100];
    optiga_util_t * me_util = NULL;
    optiga_crypt_t * me_crypt = NULL;

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
         * Create OPTIGA util and crypt Instances
         */
        me_util = optiga_util_create(0, optiga_lib_callback, NULL);
        if (NULL == me_util)
        {
            break;
        }

        me_crypt = optiga_crypt_create(0, optiga_lib_callback, NULL);
        if (NULL == me_crypt)
        {
            break;
        }

        /**
         * Initialize the protection level and protocol version for the instances
         */
        OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(me_util,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(me_util,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);

        OPTIGA_CRYPT_SET_COMMS_PROTECTION_LEVEL(me_crypt,OPTIGA_COMMS_NO_PROTECTION);
        OPTIGA_CRYPT_SET_COMMS_PROTOCOL_VERSION(me_crypt,OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
        
        /**
         * Precondition : Get the User Secret and store it in OID
         */
        // Function name in line with SRM
        return_status = GetAutoValue(me_util, 0xF1D0);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        
        /**
         * Set the data to 0xF1E0.
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me_util,
                                               0xF1E0,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               read_oid_data,
                                               sizeof(read_oid_data));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * Set the metadata of 0xF1E0 to Auto with 0xF1D0.
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   0xF1E0,
                                                   arbitrary_oid_metadata,
                                                   sizeof(arbitrary_oid_metadata));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * Read data from a data object using optiga_util_read_data.
         * Read returns failure as there is auto reference condition is not met
         */
        offset = 0x00;
        bytes_to_read = sizeof(read_data_buffer);        
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_data(me_util,
                                              0xF1E0,
                                              offset,
                                              read_data_buffer,
                                              &bytes_to_read);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_read_data operation is completed
        }

        if ((OPTIGA_LIB_SUCCESS == optiga_lib_status) && (0 != bytes_to_read))
        {
            //Reading successed
            return_status = !OPTIGA_LIB_SUCCESS;
            break;
        }
        
        /**
         * Generate authorization code with optional data
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_generate_auth_code(me_crypt,
                                                        OPTIGA_RNG_TYPE_TRNG,
                                                        optional_data,
                                                        sizeof(optional_data),
                                                        random_data,
                                                        sizeof(random_data));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
         * Calculate HMAC on host
         */
        pal_os_memcpy(input_data_buffer, optional_data, sizeof(optional_data));
        pal_os_memcpy(&input_data_buffer[sizeof(optional_data)], random_data, sizeof(random_data));
        pal_os_memcpy(&input_data_buffer[sizeof(optional_data) + sizeof(random_data)], arbitrary_data, sizeof(arbitrary_data));
        
        // Function name in line with SRM
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
         * Perform HMAC verification using OPTIGA
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_crypt_hmac_verify(me_crypt,
                                                 OPTIGA_HMAC_SHA_256,
                                                 0xF1D0,
                                                 input_data_buffer,
                                                 sizeof(input_data_buffer),
                                                 hmac_buffer,
                                                 sizeof(hmac_buffer));
        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        /**
        * PostCondition : Read data from a data object using optiga_util_read_data.
        * Read data returns SUCCESS
         */
        offset = 0x00;
        bytes_to_read = sizeof(read_data_buffer);        
        
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_read_data(me_util,
                                              0xF1E0,
                                              offset,
                                              read_data_buffer,
                                              &bytes_to_read);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        /**
         * Reset the metadata of 0xF1E0 to default.
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me_util,
                                                   0xF1E0,
                                                   arbitrary_oid_metadata_default,
                                                   sizeof(arbitrary_oid_metadata_default));

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
        
        return_status = OPTIGA_LIB_SUCCESS;

    } while(FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
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
    
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif //OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    
}
#endif
/**
* @}
*/
