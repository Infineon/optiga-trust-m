/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file main.c
 *
 * \brief   This file is the main file.
 *
 * \ingroup  grProtectedUpdateTool
 *
 * @{
 */

#include <stdio.h>
#include <stdlib.h>

#include "pal\pal_logger.h"
#include "pal\pal_os_memory.h"
#include "protected_update_data_set.h"
#include "protected_update_data_set_version.h"
#include "user_input_parser.h"

int32_t main(int32_t argc, int8_t *argv[]) {
    int32_t exit_status = 1;
    manifest_t *p_manifest = NULL;
    int8_t buffer[100];
    protected_update_data_set_d cbor_manifest = {0};
    cbor_manifest.data = NULL;
    cbor_manifest.fragments = NULL;

    do {
        p_manifest = (manifest_t *)pal_os_calloc(1, sizeof(manifest_t));
        if (NULL == p_manifest) {
            break;
        }

        sprintf(buffer, "Tool Version : %s\n", PROTECTED_UPDATE_VERSION);
        pal_logger_print_message(buffer);

        if (0 != tool_get_user_inputs(argc, argv)) {
            break;
        }
        if (0 != tool_set_user_inputs(p_manifest)) {
            break;
        }

        // Generate fragements
        if (0 != protected_update_create_fragments(p_manifest, &cbor_manifest)) {
            pal_logger_print_message("Error while creating fragments\n");
            break;
        }

        // Generate Manifest
        if (0 != protected_update_create_manifest(p_manifest, &cbor_manifest)) {
            pal_logger_print_message("Error while creating manifest\n");
            break;
        }
        // Print the output dataset
        protected_update_print_output_dataset(&cbor_manifest);

        exit_status = 0;
    } while (0);
    // free memory
    if (NULL != cbor_manifest.data) {
        pal_os_free(cbor_manifest.data);
    }
    if (NULL != cbor_manifest.fragments) {
        pal_os_free(cbor_manifest.fragments);
    }
    if (NULL != p_manifest) {
        pal_os_free(p_manifest);
    }

    return exit_status;
}

/**
 * @}
 */
