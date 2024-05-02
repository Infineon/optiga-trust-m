/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_unit_test.c
 *
 * \brief   This file implements the infineon I2C unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "ifx_i2c_unit_test.h"

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

void ut_register_callback(void *data) {
    // to remove warning for unused parameter
    (void)(data);
}

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    optiga_lib_status_t ut_lib_status;
    uint8_t ut_callback_data = 0;
    uint8_t ut_send_frames[I2C_SEND_FRAMES] = {1, 2, 3, 4, 5};
    uint8_t ut_recv_frames[I2C_RECV_FRAMES];
    uint16_t ut_recv_length = 0;

    /* ifx_i2c_context needs to be initialized */
    ifx_i2c_context_t *ut_ifx_i2c_ctx = malloc(sizeof(ifx_i2c_context_t));
    /* p_pal_i2c_ctx needs to be initialized */
    ut_ifx_i2c_ctx->p_pal_i2c_ctx = malloc(sizeof(pal_i2c_t));
    /* pal_os_event_ctx needs to be initialized */
    ut_ifx_i2c_ctx->pal_os_event_ctx =
        pal_os_event_create(ut_register_callback, (void *)&ut_callback_data);
    /* set a Random SLAVE ADDRESS */
    ut_ifx_i2c_ctx->slave_address = UT_SLAVE_ADDR;

    /* ifx_i2c_open unit test */
    ut_lib_status = ifx_i2c_open(ut_ifx_i2c_ctx);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    /* ifx_i2c_prl_init init needed*/
    ut_lib_status = ifx_i2c_prl_init(ut_ifx_i2c_ctx, ut_ifx_i2c_prl_event_handler);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_set_slave_address Persistant unit test */
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_lib_status =
        ifx_i2c_set_slave_address(ut_ifx_i2c_ctx, UT_SLAVE_ADDR, PL_REG_BASE_ADDR_PERSISTANT);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_set_slave_address Volatile unit test */
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_lib_status =
        ifx_i2c_set_slave_address(ut_ifx_i2c_ctx, UT_SLAVE_ADDR, PL_REG_BASE_ADDR_VOLATILE);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_transceive unit test */
    ut_ifx_i2c_ctx->status = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->prl.state = PRL_STATE_IDLE;
    ut_lib_status = ifx_i2c_transceive(
        ut_ifx_i2c_ctx,
        ut_send_frames,
        I2C_SEND_FRAMES,
        ut_recv_frames,
        &ut_recv_length
    );
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_ctx->prl.actual_payload_length == I2C_SEND_FRAMES);

    /* ifx_i2c_reset Cold cold unit test */
    ut_ifx_i2c_ctx->status = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_lib_status = ifx_i2c_reset(ut_ifx_i2c_ctx, IFX_I2C_COLD_RESET);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_reset soft unit test */
    ut_ifx_i2c_ctx->status = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_lib_status = ifx_i2c_reset(ut_ifx_i2c_ctx, IFX_I2C_SOFT_RESET);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_reset warm unit test */
    ut_ifx_i2c_ctx->status = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->state = IFX_I2C_STATE_IDLE;
    ut_lib_status = ifx_i2c_reset(ut_ifx_i2c_ctx, IFX_I2C_WARM_RESET);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_close unit test */
    ut_ifx_i2c_ctx->status = IFX_I2C_STATE_IDLE;
    ut_ifx_i2c_ctx->prl.state = PRL_STATE_IDLE;
    ut_ifx_i2c_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_lib_status = ifx_i2c_close(ut_ifx_i2c_ctx);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
}