/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_util_integration_tests.c
 *
 * \brief   This file implements the OPTIGA util integration tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_util_integration_test.h"

optiga_lib_status_t optiga_lib_status;

/**
 * Sample Trust Anchor
 */
static const uint8_t trust_anchor[] = {
    // 00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F
    0x30, 0x82, 0x02, 0x7E, 0x30, 0x82, 0x02, 0x05, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
    0x9B, 0x0C, 0x24, 0xB4, 0x5E, 0x7D, 0xE3, 0x73, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE,
    0x3D, 0x04, 0x03, 0x02, 0x30, 0x74, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
    0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x18, 0x49, 0x6E,
    0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6E, 0x6F, 0x6C, 0x6F, 0x67,
    0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31, 0x1B, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C,
    0x12, 0x4F, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4D, 0x29, 0x20, 0x54, 0x72, 0x75, 0x73,
    0x74, 0x20, 0x58, 0x31, 0x25, 0x30, 0x23, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x1C, 0x49, 0x6E,
    0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x53, 0x65, 0x72, 0x76,
    0x65, 0x72, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36,
    0x31, 0x30, 0x31, 0x34, 0x30, 0x33, 0x35, 0x38, 0x33, 0x36, 0x5A, 0x17, 0x0D, 0x34, 0x31, 0x31,
    0x30, 0x30, 0x38, 0x30, 0x33, 0x35, 0x38, 0x33, 0x36, 0x5A, 0x30, 0x74, 0x31, 0x0B, 0x30, 0x09,
    0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55,
    0x04, 0x0A, 0x0C, 0x18, 0x49, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x54, 0x65, 0x63,
    0x68, 0x6E, 0x6F, 0x6C, 0x6F, 0x67, 0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31, 0x1B, 0x30, 0x19,
    0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x12, 0x4F, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4D,
    0x29, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x58, 0x31, 0x25, 0x30, 0x23, 0x06, 0x03, 0x55,
    0x04, 0x03, 0x0C, 0x1C, 0x49, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x54, 0x65, 0x73,
    0x74, 0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x43, 0x41,
    0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x05, 0x2B,
    0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0x7B, 0x2E, 0xE6, 0xFB, 0xBD, 0x6F, 0x40, 0x0F,
    0x41, 0x9F, 0xE5, 0xF0, 0x8C, 0x97, 0x21, 0xB0, 0x07, 0xB5, 0xBB, 0xD2, 0xB8, 0x5A, 0x14, 0x3B,
    0x75, 0x54, 0x7E, 0xEA, 0xFE, 0xF2, 0x8D, 0x5A, 0xB8, 0x54, 0xE0, 0xC8, 0xAD, 0xED, 0xF1, 0xD5,
    0x8B, 0x97, 0xBA, 0x02, 0x3E, 0xD9, 0x25, 0xE0, 0x00, 0x86, 0x17, 0x35, 0xE6, 0xE6, 0xD9, 0x12,
    0x0F, 0x8A, 0x21, 0x1C, 0x62, 0xFA, 0xCE, 0xF6, 0x9E, 0xB1, 0xF8, 0x8C, 0xA3, 0xDC, 0x52, 0x04,
    0x83, 0xEB, 0xA0, 0xB3, 0xFA, 0xB0, 0xCA, 0x02, 0x30, 0xB1, 0xFE, 0x53, 0x4E, 0xAD, 0xFB, 0xE0,
    0x88, 0x05, 0x86, 0x4E, 0x5E, 0x67, 0xEB, 0x7B, 0xA3, 0x63, 0x30, 0x61, 0x30, 0x1D, 0x06, 0x03,
    0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0x91, 0x4A, 0x4B, 0x07, 0x58, 0xB2, 0xC6, 0x4B, 0x37,
    0xFD, 0x91, 0x62, 0xD8, 0x8A, 0x17, 0x28, 0xAA, 0x94, 0x18, 0x62, 0x30, 0x1F, 0x06, 0x03, 0x55,
    0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x91, 0x4A, 0x4B, 0x07, 0x58, 0xB2, 0xC6, 0x4B,
    0x37, 0xFD, 0x91, 0x62, 0xD8, 0x8A, 0x17, 0x28, 0xAA, 0x94, 0x18, 0x62, 0x30, 0x0F, 0x06, 0x03,
    0x55, 0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xFF, 0x30, 0x0E, 0x06,
    0x03, 0x55, 0x1D, 0x0F, 0x01, 0x01, 0xFF, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x0A, 0x06,
    0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x67, 0x00, 0x30, 0x64, 0x02, 0x30,
    0x20, 0x1C, 0x7A, 0x21, 0x50, 0x50, 0xC9, 0x15, 0x1C, 0xC5, 0x14, 0x8D, 0x46, 0x5C, 0xA6, 0xD3,
    0x81, 0xCE, 0x57, 0x06, 0x1A, 0xAE, 0x39, 0x10, 0x27, 0x51, 0x42, 0xEF, 0xCD, 0x64, 0x75, 0x99,
    0xDE, 0x0D, 0x3D, 0x01, 0x47, 0x69, 0xFC, 0x93, 0x6D, 0x99, 0xC7, 0xF0, 0xF8, 0x8C, 0xAA, 0xD1,
    0x02, 0x30, 0x68, 0xC3, 0x27, 0xD9, 0x0F, 0x52, 0xAD, 0x3A, 0xA8, 0xDB, 0xF8, 0x53, 0x11, 0x1D,
    0xF1, 0x30, 0x6B, 0x39, 0xF3, 0x3F, 0xEF, 0x65, 0x61, 0xBE, 0xC4, 0xDD, 0x19, 0x11, 0x1E, 0x83,
    0xF9, 0xE8, 0x3F, 0x41, 0x97, 0x45, 0xFC, 0x61, 0xE0, 0x06, 0xD0, 0xE6, 0xF7, 0x5C, 0x9F, 0xE2,
    0x57, 0xC2,
};

