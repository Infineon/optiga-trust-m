/**
 * SPDX-FileCopyrightText: 2019-2024 Infineon Technologies AG
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

pal_status_t
pal_logger_write(void *p_logger_context, const uint8_t *p_log_data, uint32_t log_data_length) {
    uint32_t i;

    for (i = 0; i < log_data_length; i++) {
        printf("%c", *(p_log_data + i));
    }

    return PAL_STATUS_SUCCESS;
}

/**
 * @}
 */
