/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_logger_unit_test.c
 *
 * \brief    This file implements the OPTIGA lib logger unit tests
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_lib_logger_unit_test.h"

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    char_t *input_string = "Optiga Trust - Unit Tests";
    uint16_t ut_return_success = OPTIGA_LIB_SUCCESS;
    uint16_t ut_return_failure = OPTIGA_DEVICE_ERROR;

    /* optiga_lib_print_string check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_string((const char_t *)input_string);

    /* optiga_lib_print_string_with_newline check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_string_with_newline((const char_t *)input_string);

    /* optiga_lib_print_message check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_message(
        (const char_t *)input_string,
        OPTIGA_UTIL_SERVICE,
        OPTIGA_LIB_LOGGER_COLOR_RED
    );

    /* optiga_lib_print_status check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_status(OPTIGA_CRYPT_SERVICE, OPTIGA_LIB_LOGGER_COLOR_GREEN, ut_return_success);

    /* optiga_lib_print_status check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_status(
        OPTIGA_COMMAND_LAYER,
        OPTIGA_LIB_LOGGER_COLOR_YELLOW,
        ut_return_failure
    );

    /* optiga_lib_print_array_hex_format check
    *  Nothing to be checked here, just for coverage
    +  Checked during execution
    */
    optiga_lib_print_array_hex_format(
        (const uint8_t *)input_string,
        6,
        OPTIGA_LIB_LOGGER_COLOR_BLUE
    );
}