/**
 * Sample metadata
 */
static const uint8_t metadata[] = {
    // Metadata tag in the data object
    0x20,
    0x05,
    // Read tag in the metadata
    0xD1,
    0x03,
    // LcsO < Operation
    0xE1,
    0xFB,
    0x03,
};

/**
 * Initialize the counter object with a threshold value 0x0A
 */
static const uint8_t initial_counter_object_data[] = {
    // Initial counter value
    0x00,
    0x00,
    0x00,
    0x00,
    // Threshold value
    0x00,
    0x00,
    0x00,
    0x0A,
};

/**
 * Metadata for reset version tag :
 * Version Tag = 00.
 */
const uint8_t reset_version_tag_metadata[] = {0x20, 0x04, 0xC1, 0x02, 0x00, 0x00};

/**
 * Metadata for target OID :
 * Change access condition = Integrity protected using 0xE0E3.
 */
const uint8_t target_oid_metadata[] = {
    0x20,
    0x05,
    0xD0,
    0x03,
    0x21,
    0xE0,
    0xE3,
};

/**
 * Metadata for target OID with confidentiality :
 * Change access condition = Integrity protected using 0xE0E3 & Confidentiality using 0xF1D1
 */
const uint8_t target_oid_metadata_with_confidentiality[] = {
    0x20,
    0x09,
    // 0xC1, 0x02, 0x00, 0x00,
    0xD0,
    0x07,
    0x21,
    0xE0,
    0xE3,
    0xFD,
    0x20,
    0xF1,
    0xD1};

/**
 * Metadata for secure update of target OID metadata with confidentiality :
 * Change access condition = Integrity protected using 0xE0E3 & Confidentiality using 0xF1D1
 */
