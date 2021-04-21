/**
* \copyright
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \endcopyright
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

#include "optiga/optiga_lib_config.h"
#include "optiga/pal/pal.h"
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "optiga/pal/pal_os_timer.h"

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
pal_status_t pal_init(void)
{
	pal_status_t status;

#if OPTIGA_COMMS_DEFAULT_RESET_TYPE == 0
    status = pal_gpio_init(&optiga_vdd_0);
    if (status != PAL_STATUS_SUCCESS)
        return status;
#endif

#if OPTIGA_COMMS_DEFAULT_RESET_TYPE == 0 || OPTIGA_COMMS_DEFAULT_RESET_TYPE == 2
    status = pal_gpio_init(&optiga_reset_0);
    if (status != PAL_STATUS_SUCCESS)
        return status;
#endif

	status = pal_timer_init();
    if (status != PAL_STATUS_SUCCESS)
        return status;

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
pal_status_t pal_deinit(void)
{
    pal_timer_deinit();

#if OPTIGA_COMMS_DEFAULT_RESET_TYPE == 0 || OPTIGA_COMMS_DEFAULT_RESET_TYPE == 2
    pal_gpio_deinit(&optiga_reset_0);
#endif

#if OPTIGA_COMMS_DEFAULT_RESET_TYPE == 0
    pal_gpio_deinit(&optiga_vdd_0);
#endif

    return PAL_STATUS_SUCCESS;
}

/**
* @}
*/

