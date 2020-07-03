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
* \file example_optiga_util_protected_update.c
*
* \brief   This file provides the example for util protected update operation using #optiga_util_protected_update_start,
*           #optiga_util_protected_update_continue & #optiga_util_protected_update_final.
*
* \ingroup grOptigaExamples
*
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga_example.h"
#include "protected_update_data_set/example_optiga_util_protected_update.h"

/**
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
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
 * ECC-256 Trust Anchor
 */
const uint8_t trust_anchor [] = 
{
    0x30, 0x82, 0x02, 0x58, 0x30, 0x82, 0x01, 0xFF, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x2F,
    0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x56, 0x31, 0x0B,
    0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x49, 0x4E, 0x31, 0x0D, 0x30, 0x0B, 0x06,
    0x03, 0x55, 0x04, 0x0A, 0x0C, 0x04, 0x49, 0x46, 0x49, 0x4E, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03,
    0x55, 0x04, 0x0B, 0x0C, 0x03, 0x43, 0x43, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04,
    0x03, 0x0C, 0x0A, 0x49, 0x6E, 0x74, 0x43, 0x41, 0x20, 0x50, 0x32, 0x35, 0x36, 0x31, 0x15, 0x30,
    0x13, 0x06, 0x03, 0x55, 0x04, 0x2E, 0x13, 0x0C, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x41, 0x6E,
    0x63, 0x68, 0x6F, 0x72, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36, 0x30, 0x35, 0x32, 0x36, 0x30, 0x38,
    0x30, 0x31, 0x33, 0x37, 0x5A, 0x17, 0x0D, 0x31, 0x37, 0x30, 0x36, 0x30, 0x35, 0x30, 0x38, 0x30,
    0x31, 0x33, 0x37, 0x5A, 0x30, 0x5A, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
    0x02, 0x49, 0x4E, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x04, 0x49, 0x46,
    0x49, 0x4E, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x03, 0x43, 0x43, 0x53,
    0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x0E, 0x65, 0x6E, 0x64, 0x45, 0x6E,
    0x74, 0x69, 0x74, 0x79, 0x20, 0x50, 0x32, 0x35, 0x36, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55,
    0x04, 0x2E, 0x13, 0x0C, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x41, 0x6E, 0x63, 0x68, 0x6F, 0x72,
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x19, 0xB5, 0xB2, 0x17, 0x0D,
    0xF5, 0x98, 0x5E, 0xD4, 0xD9, 0x72, 0x16, 0xEF, 0x61, 0x39, 0x3F, 0x14, 0x58, 0xAF, 0x5C, 0x02,
    0x78, 0x07, 0xCA, 0x48, 0x8F, 0x2A, 0xE3, 0x90, 0xB9, 0x03, 0xA1, 0xD2, 0x46, 0x20, 0x09, 0x21,
    0x52, 0x98, 0xDC, 0x8E, 0x88, 0x84, 0x67, 0x8E, 0x83, 0xD1, 0xDE, 0x0F, 0x1C, 0xE5, 0x19, 0x1D,
    0x0C, 0x74, 0x60, 0x41, 0x58, 0x5B, 0x36, 0x55, 0xF8, 0x3D, 0xAB, 0xA3, 0x81, 0xB9, 0x30, 0x81,
    0xB6, 0x30, 0x09, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1D, 0x06, 0x03,
    0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xB5, 0x97, 0xFD, 0xAB, 0x36, 0x1A, 0xA0, 0xA2, 0x23,
    0xA7, 0x68, 0x25, 0x25, 0xFB, 0x82, 0x55, 0xD0, 0x4F, 0xCF, 0xB8, 0x30, 0x7A, 0x06, 0x03, 0x55,
    0x1D, 0x23, 0x04, 0x73, 0x30, 0x71, 0x80, 0x14, 0x1A, 0xBB, 0x56, 0x44, 0x65, 0x8C, 0x4D, 0x4F,
    0xCD, 0x29, 0xA2, 0x3F, 0x4C, 0xC6, 0xBC, 0xA8, 0x8B, 0xA4, 0x0A, 0xDA, 0xA1, 0x56, 0xA4, 0x54,
    0x30, 0x52, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x49, 0x4E, 0x31,
    0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x04, 0x49, 0x46, 0x49, 0x4E, 0x31, 0x0C,
    0x30, 0x0A, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x03, 0x43, 0x43, 0x53, 0x31, 0x0F, 0x30, 0x0D,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x06, 0x52, 0x6F, 0x6F, 0x74, 0x43, 0x41, 0x31, 0x15, 0x30,
    0x13, 0x06, 0x03, 0x55, 0x04, 0x2E, 0x13, 0x0C, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x41, 0x6E,
    0x63, 0x68, 0x6F, 0x72, 0x82, 0x01, 0x2E, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x1D, 0x0F, 0x01, 0x01,
    0xFF, 0x04, 0x04, 0x03, 0x02, 0x00, 0x81, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
    0x04, 0x03, 0x02, 0x03, 0x47, 0x00, 0x30, 0x44, 0x02, 0x20, 0x68, 0xFD, 0x9C, 0x8F, 0x35, 0x33,
    0x0B, 0xB8, 0x32, 0x8C, 0xAF, 0x1C, 0x81, 0x4E, 0x41, 0x29, 0x26, 0xCB, 0xB7, 0x10, 0xA0, 0x75,
    0xFC, 0x89, 0xAE, 0xC5, 0x1D, 0x92, 0x8E, 0x72, 0xEF, 0x5C, 0x02, 0x20, 0x7D, 0xC1, 0xEB, 0x58,
    0x21, 0xF1, 0xFD, 0xFB, 0x5E, 0xD7, 0xDE, 0x06, 0xC9, 0xB4, 0xFF, 0x59, 0x8D, 0x37, 0x8C, 0x7A,
    0x48, 0xCD, 0x2D, 0x99, 0x74, 0x77, 0x58, 0x9D, 0x95, 0x51, 0x8F, 0x5D
};

