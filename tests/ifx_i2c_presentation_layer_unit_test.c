/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_presentation_layer_unit_test.c
 *
 * \brief   This file implements the Infineon I2C presentation layer unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "ifx_i2c_presentation_layer_unit_test.h"

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

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    optiga_lib_status_t ut_lib_status;
    uint8_t ut_send_frames[PL_SEND_FRAMES] = {1, 2, 3, 4, 5};
    uint8_t ut_recv_frames[PL_RECV_FRAMES];
    uint16_t ut_recv_length = 0;

    /* ifx_i2c_context needs to be initialized */
    ifx_i2c_context_t *ut_ifx_i2c_prl_ctx = malloc(sizeof(ifx_i2c_context_t));
    /* p_pal_i2c_ctx needs to be initialized */
    ut_ifx_i2c_prl_ctx->p_pal_i2c_ctx = malloc(sizeof(pal_i2c_t));
    /* PAL needs to be initialized */
    ut_ifx_i2c_prl_ctx->do_pal_init = TRUE;
    /* set a Random SLAVE ADDRESS */
    ut_ifx_i2c_prl_ctx->slave_address = UT_SLAVE_ADDR;

    /* ifx_i2c_prl_init unit test */
    ut_lib_status = ifx_i2c_prl_init(ut_ifx_i2c_prl_ctx, ut_ifx_i2c_prl_event_handler);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_prl_transceive unit test */
    ut_lib_status = ifx_i2c_prl_transceive(
        ut_ifx_i2c_prl_ctx,
        ut_send_frames,
        PL_SEND_FRAMES,
        ut_recv_frames,
        &ut_recv_length
    );
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_prl_ctx->prl.actual_payload_length == PL_SEND_FRAMES);

    /* ifx_i2c_prl_close unit test */
    ut_ifx_i2c_prl_ctx->prl.state = PRL_STATE_IDLE;
    ut_ifx_i2c_prl_ctx->manage_context_operation = IFX_I2C_SESSION_CONTEXT_NONE;
    ut_lib_status = ifx_i2c_prl_close(ut_ifx_i2c_prl_ctx, ut_ifx_i2c_prl_event_handler);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
}