const uint8_t target_oid_metadata_for_secure_metadata_update[] =
    {0x20, 0x0C, 0xD8, 0x07, 0x21, 0xE0, 0xE3, 0xFD, 0x20, 0xF1, 0xD1, 0xF0, 0x01, 0x01};

/**
 * Metadata for Trust Anchor :
 * Execute access condition = Always
 * Data object type  =  Trust Anchor
 */
uint8_t trust_anchor_metadata[] = {0x20, 0x06, 0xD3, 0x01, 0x00, 0xE8, 0x01, 0x11};

/**
 * Metadata for target key OID :
 * Change access condition = Integrity protected using 0xE0E3
 * Execute access condition = Always
 */
const uint8_t target_key_oid_metadata[] =
    {0x20, 0x0C, 0xC1, 0x02, 0x00, 0x00, 0xD0, 0x03, 0x21, 0xE0, 0xE3, 0xD3, 0x01, 0x00};

/**
 * Metadata for shared secret OID :
 * Execute access condition = Always
 * Data object type  =  Protected updated secret
 */
uint8_t confidentiality_oid_metadata[] = {0x20, 0x06, 0xD3, 0x01, 0x00, 0xE8, 0x01, 0x23};

/**
 * Shared secret data
 */
const unsigned char shared_secret[] = {
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27,
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6,
    0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6, 0xD4, 0xC5, 0x4F, 0x5B, 0x12, 0xC5, 0x7E, 0xDB, 0x27,
    0xCE, 0xD2, 0x24, 0x04, 0x8F, 0x25, 0x48, 0x2A, 0xA1, 0x49, 0xC9, 0xF4, 0x92, 0xA9, 0x92, 0xF6};

const optiga_protected_update_data_configuration_t optiga_protected_update_data_set[] = {
#ifdef INTEGRITY_PROTECTED
    {0xE0E1,
     target_oid_metadata,
     sizeof(target_oid_metadata),
     &data_integrity_configuration,
     "Protected Update - Integrity"},
#endif

#ifdef CONFIDENTIALITY_PROTECTED
    {0xE0E1,
     target_oid_metadata_with_confidentiality,
     sizeof(target_oid_metadata_with_confidentiality),
     &data_confidentiality_configuration,
     "Protected Update - Confidentiality"},
#endif

#ifdef AES_KEY_UPDATE
    {0xE200,
     target_oid_metadata,
     sizeof(target_oid_metadata),
     &data_aes_key_configuration,
     "Protected Update - AES Key"},
#endif

#ifdef ECC_KEY_UPDATE
    {0xE0F1,
     target_key_oid_metadata,
     sizeof(target_key_oid_metadata),
     &data_ecc_key_configuration,
     "Protected Update - ECC Key"},
#endif

#ifdef METADATA_UPDATE
    {0xE0E2,
     target_oid_metadata_for_secure_metadata_update,
     sizeof(target_oid_metadata_for_secure_metadata_update),
     &metadata_update_configuration,
     "Protected Update - Metadata"},
#endif

#ifdef RSA_KEY_UPDATE
    {0xE0FC,
     target_key_oid_metadata,
     sizeof(target_key_oid_metadata),
     &data_rsa_key_configuration,
     "Protected Update - RSA Key"},
#endif
};

/**
 * Local functions prototype
 */
static optiga_lib_status_t
write_metadata(optiga_util_t *me, uint16_t oid, uint8_t *metadata, uint8_t metadata_length) {
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;

    do {
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_metadata(me, oid, metadata, metadata_length);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) {
            // Wait until the optiga_util_write_metadata operation is completed
        }

    } while (FALSE);

    return (return_status);
}

