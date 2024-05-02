/**
 * SPDX-FileCopyrightText: 2021-2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file    optiga_lib_logger.h
 *
 * \brief   This file provides the prototypes for the OPTIGA library logger.
 *
 * \ingroup grOptigaLibCommon
 *
 * @{
 */

#ifndef _OPTIGA_LIB_LOGGER_H_
#define _OPTIGA_LIB_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga_lib_types.h"

// Logger levels
#define OPTIGA_UTIL_SERVICE "[optiga util]     : "
#define OPTIGA_CRYPT_SERVICE "[optiga crypt]    : "
#define OPTIGA_COMMAND_LAYER "[optiga cmd]      : "
#define OPTIGA_COMMUNICATION_LAYER "[optiga comms]    : "

// Logger colors options
#define OPTIGA_LIB_LOGGER_COLOR_RED "\x1b[31m"
#define OPTIGA_LIB_LOGGER_COLOR_GREEN "\x1b[32m"
#define OPTIGA_LIB_LOGGER_COLOR_YELLOW "\x1b[33m"
#define OPTIGA_LIB_LOGGER_COLOR_BLUE "\x1b[34m"
#define OPTIGA_LIB_LOGGER_COLOR_MAGENTA "\x1b[35m"
#define OPTIGA_LIB_LOGGER_COLOR_CYAN "\x1b[36m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_GREY "\x1b[90m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_RED "\x1b[91m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_GREEN "\x1b[92m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_YELLOW "\x1b[93m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_BLUE "\x1b[94m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_MAGENTA "\x1b[95m"
#define OPTIGA_LIB_LOGGER_COLOR_LIGHT_CYAN "\x1b[96m"
#define OPTIGA_LIB_LOGGER_COLOR_DEFAULT "\x1b[0m"

// Logger color for different layers and data
#define OPTIGA_UTIL_SERVICE_COLOR OPTIGA_LIB_LOGGER_COLOR_DEFAULT
#define OPTIGA_CRYPT_SERVICE_COLOR OPTIGA_LIB_LOGGER_COLOR_DEFAULT
#define OPTIGA_COMMAND_LAYER_COLOR OPTIGA_LIB_LOGGER_COLOR_DEFAULT
#define OPTIGA_COMMUNICATION_LAYER_COLOR OPTIGA_LIB_LOGGER_COLOR_DEFAULT
#define OPTIGA_ERROR_COLOR OPTIGA_LIB_LOGGER_COLOR_RED
#define OPTIGA_UNPROTECTED_DATA_COLOR OPTIGA_LIB_LOGGER_COLOR_LIGHT_CYAN
#define OPTIGA_PROTECTED_DATA_COLOR OPTIGA_LIB_LOGGER_COLOR_YELLOW

#define OPTIGA_HEX_BYTE_SEPERATOR ""
/**
 * \brief To log a string
 *
 * \details
 * To log a string.
 * -
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] input_string        Pointer to the log string
 *
 */
void optiga_lib_print_string(const char_t *input_string);

/**
 * \brief To log a string with a new line
 *
 * \details
 * To log a string with a new line
 * -
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_log_string        Pointer to the log string
 *
 */
void optiga_lib_print_string_with_newline(const char_t *p_log_string);

/**
 * \brief To log a string with a new line and layer information.
 *
 * \details
 * To log a string with information of the layer from which it is invoked and with a new line
 * -
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_log_string       Pointer to the log string
 * \param[in] p_log_layer        Pointer to the invoked layer information
 * \param[in] p_log_color        Pointer to the color of log string
 *
 */
void optiga_lib_print_message(
    const char_t *p_log_string,
    const char_t *p_log_layer,
    const char_t *p_log_color
);

/**
 * \brief To log a return value with a new line and layer information.
 *
 * \details
 * To log a if the return value was successful or not along with layer information
 * -
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_log_layer        Pointer to the invoked layer information
 * \param[in] p_log_color        Pointer to the color of log string
 * \param[in] return_value       Return value from the invoked layer
 *
 */
void optiga_lib_print_status(
    const char_t *p_log_layer,
    const char_t *p_log_color,
    uint16_t return_value
);
/**
 * \brief To log the byte array in hex format
 *
 * \details
 * To log the byte array in hex format
 * -
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[in] p_log_string        Pointer to the array string
 * \param[in] length              Length of the array buffer
 * \param[in] p_log_color         Pointer to the color of array string
 *
 */
void optiga_lib_print_array_hex_format(
    const uint8_t *p_log_string,
    uint16_t length,
    const char_t *p_log_color
);

/**
 * \brief Converts the input array buffer to encode the color specified
 *
 * \details
 * Converts the input array buffer  to encode the color specified
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[out] p_output_buffer    Pointer to output buffer
 * \param[in]  p_temp_buffer      Pointer to the input array buffer
 * \param[in]  p_color            Pointer to the color of array string
 */
#define OPTIGA_LIB_LOGGER_PRINT_ARRAY(p_output_buffer, p_temp_buffer, p_color) \
    { \
        sprintf( \
            (char_t *)p_output_buffer, \
            "%25s%s%s", \
            p_color, \
            p_temp_buffer, \
            OPTIGA_LIB_LOGGER_COLOR_DEFAULT \
        ); \
    }

/**
 * \brief Converts the input string to encode the color specified
 *
 * \details
 * Converts the input string to encode the color specified
 *
 * \pre
 * - None
 *
 * \note
 * - None
 *
 * \param[out] p_output_buffer    Pointer to output buffer
 * \param[in]  p_temp_buffer      Pointer to the input array buffer
 * \param[in]  p_layer            Pointer to the layer info buffer
 * \param[in]  p_color            Pointer to the color of array string
 */
#define OPTIGA_LIB_LOGGER_PRINT_INFO(p_output_buffer, p_temp_buffer, p_layer, p_color) \
    { \
        sprintf( \
            (char_t *)p_output_buffer, \
            "%s%s%s%s", \
            p_color, \
            p_layer, \
            p_temp_buffer, \
            OPTIGA_LIB_LOGGER_COLOR_DEFAULT \
        ); \
    }

#ifdef __cplusplus
}
#endif

#endif /*_OPTIGA_LIB_LOGGER_H_ */

/**
 * @}
 */
