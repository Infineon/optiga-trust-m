/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_unit_test.c
 *
 * \brief   This file implements the PAL unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "pal_unit_test.h"

pal_i2c_t ut_pal_i2c = {
    /// Pointer to I2C master platform specific context
    NULL,
    // Pointer to store the callers context information
    NULL,
    /// Pointer to store the callers handler
    NULL,
    /// I2C slave address
    0,
};

void pal_os_timer_unit_tests(void) {
    /* PAL Timer init check*/
    pal_status_t pal_status = pal_timer_init();
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_os_timer_get_time_in_microseconds check*/
    uint32_t tick_count_in_us = pal_os_timer_get_time_in_microseconds();
    assert(tick_count_in_us != 0);

    /* pal_os_timer_get_time_in_milliseconds check*/
    uint32_t tick_count_in_ms = pal_os_timer_get_time_in_milliseconds();
    assert(tick_count_in_ms != 0);

    /* pal_os_timer_delay_in_milliseconds check
    * We add 100 millesecond delay and we read again the tick count
    # The new value should be minimum 100 millisencond higher
    */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    pal_os_timer_delay_in_milliseconds(2000);
    gettimeofday(&tv2, NULL);
    assert(tv2.tv_sec >= tv1.tv_sec + 2);

    /* PAL Timer deinit check*/
    pal_status = pal_timer_deinit();
    assert(pal_status == PAL_STATUS_SUCCESS);
}

void pal_os_lock_unit_tests(void) {
    pal_os_lock_t ut_os_lock;
    uint8_t lock_type = 2;
    pal_status_t pal_status;

    /* pal_os_lock_destroy check
     * lock type should be as specified in lock create function
     */
    pal_os_lock_create(&ut_os_lock, lock_type);
    assert(ut_os_lock.type == lock_type);

    /* pal_os_lock_destroy check
     * lock type should be rolled back to 0
     */
    pal_os_lock_destroy(&ut_os_lock);
    assert(ut_os_lock.type == 0);

    /* pal_os_lock_acquire check
     * ut_os_lock is locked, this should return a success and lock status should be at 1
     */
    pal_status = pal_os_lock_acquire(&ut_os_lock);
    assert(pal_status == PAL_STATUS_SUCCESS);
    assert(ut_os_lock.lock == 1);

    /* pal_os_lock_acquire check
     * ut_os_lock is already locked, this should return a failure and lock status should stay at 1
     */
    pal_status = pal_os_lock_acquire(&ut_os_lock);
    assert(pal_status == PAL_STATUS_FAILURE);
    assert(ut_os_lock.lock == 1);

    /* pal_os_lock_release check
     * lock should be set back to 0 after lock
     */
    pal_os_lock_release(&ut_os_lock);
    assert(ut_os_lock.lock == 0);

    /* pal_os_lock_enter_critical_section check
     *  Nothing to be checked here, just for coverage
     */
    pal_os_lock_enter_critical_section();

    /* pal_os_lock_exit_critical_section check
     *  Nothing to be checked here, just for coverage
     */
    pal_os_lock_exit_critical_section();
}

void pal_logger_unit_tests(void) {
    pal_status_t pal_status;
    char_t *ut_log_string = "Unit test logger";

    /* pal_logger_write check*/
    pal_status = pal_logger_write(NULL, (const uint8_t *)ut_log_string, strlen(ut_log_string));
    assert(pal_status == PAL_STATUS_SUCCESS);
}

void pal_i2c_unit_tests(void) {
    pal_status_t pal_status;
    uint8_t ut_data[5] = {1, 2, 3, 4, 5};
    uint8_t ut_data_len = 5;

    /* pal_i2c_init check*/
    pal_status = pal_i2c_init(&ut_pal_i2c);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_i2c_deinit check*/
    pal_status = pal_i2c_deinit(&ut_pal_i2c);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_i2c_write check*/
    pal_status = pal_i2c_write(&ut_pal_i2c, ut_data, ut_data_len);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_i2c_read check*/
    pal_status = pal_i2c_read(&ut_pal_i2c, ut_data, ut_data_len);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_i2c_set_bitrate check*/
    pal_status = pal_i2c_set_bitrate(&ut_pal_i2c, 10);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_i2c_set_bitrate higher than max bitrate check*/
    pal_status = pal_i2c_set_bitrate(&ut_pal_i2c, PAL_I2C_MASTER_MAX_BITRATE_OVERFLOW);
    assert(pal_status == PAL_STATUS_SUCCESS);
}

void pal_gpio_unit_tests(void) {
    pal_status_t pal_status;

    /* pal_gpio_init check*/
    pal_status = pal_gpio_init(NULL);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_gpio_deinit check*/
    pal_status = pal_gpio_deinit(NULL);
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* pal_gpio_set_high check
     *  Nothing to be checked here, just for coverage
     */
    pal_gpio_set_high(NULL);

    /* pal_gpio_set_low check
     *  Nothing to be checked here, just for coverage
     */
    pal_gpio_set_low(NULL);
}

