# Copyright (c) 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../include/pal
    ${PROJECT_SOURCE_DIR}/../include/common
    ${PROJECT_SOURCE_DIR}/../include/comms
    ${PROJECT_SOURCE_DIR}/../include/cmd
    ${PROJECT_SOURCE_DIR}/../include/ifx_i2c
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include/mbedtls
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include/psa
    ${PROJECT_SOURCE_DIR}/../tests
)

# Add all source files in the src directory
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/cmd SRC_CMD_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/common SRC_COMMON_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/crypt SRC_CRYPT_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/util SRC_UTIL_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls/library MBEDTLS_FILES)
set(PAL_FILES ${PROJECT_SOURCE_DIR}/../extras/pal/pal_crypt_mbedtls.c)

set(PAL_LINUX_UART_FILES 
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/optiga_comms_tc_uart.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_config.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_gpio.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_logger.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_os_datastore.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_os_event.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_os_lock.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_os_memory.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/pal_os_timer.c
)

add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_default_config.h" -DOPTIGA_USE_SOFT_RESET -DPAL_OS_HAS_EVENT_INIT -DOPTIGA_SYNC_COMMS)
add_link_options(-lrt -fprofile-arcs -ftest-coverage --coverage)

add_library(optiga_trust_M_lib STATIC 
    ${SRC_CMD_FILES}
    ${SRC_COMMON_FILES} 
    ${SRC_CRYPT_FILES} 
    ${SRC_UTIL_FILES}
    ${MBEDTLS_FILES}
    ${PAL_FILES}
    ${PAL_LINUX_UART_FILES})

# Add executable files
add_executable(optiga_lib_common_unit_test optiga_lib_common_unit_test.c)
add_executable(optiga_lib_logger_unit_test optiga_lib_logger_unit_test.c)
add_executable(optiga_cmd_unit_test optiga_cmd_unit_test.c)
add_executable(optiga_util_integration_test optiga_util_integration_test.c)
add_executable(optiga_crypt_integration_test optiga_crypt_integration_test.c)

# Add target link libraries
target_link_libraries(optiga_lib_common_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_lib_logger_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_cmd_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_util_integration_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_crypt_integration_test optiga_trust_M_lib -lrt)

# Add Ctest
include(CTest)

add_test(NAME OPTIGA_LIB_COMMON_UNIT_TEST COMMAND optiga_lib_common_unit_test)
add_test(NAME OPTIGA_LIB_LOGGER_UNIT_TEST COMMAND optiga_lib_logger_unit_test)
add_test(NAME OPTIGA_CMD_UNIT_TEST COMMAND optiga_cmd_unit_test)
add_test(NAME OPTIGA_UTIL_INTEGRATION_TEST COMMAND optiga_util_integration_test)
add_test(NAME OPTIGA_CRYPT_INTEGRATION_TEST COMMAND optiga_crypt_integration_test)