/**
 * Metadata for reset version tag :
 * Version Tag = 00.
 */
const uint8_t reset_version_tag_metadata[] = 
{
  0x20, 0x04,
        0xC1, 0x02, 0x00, 0x00
};

/**
 * Metadata for target OID :
 * Change access condition = Integrity protected using 0xE0E3.
 */
const uint8_t target_oid_metadata[] = 
{
  0x20, 0x05,
        0xD0, 0x03, 0x21, 0xE0, 0xE3,
}; 


/**
 * Metadata for target OID with confidentiality :
 * Change access condition = Integrity protected using 0xE0E3 & Confidentiality using 0xF1D1
 */
const uint8_t target_oid_metadata_with_confidentiality[] = 
{
    0x20, 0x09,
          //0xC1, 0x02, 0x00, 0x00,
          0xD0, 0x07, 0x21, 0xE0, 0xE3, 0xFD, 0x20, 0xF1, 0xD1
};

/**
 * Metadata for secure update of target OID metadata with confidentiality :
 * Change access condition = Integrity protected using 0xE0E3 & Confidentiality using 0xF1D1
 */
const uint8_t target_oid_metadata_for_secure_metadata_update[] =
{
    0x20, 0x0C,
        0xD8, 0x07, 0x21, 0xE0, 0xE3, 0xFD, 0x20, 0xF1, 0xD1, 0xF0, 0x01, 0x01
};

/**
 * Metadata for Trust Anchor :
 * Execute access condition = Always
 * Data object type  =  Trust Anchor
 */
uint8_t trust_anchor_metadata[] = 
{
    0x20, 0x06,
          0xD3, 0x01, 0x00,
          0xE8, 0x01, 0x11
};

/**
 * Metadata for target key OID :
 * Change access condition = Integrity protected using 0xE0E3
 * Execute access condition = Always
 */
