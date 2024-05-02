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

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "stdio.h"

// lint --e{552,714} suppress "Accessed by user of this structure"
pal_logger_t logger_console = {.logger_config_ptr = NULL, .logger_rx_flag = 1, .logger_tx_flag = 1};
///
#define CONSOLE_PORT 0

pal_status_t pal_logger_init(void *p_logger_context) {
    pal_status_t return_status = PAL_STATUS_SUCCESS;

    return return_status;
}
pal_status_t
pal_logger_write(void *p_logger_context, const uint8_t *p_log_data, uint32_t log_data_length) {
    pal_status_t return_status = PAL_STATUS_SUCCESS;

    printf("%s", p_log_data);
    memset(p_log_data, 0x00, log_data_length);
    // ESP_LOGI ("", "%s\r\n", p_log_data);
    return ((pal_status_t)return_status);
}

pal_status_t
pal_logger_read(void *p_logger_context, uint8_t *p_log_data, uint32_t log_data_length) {
    pal_status_t return_status = PAL_STATUS_SUCCESS;

    return ((pal_status_t)return_status);
}
/**
 * @}
 */
