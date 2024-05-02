/**
 * SPDX-FileCopyrightText: 2018-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal.c
 *
 * \brief    This file implements the platform abstraction layer APIs.
 *
 * \ingroup  grPAL
 * @{
 */

#include "pal.h"

/**
 * @brief Initializes the PAL layer
 *
 * Initializes the PAL layer.
 * <br>
 *
 *<b>API Details:</b>
 * - Initializes the PAL layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL init fails.
 */
pal_status_t pal_init(void) {
    return PAL_STATUS_SUCCESS;
}

/**
 * @brief De-Initializes the PAL layer
 *
 * De-Initializes the PAL layer.
 * <br>
 *
 *<b>API Details:</b>
 * - De-Initializes the PAL layer<br>
 *
 *<b>User Input:</b><br>
 * - None
 *
 * \retval  #PAL_STATUS_SUCCESS  Returns when the PAL de-init it successfull
 * \retval  #PAL_STATUS_FAILURE  Returns when the PAL de-init fails.
 */
pal_status_t pal_deinit(void) {
    return PAL_STATUS_SUCCESS;
}

/**
 * @}
 */
