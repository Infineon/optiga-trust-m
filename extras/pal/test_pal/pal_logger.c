/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file    pal_logger.c
 *
 * \brief   This file provides the prototypes declarations for pal logger.
 *
 * \ingroup grPAL
 *
 * @{
 */

#include "pal_logger.h"

pal_logger_t logger_console = {.logger_config_ptr = NULL, .logger_rx_flag = 0, .logger_tx_flag = 1};

/* PAL Logger Write function */
pal_status_t
pal_logger_write(void *p_logger_context, const uint8_t *p_log_data, uint32_t log_data_length) {
    /* Logger context not needed */
    (void)(p_logger_context);

    int p_log_data_pos;
    for (p_log_data_pos = 0; p_log_data_pos < log_data_length; p_log_data_pos++) {
        printf("%c", *(p_log_data + p_log_data_pos));
    }

    return PAL_STATUS_SUCCESS;
}

/**
 * @}
 */
