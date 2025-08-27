# Copyright (c) 2025 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add executable files
add_executable(optiga_lib_common_unit_test optiga_lib_common_unit_test.c)
add_executable(optiga_lib_logger_unit_test optiga_lib_logger_unit_test.c)
add_executable(optiga_cmd_unit_test optiga_cmd_unit_test.c)
add_executable(optiga_util_integration_test optiga_util_integration_test.c)
add_executable(optiga_crypt_integration_test optiga_crypt_integration_test.c)

# Add target link libraries
if(BUILD_LIBUSB)
target_link_libraries(optiga_lib_common_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_lib_logger_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_cmd_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_util_integration_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_crypt_integration_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
else()
target_link_libraries(optiga_lib_common_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_lib_logger_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_cmd_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_util_integration_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_crypt_integration_test optiga_trust_M_lib -lrt)
endif()

# Add Ctest
include(CTest)

add_test(NAME OPTIGA_LIB_COMMON_UNIT_TEST COMMAND optiga_lib_common_unit_test)
add_test(NAME OPTIGA_LIB_LOGGER_UNIT_TEST COMMAND optiga_lib_logger_unit_test)
add_test(NAME OPTIGA_CMD_UNIT_TEST COMMAND optiga_cmd_unit_test)
add_test(NAME OPTIGA_UTIL_INTEGRATION_TEST COMMAND optiga_util_integration_test)
add_test(NAME OPTIGA_CRYPT_INTEGRATION_TEST COMMAND optiga_crypt_integration_test)