// Precondition 2
static optiga_lib_status_t
write_confidentiality_oid(optiga_util_t *me, uint16_t confidentiality_oid) {
    optiga_lib_status_t return_status = OPTIGA_UTIL_ERROR;

    do {
        /**
         * Precondition :
         * Metadata for 0xF1D1 :
         * Execute access condition = Always
         * Data object type  =  Protected updated secret
         */
        return_status = write_metadata(
            me,
            confidentiality_oid,
            confidentiality_oid_metadata,
            sizeof(confidentiality_oid_metadata)
        );
        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        /**
         *  Precondition :
         *  Write shared secret in OID 0xF1D1
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(
            me,
            confidentiality_oid,
            OPTIGA_UTIL_ERASE_AND_WRITE,
            0,
            shared_secret,
            sizeof(shared_secret)
        );

        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) {
            // Wait until the optiga_util_write_data operation is completed
        }
    } while (FALSE);
    return (return_status);
}

// Precondition 1
static optiga_lib_status_t write_trust_anchor(optiga_util_t *me, uint16_t trust_anchor_oid) {
    optiga_lib_status_t return_status = OPTIGA_LIB_SUCCESS;

    do {
        /**
         * Precondition :
         * Update Metadata for 0xE0E3 :
         * Execute access condition = Always
         * Data object type  =  Trust Anchor
         */
        return_status = write_metadata(
            me,
            trust_anchor_oid,
            trust_anchor_metadata,
            sizeof(trust_anchor_metadata)
        );
        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_write_data(
            me,
            trust_anchor_oid,
            OPTIGA_UTIL_ERASE_AND_WRITE,
            0,
            trust_anchor,
            sizeof(trust_anchor)
        );

        if (OPTIGA_LIB_SUCCESS != return_status) {
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_status) {
            // Wait until the optiga_util_write_data operation is completed
        }

    } while (FALSE);
    return (return_status);
}

// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

void ut_optiga_util_open_close_fct() {
    optiga_util_t *ut_optiga_util_instance = NULL;
    optiga_lib_status_t ut_optiga_lib_status;

    ut_optiga_util_instance = optiga_util_create(0, optiga_util_callback, NULL);
    assert(ut_optiga_util_instance != NULL);
    assert(ut_optiga_util_instance->instance_state == OPTIGA_LIB_SUCCESS);

    ut_optiga_lib_status = optiga_util_open_application(ut_optiga_util_instance, FALSE);
    assert(ut_optiga_lib_status == OPTIGA_LIB_SUCCESS);

    ut_optiga_util_instance->instance_state = OPTIGA_LIB_INSTANCE_FREE;
    ut_optiga_lib_status = optiga_util_close_application(ut_optiga_util_instance, FALSE);
    assert(ut_optiga_lib_status == OPTIGA_LIB_SUCCESS);

    pal_os_timer_delay_in_milliseconds(100);

    ut_optiga_util_instance->instance_state = OPTIGA_LIB_INSTANCE_FREE;
    ut_optiga_lib_status = optiga_util_open_application(ut_optiga_util_instance, TRUE);
    assert(ut_optiga_lib_status == OPTIGA_LIB_SUCCESS);

    ut_optiga_util_instance->instance_state = OPTIGA_LIB_INSTANCE_FREE;
    ut_optiga_lib_status = optiga_util_close_application(ut_optiga_util_instance, TRUE);
    assert(ut_optiga_lib_status == OPTIGA_LIB_SUCCESS);

    ut_optiga_util_instance->instance_state = OPTIGA_LIB_INSTANCE_FREE;
    ut_optiga_lib_status = optiga_util_destroy(ut_optiga_util_instance);
    assert(ut_optiga_lib_status == OPTIGA_LIB_SUCCESS);
}

