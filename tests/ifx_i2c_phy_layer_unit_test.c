/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file ifx_i2c_phy_layer_unit_test.c
 *
 * \brief   This file implements the infineon I2C physical Layer unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "ifx_i2c_phy_layer_unit_test.h"

/* Event handler creation for the physical layer event */
_STATIC_H void ut_ifx_i2c_pl_event_handler(
    ifx_i2c_context_t *p_ctx,
    optiga_lib_status_t event,
    const uint8_t *p_data,
    uint16_t data_len
) {
    // to remove warning for unused parameter
    (void)(p_ctx);
    (void)(event);
    (void)(p_data);
    (void)(data_len);
}

int main(int argc, char **argv) {
    // to remove warning for unused parameter
    (void)(argc);
    (void)(argv);

    optiga_lib_status_t ut_lib_status;
    uint8_t ut_send_frames[PL_SEND_FRAMES] = {1, 2, 3, 4, 5};

    /* ifx_i2c_context needs to be initialized */
    ifx_i2c_context_t *ut_ifx_i2c_pl_ctx = malloc(sizeof(ifx_i2c_context_t));
    /* p_pal_i2c_ctx needs to be initialized */
    ut_ifx_i2c_pl_ctx->p_pal_i2c_ctx = malloc(sizeof(pal_i2c_t));
    /* PAL needs to be initialized */
    ut_ifx_i2c_pl_ctx->do_pal_init = TRUE;
    /* set a Random SLAVE ADDRESS */
    ut_ifx_i2c_pl_ctx->slave_address = UT_SLAVE_ADDR;

    /* ifx_i2c_pl_init unit test */
    ut_lib_status = ifx_i2c_pl_init(ut_ifx_i2c_pl_ctx, ut_ifx_i2c_pl_event_handler);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_pl_send_frame unit test */
    ut_lib_status = ifx_i2c_pl_send_frame(ut_ifx_i2c_pl_ctx, ut_send_frames, PL_SEND_FRAMES);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_pl_ctx->pl.tx_frame_len == PL_SEND_FRAMES);

    /* ifx_i2c_pl_receive_frame unit test */
    ut_ifx_i2c_pl_ctx->pl.frame_state = PL_STATE_READY;
    ut_lib_status = ifx_i2c_pl_receive_frame(ut_ifx_i2c_pl_ctx);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);

    /* ifx_i2c_pl_write_slave_address unit test
     *   Using volatile address
     */
    ut_lib_status =
        ifx_i2c_pl_write_slave_address(ut_ifx_i2c_pl_ctx, UT_SLAVE_ADDR, PL_REG_BASE_ADDR_VOLATILE);
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_pl_ctx->pl.buffer[ADDRESS_OFFSET] == UT_SLAVE_ADDR);
    assert(ut_ifx_i2c_pl_ctx->pl.buffer[MODE_OFFSET] == PL_REG_BASE_ADDR_VOLATILE);

    /* ifx_i2c_pl_write_slave_address unit test
     *   Using persistant address
     */
    ut_lib_status = ifx_i2c_pl_write_slave_address(
        ut_ifx_i2c_pl_ctx,
        UT_SLAVE_ADDR,
        PL_REG_BASE_ADDR_PERSISTANT
    );
    assert(ut_lib_status == IFX_I2C_STACK_SUCCESS);
    assert(ut_ifx_i2c_pl_ctx->pl.buffer[ADDRESS_OFFSET] == UT_SLAVE_ADDR);
    assert(ut_ifx_i2c_pl_ctx->pl.buffer[MODE_OFFSET] == PL_REG_BASE_ADDR_PERSISTANT);

    return 0;
}