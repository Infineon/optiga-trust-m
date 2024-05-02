/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * @{
 */

#include "optiga_crypt.h"
#include "optiga_util.h"
#include "pal_os_timer.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include "optiga_crypt.h"
#include "optiga_lib_common.h"
#include "optiga_util.h"

optiga_lib_status_t crypt_event_completed_status;

// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_crypt_event_completed(void *context, optiga_lib_status_t return_status) {
    crypt_event_completed_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {
    int error = 0;
    optiga_crypt_t *me = NULL;
    optiga_lib_status_t command_queue_status = OPTIGA_CRYPT_ERROR;

    if (olen != NULL) {
        me = optiga_crypt_create(0, optiga_crypt_event_completed, NULL);
        if (NULL == me) {
            // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
            error = -0x0034;
        } else {
            crypt_event_completed_status = OPTIGA_LIB_BUSY;
            command_queue_status = optiga_crypt_random(me, OPTIGA_RNG_TYPE_TRNG, output, len);
            if (command_queue_status != OPTIGA_LIB_SUCCESS) {
                // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
                error = -0x0034;
            }

            if (!error) {
                while (OPTIGA_LIB_BUSY == crypt_event_completed_status) {
                    pal_os_timer_delay_in_milliseconds(5);
                }

                if (crypt_event_completed_status != OPTIGA_LIB_SUCCESS) {
                    // MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE
                    error = -0x0034;
                } else {
                    *olen = len;
                }
            }
        }
        optiga_crypt_destroy(me);  // CCW => Seems to be missing?
    }

    return error;
}

#endif
