/**
* \copyright
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
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
typedef struct pal_gpio
{
    /// Pointer to GPIO platform specific context/structure
    void * p_gpio_hw;

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
LIBRARY_EXPORTS void pal_gpio_set_high(const pal_gpio_t * p_gpio_context);

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
LIBRARY_EXPORTS void pal_gpio_set_low(const pal_gpio_t * p_gpio_context);

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
LIBRARY_EXPORTS pal_status_t pal_gpio_init(const pal_gpio_t * p_gpio_context);

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
LIBRARY_EXPORTS pal_status_t pal_gpio_deinit(const pal_gpio_t * p_gpio_context);


#ifdef __cplusplus
}
#endif

#endif /* _PAL_GPIO_H_ */

/**
* @}
*/

