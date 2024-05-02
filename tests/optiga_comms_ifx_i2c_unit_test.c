/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_comms_ifx_i2c_unit_test.c
 *
 * \brief   This file implements the OPTIGA comms infineon I2C unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_comms_ifx_i2c_unit_test.h"

/* Event handler creation for the Transport layer event */
_STATIC_H void ut_ifx_i2c_prl_event_handler(
    ifx_i2c_context_t *p_ctx,
    optiga_lib_status_t event,
    const uint8_t *p_data,
    uint16_t data_len
) {
    /* to remove warning for unused parameter */
    (void)(p_ctx);
    (void)(event);
    (void)(p_data);
    (void)(data_len);
}

void ut_register_callback(void *p_ctx, optiga_lib_status_t event) {
    // to remove warning for unused parameter
    (void)(p_ctx);
    (void)(event);
}

void ut_pal_os_register_callback(void *data) {
    // to remove warning for unused parameter
    (void)(data);
}

void optiga_comms_ifx_i2c_unit_test() {
    optiga_comms_t *ut_optiga_comms;
    optiga_lib_status_t ut_optiga_lib_status;
    uint8_t ut_callback_data = 0;
    uint16_t ut_msg_length = 5;
    uint16_t ut_msg_recv_length = 0;
    const uint8_t ut_tx_data[5] = {1, 2, 3, 4, 5};
    uint8_t ut_rx_data[5] = {0};

    /* optiga_comms_create check*/
    ut_optiga_comms = optiga_comms_create(ut_register_callback, (void *)&ut_callback_data);
    assert(ut_optiga_comms != NULL);
    assert(ut_optiga_comms->instance_init_state == TRUE);

    /* optiga_comms_destroy check*/
    optiga_comms_destroy(ut_optiga_comms);
    assert(ut_optiga_comms->instance_init_state == FALSE);

    /* optiga_comms_create again*/
    ut_optiga_comms = optiga_comms_create(ut_register_callback, (void *)&ut_callback_data);
    assert(ut_optiga_comms != NULL);
    assert(ut_optiga_comms->instance_init_state == TRUE);

    /* optiga_comms_set_callback_handler check
     *  Nothing to be checked here, just for coverage
     */
    optiga_comms_set_callback_handler(ut_optiga_comms, ut_register_callback);

    /* optiga_comms_set_callback_context check
     *  Nothing to be checked here, just for coverage
     */
    optiga_comms_set_callback_context(ut_optiga_comms, (void *)&ut_callback_data);

    /* optiga_comms_open check*/
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ifx_i2c_context_t *ut_comms_ctx = (ifx_i2c_context_t *)ut_optiga_comms->p_comms_ctx;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    /* pal_os_event_ctx needs to be initialized */
    ut_optiga_comms->p_pal_os_event_ctx =
        pal_os_event_create(ut_pal_os_register_callback, (void *)&ut_callback_data);
    ut_optiga_lib_status = optiga_comms_open(ut_optiga_comms);
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);

    /* ifx_i2c_prl_init init needed*/
    ut_comms_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_comms_ctx->do_pal_init = TRUE;
    ut_optiga_lib_status = ifx_i2c_prl_init(ut_comms_ctx, ut_ifx_i2c_prl_event_handler);
    assert(ut_optiga_lib_status == IFX_I2C_STACK_SUCCESS);

    /* optiga_comms_transceive check*/
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->state = IFX_I2C_STATE_IDLE;
    ut_optiga_lib_status = optiga_comms_transceive(
        ut_optiga_comms,
        ut_tx_data,
        ut_msg_length,
        ut_rx_data,
        &ut_msg_recv_length
    );
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);

    /* optiga_comms_close check*/
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->prl.state = PRL_STATE_IDLE;
    ut_comms_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_comms->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_lib_status = optiga_comms_close(ut_optiga_comms);
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);

    sleep(1);

    /* optiga_comms_reset check
     * Cold reset. Both reset pin and VDD pin are toggled low and then high
     */
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->state = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_comms->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_lib_status = optiga_comms_reset(ut_optiga_comms, IFX_I2C_COLD_RESET);
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);

    sleep(1);

    /* optiga_comms_reset check
     * Soft reset. 0x0000 is written to IFX-I2C Soft reset register
     */
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->state = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_comms->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_lib_status = optiga_comms_reset(ut_optiga_comms, IFX_I2C_SOFT_RESET);
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);

    sleep(1);

    /* optiga_comms_reset check
     * Warm reset. Only reset pin is toggled low and then high
     */
    ut_optiga_comms->state = OPTIGA_COMMS_FREE;
    ut_comms_ctx->status = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->state = IFX_I2C_STATE_IDLE;
    ut_comms_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_comms->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_optiga_lib_status = optiga_comms_reset(ut_optiga_comms, IFX_I2C_WARM_RESET);
    assert(ut_optiga_lib_status == OPTIGA_COMMS_SUCCESS);
}

int main(int argc, char **argv) {
    /* pal_crypt_version unit tests function */
    optiga_comms_ifx_i2c_unit_test();

    return 0;
}