void ut_optiga_util_write_data_metadata_fct() {
    optiga_lib_status_t ut_return_status;
    uint16_t offset;
    uint16_t optiga_oid, optiga_counter_oid;
    optiga_util_t *ut_optiga_util_instance = NULL;

    /** Create OPTIGA Util Instance */
    ut_optiga_util_instance = optiga_util_create(0, optiga_util_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    // Read device end entity certificate from OPTIGA
    optiga_oid = 0xE0E0;
    offset = 0x00;

    // OPTIGA Comms Shielded connection settings to enable the protection
    OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(ut_optiga_util_instance, OPTIGA_COMMS_NO_PROTECTION);

    /**
     * Read data from a data object (e.g. certificate data object)
     * using optiga_util_read_data.
     */
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_util_write_data(
        ut_optiga_util_instance,
        optiga_oid,
        OPTIGA_UTIL_ERASE_AND_WRITE,
        offset,
        trust_anchor,
        sizeof(trust_anchor)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status =
        optiga_util_write_metadata(ut_optiga_util_instance, 0xE0E8, metadata, sizeof(metadata));

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    optiga_counter_oid = 0xE120;
    offset = 0x00;
    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_util_write_data(
        ut_optiga_util_instance,
        optiga_counter_oid,
        OPTIGA_UTIL_ERASE_AND_WRITE,
        offset,
        initial_counter_object_data,
        sizeof(initial_counter_object_data)
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    // In this example, the counter is update by 5 and the final count would be 15
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_update_count(ut_optiga_util_instance, optiga_counter_oid, 0x05);

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
}

void ut_optiga_util_read_data_metadata_fct() {
    optiga_lib_status_t ut_return_status;
    uint16_t offset, bytes_to_read;
    uint16_t optiga_oid;
    uint8_t read_data_buffer[1024];
    optiga_util_t *ut_optiga_util_instance = NULL;

    /** Create OPTIGA Util Instance */
    ut_optiga_util_instance = optiga_util_create(0, optiga_util_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    // Read device end entity certificate from OPTIGA
    optiga_oid = 0xE0E8;
    offset = 0x00;
    bytes_to_read = sizeof(read_data_buffer);

    // OPTIGA Comms Shielded connection settings to enable the protection
    OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(
        ut_optiga_util_instance,
        OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET
    );
    OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(
        ut_optiga_util_instance,
        OPTIGA_COMMS_RESPONSE_PROTECTION
    );

    optiga_lib_status = OPTIGA_LIB_BUSY;

    ut_return_status = optiga_util_read_data(
        ut_optiga_util_instance,
        optiga_oid,
        offset,
        read_data_buffer,
        &bytes_to_read
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);
    /**
     * Read metadata of a data object (e.g. certificate data object E0E0)
     * using optiga_util_read_data.
     */
    optiga_oid = 0xE0E0;
    bytes_to_read = sizeof(read_data_buffer);
    optiga_lib_status = OPTIGA_LIB_BUSY;
    ut_return_status = optiga_util_read_metadata(
        ut_optiga_util_instance,
        optiga_oid,
        read_data_buffer,
        &bytes_to_read
    );

    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    while (OPTIGA_LIB_BUSY == optiga_lib_status) {
    };
    /* This is a dummy PAL, no chip exists */
    // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

    ut_return_status = optiga_util_destroy(ut_optiga_util_instance);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
}

void ut_optiga_util_protected_update_fct(void) {
    optiga_util_t *ut_optiga_util_instance = NULL;
    optiga_lib_status_t ut_return_status;
    uint16_t trust_anchor_oid = 0xE0E3;
    uint16_t confidentiality_oid = 0xF1D1;
    uint16_t data_config = 0;

    ut_optiga_util_instance = optiga_util_create(0, optiga_util_callback, NULL);
    assert(ut_optiga_util_instance != NULL);

    /**
     *  Precondition 1 :
     *  Update the metadata and trust anchor in OID 0xE0E3
     */
    ut_return_status = write_trust_anchor(ut_optiga_util_instance, trust_anchor_oid);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    /**
     *  Precondition 2 :
     *  Update the metadata and secret for confidentiality in OID 0xF1D1
     */
    ut_return_status = write_confidentiality_oid(ut_optiga_util_instance, confidentiality_oid);
    assert(OPTIGA_LIB_SUCCESS == ut_return_status);

    for (data_config = 0; data_config < sizeof(optiga_protected_update_data_set)
                                            / sizeof(optiga_protected_update_data_configuration_t);
         data_config++)

    {
        /**
         *  Precondition 3 :
         *  Update the metadata of target OID
         */
        ut_return_status = write_metadata(
            ut_optiga_util_instance,
            optiga_protected_update_data_set[data_config].target_oid,
            (uint8_t *)optiga_protected_update_data_set[data_config].target_oid_metadata,
            (uint8_t)optiga_protected_update_data_set[data_config].target_oid_metadata_length
        );
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);

        /**
         *   Send the manifest using optiga_util_protected_update_start
         */

        optiga_lib_status = OPTIGA_LIB_BUSY;
        ut_return_status = optiga_util_protected_update_start(
            ut_optiga_util_instance,
            optiga_protected_update_data_set[data_config].data_config->manifest_version,
            optiga_protected_update_data_set[data_config].data_config->manifest_data,
            optiga_protected_update_data_set[data_config].data_config->manifest_length
        );

        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
        while (OPTIGA_LIB_BUSY == optiga_lib_status) {
        };
        /* This is a dummy PAL, no chip exists */
        // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

        if (NULL
            != optiga_protected_update_data_set[data_config].data_config->continue_fragment_data) {
            /**
             *   Send the first fragment using optiga_util_protected_update_continue
             */
            optiga_lib_status = OPTIGA_LIB_BUSY;
            ut_return_status = optiga_util_protected_update_continue(
                ut_optiga_util_instance,
                optiga_protected_update_data_set[data_config].data_config->continue_fragment_data,
                optiga_protected_update_data_set[data_config].data_config->continue_fragment_length
            );

            // assert(OPTIGA_LIB_SUCCESS == ut_return_status);
            // while (OPTIGA_LIB_BUSY == optiga_lib_status){ };
            /* This is a dummy PAL, no chip exists */
            // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);
        }

        /**
         *   Send the last fragment using optiga_util_protected_update_final
         */
        optiga_lib_status = OPTIGA_LIB_BUSY;
        ut_return_status = optiga_util_protected_update_final(
            ut_optiga_util_instance,
            optiga_protected_update_data_set[data_config].data_config->final_fragment_data,
            optiga_protected_update_data_set[data_config].data_config->final_fragment_length
        );

        // assert(OPTIGA_LIB_SUCCESS == ut_return_status);
        // while (OPTIGA_LIB_BUSY == optiga_lib_status){ };
        /* This is a dummy PAL, no chip exists */
        // assert(OPTIGA_LIB_SUCCESS == optiga_lib_status);

        /**
         *  Revert the version tag of metadata configuration to re-run the protected update examples
         *  Update the metadata of target OID for version tag to be 00
         */
        ut_return_status = write_metadata(
            ut_optiga_util_instance,
            optiga_protected_update_data_set[data_config].target_oid,
            (uint8_t *)reset_version_tag_metadata,
            (uint8_t)sizeof(reset_version_tag_metadata)
        );
        assert(OPTIGA_LIB_SUCCESS == ut_return_status);
    }
}

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    /* optiga_util_create, optiga_util_destroy,
    optiga_util_open_application, optiga_util_close_application
    Unit tests covered.
    */
    ut_optiga_util_open_close_fct();

    /*
    optiga_util_read_data, optiga_util_read_metadata Unit tests covered.
    */
    ut_optiga_util_read_data_metadata_fct();

    /*
    optiga_util_write_data, optiga_util_write_metadata, optiga_util_update_count Unit tests covered.
    */
    ut_optiga_util_write_data_metadata_fct();

    /*
    optiga_util_protected_update_start, optiga_util_protected_update_continue, optiga_util_protected_update_final Unit tests covered.
    */
    ut_optiga_util_protected_update_fct();
}