const uint8_t target_key_oid_metadata[] = 
{
    0x20, 0x0C,
            0xC1, 0x02, 0x00, 0x00,
            0xD0, 0x03, 0x21, 0xE0, 0xE3,
            0xD3, 0x01, 0x00
};

/**
 * Metadata for shared secret OID :
 * Execute access condition = Always
 * Data object type  =  Protected updated secret
 */
uint8_t confidentiality_oid_metadata[] = 
{
    0x20, 0x06,
            0xD3, 0x01, 0x00,
            0xE8, 0x01, 0x23
};


/**
 * Shared secret data
 */
const unsigned char shared_secret[] = 
{
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27, 
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6
};


const optiga_protected_update_data_configuration_t  optiga_protected_update_data_set[] =
{
#ifdef INTEGRITY_PROTECTED
    {
        0xE0E1,
        target_oid_metadata,
        sizeof(target_oid_metadata),
        &data_integrity_configuration
    },
#endif

#ifdef CONFIDENTIALITY_PROTECTED
    {
        0xE0E1,
        target_oid_metadata_with_confidentiality,
        sizeof(target_oid_metadata_with_confidentiality),
        &data_confidentiality_configuration
    },
#endif

#ifdef AES_KEY_UPDATE
    {
        0xE200,
        target_oid_metadata,
        sizeof(target_oid_metadata),
        &data_aes_key_configuration
    },
#endif

#ifdef ECC_KEY_UPDATE
    {
        0xE0F1,
        target_key_oid_metadata,
        sizeof(target_key_oid_metadata),
        &data_ecc_key_configuration
    },
#endif

#ifdef METADATA_UPDATE
    {
        0xE0E2,
        target_oid_metadata_for_secure_metadata_update,
        sizeof(target_oid_metadata_for_secure_metadata_update),
        &metadata_update_configuration
    },
#endif
    
#ifdef RSA_KEY_UPDATE
    {
        0xE0FC,
        target_key_oid_metadata,
        sizeof(target_key_oid_metadata),
        &data_rsa_key_configuration
    },
#endif
};

/**
 * Local functions prototype
 */
static optiga_lib_status_t write_metadata(optiga_util_t * me, uint16_t oid, uint8_t * metadata, uint8_t metadata_length)
{
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;

    do
    {
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me,
                                                   oid,
                                                   metadata,
                                                   metadata_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_write_metadata operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //writing metadata to a data object failed.
            return_status = optiga_lib_status;
            break;
        }
    } while (FALSE);

    return(return_status);
}

// Precondition 2
static optiga_lib_status_t write_confidentiality_oid(optiga_util_t * me, uint16_t confidentiality_oid)
{
    optiga_lib_status_t return_status = OPTIGA_UTIL_ERROR;

    do
    {
        /**
         * Precondition :
         * Metadata for 0xF1D1 :
         * Execute access condition = Always
         * Data object type  =  Protected updated secret
         */
        return_status = write_metadata(me,
                                       confidentiality_oid, 
                                       confidentiality_oid_metadata,
                                       sizeof(confidentiality_oid_metadata));
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }


        /**
        *  Precondition :
        *  Write shared secret in OID 0xF1D1
        */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me,
                                                confidentiality_oid,
                                                OPTIGA_UTIL_ERASE_AND_WRITE,
                                                0,
                                                shared_secret,
                                                sizeof(shared_secret));

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_write_data operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //writing data to a data object failed.
            return_status = optiga_lib_status;            
            break;
        }
    } while (FALSE);
    return (return_status);
}


