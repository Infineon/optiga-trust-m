/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_util_integration_tests.h
 *
 * \brief    This file defines APIs, types and data structures used in OPTIGA util integration tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#ifndef OPTIGA_UTIL_UNIT_TEST
#define OPTIGA_UTIL_UNIT_TEST

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "optiga_cmd.h"
#include "optiga_lib_common.h"
#include "optiga_util.h"
#include "pal_os_timer.h"
#include "protected_update_data_set/aes_key_object_confidentiality.h"
#include "protected_update_data_set/data_object.h"
#include "protected_update_data_set/data_object_confidentiality.h"
#include "protected_update_data_set/ecc_key_object_confidentiality.h"
#include "protected_update_data_set/metadata_object_confidentiality.h"
#include "protected_update_data_set/rsa_key_object_confidentiality.h"

/** @brief Macro to enable secure data update in OID example */
#define INTEGRITY_PROTECTED (0x00)
/** @brief Macro to enable secure data update in OID with confidentiality example */
#define CONFIDENTIALITY_PROTECTED (0x01)
/** @brief Macro to enable secure AES key update in key object example */
#define AES_KEY_UPDATE (0x02)
/** @brief Macro to enable secure ECC key update in key object example */
#define ECC_KEY_UPDATE (0x03)
/** @brief Macro to enable secure RSA key update in key object example */
#define RSA_KEY_UPDATE (0x04)
/** @brief Macro to enable secure metadata update of  OID example */
#define METADATA_UPDATE (0x05)

/**
 * \brief Specifies the structure for protected update manifest and fragment configuration
 */
typedef struct optiga_protected_update_manifest_fragment_configuration {
    /// Manifest version.
    uint8_t manifest_version;
    /// Pointer to a buffer where manifest data is stored.
    const uint8_t *manifest_data;
    /// Manifest length
    uint16_t manifest_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const uint8_t *continue_fragment_data;
    /// Continue fragment length
    uint16_t continue_fragment_length;
    /// Pointer to a buffer where final fragment data is stored.
    const uint8_t *final_fragment_data;
    /// Final fragment length
    uint16_t final_fragment_length;
} optiga_protected_update_manifest_fragment_configuration_t;

#ifdef INTEGRITY_PROTECTED
/**
 * Integrity protected manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_integrity_configuration = {
    0x01,
    manifest_int,
    sizeof(manifest_int),
    int_continue_fragment_array,
    sizeof(int_continue_fragment_array),
    int_final_fragment_array,
    sizeof(int_final_fragment_array)};
#endif

#ifdef CONFIDENTIALITY_PROTECTED
/**
 * Integrity and confidentiality protected manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_confidentiality_configuration = {
    0x01,
    manifest_int_conf,
    sizeof(manifest_int_conf),
    int_conf_continue_fragment_array,
    sizeof(int_conf_continue_fragment_array),
    int_conf_final_fragment_array,
    sizeof(int_conf_final_fragment_array)};
#endif

#ifdef AES_KEY_UPDATE
/**
 * AES key update manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_aes_key_configuration = {
    0x01,
    manifest_aes_key,
    sizeof(manifest_aes_key),
    NULL,
    0,
    aes_key_final_fragment_array,
    sizeof(aes_key_final_fragment_array)};
#endif

#ifdef ECC_KEY_UPDATE
/**
 * ECC key update manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_ecc_key_configuration = {
    0x01,
    manifest_ecc_key,
    sizeof(manifest_ecc_key),
    NULL,
    0,
    ecc_key_final_fragment_array,
    sizeof(ecc_key_final_fragment_array)};
#endif

#ifdef RSA_KEY_UPDATE
/**
 * RSA key update manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t data_rsa_key_configuration = {
    0x01,
    manifest_rsa_key,
    sizeof(manifest_rsa_key),
    NULL,
    0,
    rsa_key_final_fragment_array,
    sizeof(rsa_key_final_fragment_array)};
#endif

#ifdef METADATA_UPDATE
/**
 * Metadata update manifest and fragment configuration
 */
optiga_protected_update_manifest_fragment_configuration_t metadata_update_configuration = {
    0x01,
    manifest_metadata,
    sizeof(manifest_metadata),
    NULL,
    0,
    metadata_final_fragment_array,
    sizeof(metadata_final_fragment_array)};
#endif
/**
 * \brief Specifies the structure for protected update data configuration
 */
typedef struct optiga_protected_update_data_configuration {
    /// Target OID
    uint16_t target_oid;
    /// Target OID metadata
    const uint8_t *target_oid_metadata;
    /// Target OID metadata length
    uint16_t target_oid_metadata_length;
    /// Pointer to a buffer where continue fragment data is stored.
    const optiga_protected_update_manifest_fragment_configuration_t *data_config;
    /// Pointer to a protected update example string.
    const char *set_prot_example_string;
} optiga_protected_update_data_configuration_t;

#endif  // OPTIGA_UTIL_UNIT_TEST
