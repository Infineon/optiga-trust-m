/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file example_optiga_util_update_count.c
 *
 * \brief   This file provides the example to update the counter object value in OPTIGA using
 *          #optiga_util_update_count.
 *
 * \ingroup grOptigaExamples
 *
 * @{
 */

#include "optiga_example.h"
#include "optiga_util.h"

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
extern void example_optiga_init(void);
extern void example_optiga_deinit(void);
#endif

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
 * Callback when optiga_util_xxxx operation is completed asynchronously
 */
static volatile optiga_lib_status_t optiga_lib_status;
// lint --e{818} suppress "argument "context" is not used in the sample provided"
static void optiga_util_callback(void *context, optiga_lib_status_t return_status) {
    optiga_lib_status = return_status;
    if (NULL != context) {
        // callback to upper layer here
    }
}

/**
 * The below example demonstrates update counter functionalities
 *
 * Example for #optiga_util_update_count
 *
 */
void example_optiga_util_update_count(void) {
    uint32_t time_taken = 0;
    uint16_t optiga_counter_oid;
    uint8_t offset;

    optiga_lib_status_t return_status = !OPTIGA_LIB_SUCCESS;
    optiga_util_t *me = NULL;

    do {
#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
        /**
         * Open the application on OPTIGA which is a precondition to perform any other operations
         * using optiga_util_open_application
         */
        example_optiga_init();
#endif  // OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

        OPTIGA_EXAMPLE_LOG_MESSAGE(__FUNCTION__);
        /**
         * 1. Create OPTIGA Util Instance
         */
        me = optiga_util_create(0, optiga_util_callback, NULL);
        if (NULL == me) {
            break;
        }

        /**
         * Pre-condition
         * Any data object can be converted to counter data object by changing metadata as mentioned below:
         * - As precondition  write access should be always or LCSO < OPERATIONAL
         * - Write metadata as "0xD0, 0x01, 0x01" using #optiga_util_write_metadata
         */

        /**
         * Write default count and threshold value to counter data object (e.g. E120)
         * using optiga_util_write_data.
         *
         * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
         * in order to correctly update the used length of the object.
         */
        optiga_counter_oid = 0xE120;
        offset = 0x00;
        optiga_lib_status = OPTIGA_LIB_BUSY;

        START_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = optiga_util_write_data(
            me,
            optiga_counter_oid,
            OPTIGA_UTIL_ERASE_AND_WRITE,
            offset,
            initial_counter_object_data,
            sizeof(initial_counter_object_data)
        );

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        // In this example, the counter is update by 5 and the final count would be 15
        optiga_lib_status = OPTIGA_LIB_BUSY;
        return_status = optiga_util_update_count(me, optiga_counter_oid, 0x05);

        WAIT_AND_CHECK_STATUS(return_status, optiga_lib_status);

        READ_PERFORMANCE_MEASUREMENT(time_taken);

        return_status = OPTIGA_LIB_SUCCESS;
    } while (FALSE);
    OPTIGA_EXAMPLE_LOG_STATUS(return_status);

#ifndef OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY
    /**
     * Close the application on OPTIGA after all the operations are executed
     * using optiga_util_close_application
     */
    example_optiga_deinit();
#endif  // OPTIGA_INIT_DEINIT_DONE_EXCLUSIVELY

    OPTIGA_EXAMPLE_LOG_PERFORMANCE_VALUE(time_taken, return_status);

    if (me) {
        // Destroy the instance after the completion of usecase if not required.
        return_status = optiga_util_destroy(me);
        if (OPTIGA_LIB_SUCCESS != return_status) {
            // lint --e{774} suppress This is a generic macro
            OPTIGA_EXAMPLE_LOG_STATUS(return_status);
        }
    }
}

/**
 * @}
 */
