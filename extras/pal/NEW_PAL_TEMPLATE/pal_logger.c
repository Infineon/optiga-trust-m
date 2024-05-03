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

pal_status_t pal_logger_init(void *p_logger_context) {
    pal_status_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t *p_log_context = p_logger_context;

    do {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
    } while (0);
    return return_status;
}

pal_status_t pal_logger_deinit(void *p_logger_context) {
    pal_status_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t *p_log_context = p_logger_context;

    do {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
    } while (0);
    return return_status;
}

pal_status_t
pal_logger_write(void *p_logger_context, const uint8_t *p_log_data, uint32_t log_data_length) {
    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t *p_log_context = p_logger_context;

    do {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
    } while (0);
    return ((pal_status_t)return_status);
}

pal_status_t
pal_logger_read(void *p_logger_context, uint8_t *p_log_data, uint32_t log_data_length) {
    int32_t return_status = PAL_STATUS_FAILURE;
    pal_logger_t *p_log_context = p_logger_context;

    do {
        // !!!OPTIGA_LIB_PORTING_REQUIRED
    } while (0);
    return ((pal_status_t)return_status);
}
/**
 * @}
 */
