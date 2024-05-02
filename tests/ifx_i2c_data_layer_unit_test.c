/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_data_layer_unit_test.c
 *
 * \brief   This file implements the infineon i2c data layer unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "ifx_i2c_data_layer_unit_test.h"

/* Event handler creation for the data layer event */
_STATIC_H void ut_ifx_i2c_dl_event_handler(
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

    /* ifx_i2c_context needs to be initialized */
    ifx_i2c_context_t *ut_ifx_i2c_dl_ctx = malloc(sizeof(ifx_i2c_context_t));
    /* p_pal_i2c_ctx needs to be initialized */
    ut_ifx_i2c_dl_ctx->p_pal_i2c_ctx = malloc(sizeof(pal_i2c_t));
    /* PAL needs to be initialized */
    ut_ifx_i2c_dl_ctx->do_pal_init = TRUE;
    /* set a Random SLAVE ADDRESS */
    ut_ifx_i2c_dl_ctx->slave_address = UT_SLAVE_ADDR;

    /* ifx_i2c_dl_init unit test */
    ut_lib_status = ifx_i2c_dl_init(ut_ifx_i2c_dl_ctx, ut_ifx_i2c_dl_event_handler);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_dl_send_frame unit test */
    ut_lib_status = ifx_i2c_dl_send_frame(ut_ifx_i2c_dl_ctx, PL_SEND_FRAMES);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_dl_ctx->dl.tx_buffer_size == PL_SEND_FRAMES);

    /* ifx_i2c_dl_receive_frame unit test */
    ut_ifx_i2c_dl_ctx->dl.state = DL_STATE_IDLE;
    ut_ifx_i2c_dl_ctx->pl.frame_state = PL_STATE_READY;
    ut_lib_status = ifx_i2c_dl_receive_frame(ut_ifx_i2c_dl_ctx);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    return 0;
}