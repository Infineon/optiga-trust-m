/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_i2c.c
 *
 * \brief   This file implements the platform abstraction layer(pal) APIs for I2C.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#include "pal_i2c.h"

#define PAL_I2C_MASTER_MAX_BITRATE (400U)

static volatile uint32_t g_entry_count = 0;
static pal_i2c_t *gp_pal_i2c_current_ctx;

static pal_status_t pal_i2c_acquire(const void *p_i2c_context) {
    // To avoid compiler errors/warnings. This context might be used by a target
    // system to implement a proper mutex handling
    (void)p_i2c_context;

    if (0 == g_entry_count) {
        g_entry_count++;
        if (1 == g_entry_count) {
            return PAL_STATUS_SUCCESS;
        }
    }
    return PAL_STATUS_FAILURE;
}

static void pal_i2c_release(const void *p_i2c_context) {
    // To avoid compiler errors/warnings. This context might be used by a target
    // system to implement a proper mutex handling
    (void)p_i2c_context;

    g_entry_count = 0;
}

void invoke_upper_layer_callback(const pal_i2c_t *p_pal_i2c_ctx, optiga_lib_status_t event) {
    upper_layer_callback_t upper_layer_handler;

    upper_layer_handler = (upper_layer_callback_t)p_pal_i2c_ctx->upper_layer_event_handler;

    upper_layer_handler(p_pal_i2c_ctx->p_upper_layer_ctx, event);

    // Release I2C Bus
    pal_i2c_release(p_pal_i2c_ctx->p_upper_layer_ctx);
}

// !!!OPTIGA_LIB_PORTING_REQUIRED
// The next 5 functions are required only in case you have interrupt based i2c implementation
void i2c_master_end_of_transmit_callback(void) {
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

void i2c_master_end_of_receive_callback(void) {
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
}

void i2c_master_error_detected_callback(void) {
    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_ERROR);
}

void i2c_master_nack_received_callback(void) {
    i2c_master_error_detected_callback();
}

void i2c_master_arbitration_lost_callback(void) {
    i2c_master_error_detected_callback();
}

pal_status_t pal_i2c_init(const pal_i2c_t *p_i2c_context) {
    (void)p_i2c_context;
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_deinit(const pal_i2c_t *p_i2c_context) {
    (void)p_i2c_context;
    return PAL_STATUS_SUCCESS;
}

pal_status_t pal_i2c_write(pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length) {
    pal_status_t status = PAL_STATUS_FAILURE;

    // Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) {
        gp_pal_i2c_current_ctx = p_i2c_context;

        // Invoke the low level i2c master driver API to write to the bus
        //  !!!OPTIGA_LIB_PORTING_REQUIRED
        if (!foo_i2c_write(p_i2c_context->p_i2c_hw_config,
                          (p_i2c_context->slave_address << 1),
                          p_data,
                          length,
                          )
        {
            // If I2C Master fails to invoke the write operation, invoke upper layer event handler with error.

            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler)
            )(p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_ERROR);

            // Release I2C Bus
            pal_i2c_release((void *)p_i2c_context);
        }
        else
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            /**
             * Infineon I2C Protocol is a polling based protocol, if foo_i2c_write will fail it will be reported to the
             * upper layers by calling
             * (p_i2c_context->upper_layer_event_handler))(p_i2c_context->p_upper_layer_ctx , PAL_I2C_EVENT_ERROR);
             * If the function foo_i2c_write() will succedd then two options are possible
             * 1. if foo_i2c_write() is interrupt based, then you need to configure interrupts in the function
             *    pal_i2c_init() so that on a succesfull transmit interrupt the callback i2c_master_end_of_transmit_callback(),
             *    in case of successfull receive i2c_master_end_of_receive_callback() callback
             *    in case of not acknowedged, arbitration lost, generic error i2c_master_nack_received_callback() or
             *    i2c_master_arbitration_lost_callback()
             * 2. If foo_i2c_write() is a blocking function which will return either ok or failure after transmitting data
             *    you can handle this case directly here and call
             *    invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
             *
             */
            status = PAL_STATUS_SUCCESS;
        }
    } else {
        status = PAL_STATUS_I2C_BUSY;
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler)
        )(p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_BUSY);
    }
    return status;
}

pal_status_t pal_i2c_read(pal_i2c_t *p_i2c_context, uint8_t *p_data, uint16_t length) {
    pal_status_t status = PAL_STATUS_FAILURE;

    // Acquire the I2C bus before read/write
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) {
        gp_pal_i2c_current_ctx = p_i2c_context;

        // Invoke the low level i2c master driver API to read from the bus
        if (foo_i2c_read(p_i2c_context->p_i2c_hw_config,
                          (p_i2c_context->slave_address << 1),
                          p_data,
                          length,
                          )
        {
            // If I2C Master fails to invoke the read operation, invoke upper layer event handler with error.
            ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler)
            )(p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_ERROR);

            // Release I2C Bus
            pal_i2c_release((void *)p_i2c_context);
        }
        else
        {
            // !!!OPTIGA_LIB_PORTING_REQUIRED
            /**
             * Similar to the foo_i2c_write() case you can directly call
             * invoke_upper_layer_callback(gp_pal_i2c_current_ctx, PAL_I2C_EVENT_SUCCESS);
             * if you have blocking (non-interrupt) i2c calls
             */
            status = PAL_STATUS_SUCCESS;
        }
    } else {
        status = PAL_STATUS_I2C_BUSY;
        ((upper_layer_callback_t)(p_i2c_context->upper_layer_event_handler)
        )(p_i2c_context->p_upper_layer_ctx, PAL_I2C_EVENT_BUSY);
    }
    return status;
}

pal_status_t pal_i2c_set_bitrate(const pal_i2c_t *p_i2c_context, uint16_t bitrate) {
    pal_status_t return_status = PAL_STATUS_FAILURE;
    optiga_lib_status_t event = PAL_I2C_EVENT_ERROR;

    // Acquire the I2C bus before setting the bitrate
    if (PAL_STATUS_SUCCESS == pal_i2c_acquire(p_i2c_context)) {
        // If the user provided bitrate is greater than the I2C master hardware maximum supported value,
        // set the I2C master to its maximum supported value.
        if (bitrate > PAL_I2C_MASTER_MAX_BITRATE) {
            bitrate = PAL_I2C_MASTER_MAX_BITRATE;
        }
        // !!!OPTIGA_LIB_PORTING_REQUIRED
        // This function is NOT absolutely required for the correct working of the system, but it's recommended
        // to implement it, though
        if (foo_i2c_set_baudrate(bitrate)
        {
            return_status = PAL_STATUS_FAILURE;
        }
        else
        {
            return_status = PAL_STATUS_SUCCESS;
            event = PAL_I2C_EVENT_SUCCESS;
        }
    } else {
        return_status = PAL_STATUS_I2C_BUSY;
        event = PAL_I2C_EVENT_BUSY;
    }
    if (0 != p_i2c_context->upper_layer_event_handler) {
        ((callback_handler_t)(p_i2c_context->upper_layer_event_handler)
        )(p_i2c_context->p_upper_layer_ctx, event);
    }
    // Release I2C Bus if its acquired
    if (PAL_STATUS_I2C_BUSY != return_status) {
        pal_i2c_release((void *)p_i2c_context);
    }
    return return_status;
}

/**
 * @}
 */