void pal_os_datastore_unit_tests(void) {
    pal_status_t pal_status;
    uint8_t ut_buffer[5] = {1, 2, 3, 4, 5};
    uint16_t ut_buffer_len = 5;
    uint8_t ut_buffer_read[5] = {0, 0, 0, 0, 0};
    uint16_t ut_buffer_read_len = 0;

    /* pal_os_datastore_write and pal_os_datastore_read check using datastore OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID
     * The buffer ut_buffer is written then read to check if the data is correctly stored.
     */
    pal_status =
        pal_os_datastore_write(OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID, ut_buffer, ut_buffer_len);
    assert(pal_status == PAL_STATUS_SUCCESS);

    pal_status = pal_os_datastore_read(
        OPTIGA_PLATFORM_BINDING_SHARED_SECRET_ID,
        ut_buffer_read,
        &ut_buffer_read_len
    );
    assert(pal_status == PAL_STATUS_SUCCESS);
    assert(ut_buffer_read_len == ut_buffer_len);
    for (int itr = 0; itr < 5; itr++)
        assert(ut_buffer_read[itr] == ut_buffer[itr]);

    /* erase ut_buffer_read to use it for the next check */
    memset(ut_buffer_read, 0, ut_buffer_read_len);

    /* pal_os_datastore_write and pal_os_datastore_read check using datastore OPTIGA_COMMS_MANAGE_CONTEXT_ID
     * The buffer ut_buffer is written then read to check if the data is correctly stored.
     */
    pal_status = pal_os_datastore_write(OPTIGA_COMMS_MANAGE_CONTEXT_ID, ut_buffer, ut_buffer_len);
    assert(pal_status == PAL_STATUS_SUCCESS);

    pal_status =
        pal_os_datastore_read(OPTIGA_COMMS_MANAGE_CONTEXT_ID, ut_buffer_read, &ut_buffer_read_len);
    assert(pal_status == PAL_STATUS_SUCCESS);
    assert(ut_buffer_read_len == ut_buffer_len);
    for (int itr = 0; itr < 5; itr++)
        assert(ut_buffer_read[itr] == ut_buffer[itr]);

    /* erase ut_buffer_read to use it for the next check */
    memset(ut_buffer_read, 0, ut_buffer_read_len);

    /* pal_os_datastore_write and pal_os_datastore_read check using datastore OPTIGA_HIBERNATE_CONTEXT_ID
     * The buffer ut_buffer is written then read to check if the data is correctly stored.
     */
    pal_status = pal_os_datastore_write(OPTIGA_HIBERNATE_CONTEXT_ID, ut_buffer, ut_buffer_len);
    assert(pal_status == PAL_STATUS_SUCCESS);

    pal_status =
        pal_os_datastore_read(OPTIGA_HIBERNATE_CONTEXT_ID, ut_buffer_read, &ut_buffer_read_len);
    assert(pal_status == PAL_STATUS_SUCCESS);
    assert(ut_buffer_read_len == ut_buffer_len);
    for (int itr = 0; itr < 5; itr++)
        assert(ut_buffer_read[itr] == ut_buffer[itr]);

    /* pal_os_datastore_write check using wrong datastore ID, it should return failure.*/
    pal_status = pal_os_datastore_write(0x00, ut_buffer, ut_buffer_len);
    assert(pal_status == PAL_STATUS_FAILURE);
    /* pal_os_datastore_read check using wrong datastore ID, it should return failure.*/
    pal_status = pal_os_datastore_read(0x00, ut_buffer_read, &ut_buffer_read_len);
    assert(pal_status == PAL_STATUS_FAILURE);
}

void ut_register_callback(void *data) {
    // to remove warning for unused parameter
    (void)(data);
    printf(" Callback called ");
}

void pal_os_event_unit_tests(void) {
    pal_os_event_t *ut_pal_os_event;
    uint8_t ut_callback_data = 0;

    /* pal_os_event_create check*/
    ut_pal_os_event = pal_os_event_create(ut_register_callback, (void *)&ut_callback_data);
    assert(ut_pal_os_event->is_event_triggered == TRUE);

    /* pal_os_event_start check*/
    pal_os_event_start(ut_pal_os_event, ut_register_callback, (void *)&ut_callback_data);
    assert(ut_pal_os_event->is_event_triggered == TRUE);

    /* pal_os_event_stop check*/
    pal_os_event_stop(ut_pal_os_event);
    assert(ut_pal_os_event->is_event_triggered == FALSE);

    /* pal_os_event_register_callback_oneshot check
     *  Nothing to be checked here, just for coverage
     */
    pal_os_event_register_callback_oneshot(
        ut_pal_os_event,
        ut_register_callback,
        (void *)&ut_callback_data,
        100
    );

    /* pal_os_event_trigger_registered_callback check
     *  Nothing to be checked here, just for coverage
     */
    pal_os_event_trigger_registered_callback();

    /* pal_os_event_destroy check
     *  Nothing to be checked here, just for coverage
     */
    pal_os_event_destroy(ut_pal_os_event);
}

int main(int argc, char **argv) {
    // to remove warning for unused parameter
    (void)(argc);
    (void)(argv);

    /* PAL init check*/
    pal_status_t pal_status = pal_init();
    assert(pal_status == PAL_STATUS_SUCCESS);

    /* Memory unit tests to cover malloc */
    uint8_t *pal_os_mem = (uint8_t *)pal_os_malloc(5 * sizeof(uint8_t));
    assert(pal_os_mem != NULL);

    /* pal_os_timer unit tests function */
    pal_os_timer_unit_tests();

    /* pal_os_lock unit tests function */
    pal_os_lock_unit_tests();

    /* pal_logger unit tests function */
    pal_logger_unit_tests();

    /* pal_i2c unit tests function */
    pal_i2c_unit_tests();

    /* pal_gpio unit tests function */
    pal_gpio_unit_tests();

    /* pal_os_datastore unit tests function */
    pal_os_datastore_unit_tests();

    /* pal_os_event unit tests function */
    pal_os_event_unit_tests();

    /* PAL deinit check*/
    pal_status = pal_deinit();
    assert(pal_status == PAL_STATUS_SUCCESS);

    return 0;
}