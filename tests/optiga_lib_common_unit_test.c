/**
 * SPDX-FileCopyrightText: 2024 Infineon Technologies AG
 * SPDX-License-Identifier: MIT
 *
 * \author Infineon Technologies AG
 *
 * \file optiga_lib_common_unit_test.c
 *
 * \brief   This file implements the OPTIGA library common unit tests.
 *
 * \ingroup  grTests
 *
 * @{
 */

#include "optiga_lib_common_unit_test.h"

int main(int argc, char **argv) {
    /* to remove warning for unused parameter */
    (void)(argc);
    (void)(argv);

    uint32_t ut_u32_var = 0x11223344;
    uint8_t ut_u32_u8_array[4];
    uint32_t ut_u32_var_calculated = 0;

    uint16_t ut_u16_var = 0x5566;
    uint8_t ut_u16_u8_array[2];
    uint16_t ut_u16_var_calculated = 0;

    /* optiga_common_set_uint32, optiga_common_get_uint32 check*/
    optiga_common_set_uint32(ut_u32_u8_array, ut_u32_var);
    ut_u32_var_calculated = optiga_common_get_uint32(ut_u32_u8_array);
    assert(ut_u32_var_calculated == ut_u32_var);

    /* optiga_common_set_uint16, optiga_common_get_uint16 check*/
    optiga_common_set_uint16(ut_u16_u8_array, ut_u16_var);
    optiga_common_get_uint16(ut_u16_u8_array, &ut_u16_var_calculated);
    assert(ut_u16_var_calculated == ut_u16_var);
}