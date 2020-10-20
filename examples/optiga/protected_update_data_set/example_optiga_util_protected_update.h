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
* \file example_optiga_util_protected_update.h
*
* \brief   This file provides the data set for protected update of ecc key object with confidentiality.
*
* \ingroup grOptigaExamples
*
* @{
*/
#include "optiga/optiga_lib_config.h"
#include <stdint.h>

#include "data_object.h"
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_CONFIDENTIALITY_ENABLED
#include "data_object_confidentiality.h"
#endif
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_OBJECT_KEY_ENABLED
#include "aes_key_object_confidentiality.h"
#include "ecc_key_object_confidentiality.h"
#include "rsa_key_object_confidentiality.h"
#endif
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_OBJECT_METADATA_ENABLED    
#include "metadata_object_confidentiality.h"
#endif

/** @brief Macro to enable secure data update in OID example */
#define INTEGRITY_PROTECTED                  (0x00)
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_CONFIDENTIALITY_ENABLED
/** @brief Macro to enable secure data update in OID with confidentiality example */
#define CONFIDENTIALITY_PROTECTED            (0x01)
#endif
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_OBJECT_KEY_ENABLED
/** @brief Macro to enable secure AES key update in key object example */
#define AES_KEY_UPDATE                       (0x02)
/** @brief Macro to enable secure ECC key update in key object example */
#define ECC_KEY_UPDATE                       (0x03)
/** @brief Macro to enable secure RSA key update in key object example */
#define RSA_KEY_UPDATE                       (0x04)
#endif
#ifdef EXAMPLE_OPTIGA_UTIL_PROTECTED_UPDATE_OBJECT_METADATA_ENABLED
/** @brief Macro to enable secure metadata update of  OID example */
#define METADATA_UPDATE                      (0x05)
#endif

/**
 * \brief Specifies the structure for protected update manifest and fragment configuration
 */
typedef struct optiga_protected_update_manifest_fragment_configuration
{
    /// Manifest version.
    uint8_t manifest_version;
    /// Pointer to a buffer where manifest data is stored.
    const uint8_t * manifest_data;
    /// Manifest length
    uint16_t manifest_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const uint8_t * continue_fragment_data;
    /// Continue fragment length
    uint16_t continue_fragment_length;
    /// Pointer to a buffer where final fragment data is stored.
    const uint8_t * final_fragment_data;
    /// Final fragment length
    uint16_t final_fragment_length;
}optiga_protected_update_manifest_fragment_configuration_t;

#ifdef INTEGRITY_PROTECTED
/**
 * Integrity protected manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_integrity_configuration =
                                                                {
                                                                     0x01,
                                                                     manifest_int,
                                                                     sizeof(manifest_int),
                                                                     int_continue_fragment_array,
                                                                     sizeof(int_continue_fragment_array),
                                                                     int_final_fragment_array,
                                                                     sizeof(int_final_fragment_array)
                                                                };
#endif

#ifdef CONFIDENTIALITY_PROTECTED
/**
 * Integrity and confidentiality protected manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_confidentiality_configuration =
                                                                {
                                                                     0x01,
                                                                     manifest_int_conf,
                                                                     sizeof(manifest_int_conf),
                                                                     int_conf_continue_fragment_array,
                                                                     sizeof(int_conf_continue_fragment_array),
                                                                     int_conf_final_fragment_array,
                                                                     sizeof(int_conf_final_fragment_array)
                                                                };
#endif

#ifdef AES_KEY_UPDATE
/**
 * AES key update manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_aes_key_configuration =
                                                                {
                                                                     0x01,
                                                                     manifest_aes_key,
                                                                     sizeof(manifest_aes_key),
                                                                     NULL,
                                                                     0,
                                                                     aes_key_final_fragment_array,
                                                                     sizeof(aes_key_final_fragment_array)
                                                                };
#endif

#ifdef ECC_KEY_UPDATE
/**
 * ECC key update manifest and fragment configuration
 */                               
optiga_protected_update_manifest_fragment_configuration_t data_ecc_key_configuration = 
                                                                {    
                                                                     0x01,
                                                                     manifest_ecc_key,
                                                                     sizeof(manifest_ecc_key),
                                                                     NULL,
                                                                     0,
                                                                     ecc_key_final_fragment_array,
                                                                     sizeof(ecc_key_final_fragment_array)
                                                                };
#endif

#ifdef RSA_KEY_UPDATE                                                                
/**
 * RSA key update manifest and fragment configuration
 */                               
optiga_protected_update_manifest_fragment_configuration_t data_rsa_key_configuration = 
                                                                {    
                                                                     0x01,
                                                                     manifest_rsa_key,
                                                                     sizeof(manifest_rsa_key),
                                                                     NULL,
                                                                     0,
                                                                     rsa_key_final_fragment_array,
                                                                     sizeof(rsa_key_final_fragment_array)
                                                                };
#endif

#ifdef METADATA_UPDATE                                                                
/**
 * Metadata update manifest and fragment configuration
 */                               
optiga_protected_update_manifest_fragment_configuration_t metadata_update_configuration = 
                                                                {    
                                                                     0x01,
                                                                     manifest_metadata,
                                                                     sizeof(manifest_metadata),
                                                                     NULL,
                                                                     0,
                                                                     metadata_final_fragment_array,
                                                                     sizeof(metadata_final_fragment_array)
                                                                };
#endif

/**
 * \brief Specifies the structure for protected update data configuration
 */
typedef struct optiga_protected_update_data_configuration
{
    /// Target OID
    uint16_t target_oid;
    /// Target OID metadata
    const uint8_t * target_oid_metadata;
    /// Target OID metadata length
    uint16_t target_oid_metadata_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const optiga_protected_update_manifest_fragment_configuration_t * data_config;
    /// Pointer to a protected update example string.
    const char * set_prot_example_string;    
}optiga_protected_update_data_configuration_t;



/**
* @}
*/