// Precondition 1
static optiga_lib_status_t write_trust_anchor(optiga_util_t * me, uint16_t trust_anchor_oid)
{
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;

    do
    {
        /**
         * Precondition :
         * Update Metadata for 0xE0E3 :
         * Execute access condition = Always
         * Data object type  =  Trust Anchor
         */
        return_status = write_metadata(me, trust_anchor_oid, trust_anchor_metadata, sizeof(trust_anchor_metadata));
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(me,
                                               trust_anchor_oid,
                                               OPTIGA_UTIL_ERASE_AND_WRITE,
                                               0,
                                               trust_anchor,
                                               sizeof(trust_anchor));

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status)
        {
            //Wait until the optiga_util_write_data operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_status)
        {
            //writing data to a data object failed.
            return_status = optiga_lib_status;
            break;
        }
    } while (FALSE);
    return(return_status);
}

void example_optiga_util_protected_update(void)
{
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;
    optiga_util_t * me = NULL;
    uint16_t trust_anchor_oid = 0xE0E3;
    uint16_t confidentiality_oid = 0xF1D1;
    uint16_t data_config = 0;
    OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);

    do
    {
        /**
         *  Create OPTIGA util Instance
         *
         */
        me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == me)
        {
            break;
        }

        /**
        *  Precondition 1 :
        *  Update the metadata and trust anchor in OID 0xE0E3
        */
        return_status = write_trust_anchor(me, trust_anchor_oid);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        /**
        *  Precondition 2 :
        *  Update the metadata and secret for confidentiality in OID 0xF1D1
        */
        return_status = write_confidentiality_oid(me, confidentiality_oid);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }
        for (data_config = 0; 
            data_config < \
            sizeof(optiga_protected_update_data_set)/sizeof(optiga_protected_update_data_configuration_t); data_config++)
        
        {
            /**
            *  Precondition 3 :
            *  Update the metadata of target OID
            */
            return_status = write_metadata(me,
                                       optiga_protected_update_data_set[data_config].target_oid,
                                       (uint8_t * )optiga_protected_update_data_set[data_config].target_oid_metadata,
                                       (uint8_t)optiga_protected_update_data_set[data_config].target_oid_metadata_length);
            if (OPTIGA_LIB_SUCCESS != return_status)
                
            {
                break;
            }
            

            /**
            *   Send the manifest using optiga_util_protected_update_start
            */
            
            optiga_lib_status = OPTIGA_LIB_BUSY;
            return_status = optiga_util_protected_update_start(me,
                                                               optiga_protected_update_data_set[data_config].data_config->manifest_version,
                                                               optiga_protected_update_data_set[data_config].data_config->manifest_data,
                                                               optiga_protected_update_data_set[data_config].data_config->manifest_length);

            WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

            if (NULL != optiga_protected_update_data_set[data_config].data_config->continue_fragment_data)
            {
                /**
                *   Send the first fragment using optiga_util_protected_update_continue
                */
                optiga_lib_status = OPTIGA_LIB_BUSY;
                return_status = optiga_util_protected_update_continue(me,
                                                                      optiga_protected_update_data_set[data_config].data_config->continue_fragment_data,
                                                                      optiga_protected_update_data_set[data_config].data_config->continue_fragment_length);

                WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
            }

            /**
            *   Send the last fragment using optiga_util_protected_update_final
            */
            optiga_lib_status = OPTIGA_LIB_BUSY;
            return_status = optiga_util_protected_update_final(me,
                                                               optiga_protected_update_data_set[data_config].data_config->final_fragment_data,
                                                               optiga_protected_update_data_set[data_config].data_config->final_fragment_length);

            WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);
            /**
            *  Revert the version tag of metadata configuration to re-run the protected update examples
            *  Update the metadata of target OID for version tag to be 00
            */
            return_status = write_metadata(me,
                                           optiga_protected_update_data_set[data_config].target_oid,
                                           (uint8_t * )reset_version_tag_metadata,
                                           (uint8_t)sizeof(reset_version_tag_metadata));
            if (OPTIGA_LIB_SUCCESS != return_status)
            {
                break;
            }
            
        }
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);
    
    if (me)
    {
        //Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            //lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}

/**
* @}
*/
