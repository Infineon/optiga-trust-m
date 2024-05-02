/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file pal_gpio.h
 *
 * \brief   This file provides the prototype declarations of PAL GPIO.
 *
 * \ingroup  grPAL
 *
 * @{
 */

#ifndef _PAL_GPIO_H_
#define _PAL_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pal.h"

/**
 * \brief Structure defines the PAL GPIO configuration.
 */
typedef struct pal_gpio {
    /// Pointer to GPIO platform specific context/structure
    void *p_gpio_hw;

} pal_gpio_t;

/**
 * \brief Function to transmit and receive a packet.
 *
 * \details
 * Function to transmit and receive a packet.
 * - The API sets the pin high, only if the pin is assigned to a valid GPIO context<br>
 *   Otherwise the API returns without any failure status
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 *\param[in] p_gpio_context      Valid pointer to PAL layer GPIO context
 *
 *
 */
LIBRARY_EXPORTS void pal_gpio_set_high(const pal_gpio_t *p_gpio_context);

/**
 * \brief Sets the GPIO pin to Low state.
 *
 * \details
 * Sets the GPIO pin to Low state.
 * - The API set the pin low, only if the pin is assigned to a valid GPIO context.<br>
 *   Otherwise the API returns without any failure status.<br>
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 *\param[in] p_gpio_context      Valid pointer to PAL layer GPIO context
 *
 */
LIBRARY_EXPORTS void pal_gpio_set_low(const pal_gpio_t *p_gpio_context);

/**
 * \brief Function to initialize GPIO.
 *
 * \details
 * Function to initialize GPIO.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_gpio_context                         Valid pointer to PAL layer GPIO context
 *
 * \retval    #PAL_STATUS_SUCCESS                    On successful execution
 * \retval    #PAL_STATUS_FAILURE                    On failure
 *
 */
LIBRARY_EXPORTS pal_status_t pal_gpio_init(const pal_gpio_t *p_gpio_context);

/**
 * \brief Function to de-initialize GPIO.
 *
 * \details
 * Function to de-initialize GPIO.
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_gpio_context                         Valid pointer to PAL layer GPIO context
 *
 * \retval    #PAL_STATUS_SUCCESS                    On successful execution
 * \retval    #PAL_STATUS_FAILURE                    On failure
 */
LIBRARY_EXPORTS pal_status_t pal_gpio_deinit(const pal_gpio_t *p_gpio_context);

#ifdef __cplusplus
}
#endif

#endif /* _PAL_GPIO_H_ */

/**
 * @}
 */
