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
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/include
    ${PROJECT_SOURCE_DIR}/../tests
)

# Add all source files in the src directory
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/cmd SRC_CMD_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/common SRC_COMMON_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/comms SRC_COMMS_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/comms/ifx_i2c SRC_COMMS_IFX_I2C_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/crypt SRC_CRYPT_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/util SRC_UTIL_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls/library MBEDTLS_FILES)
set(PAL_FILES ${PROJECT_SOURCE_DIR}/../extras/pal/pal_crypt_mbedtls.c)

set(PAL_LIBUSB_FILES 
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_common.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_gpio.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_i2c.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_ifx_usb_config.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_logger.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_os_datastore.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_os_event.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_os_lock.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_os_memory.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal_os_timer.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/libusb/pal.c
)

add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_default_config.h" -DIFX_I2C_FRAME_SIZE=55 -DUSE_LIBUSB_PAL -DPAL_OS_HAS_EVENT_INIT)
add_link_options(-lrt -lusb-1.0 -lm -fprofile-arcs -ftest-coverage --coverage)

add_library(optiga_trust_M_lib STATIC 
    ${SRC_CMD_FILES}
    ${SRC_COMMON_FILES} 
    ${SRC_COMMS_FILES}
    ${SRC_COMMS_IFX_I2C_FILES}  
    ${SRC_CRYPT_FILES} 
    ${SRC_UTIL_FILES}
    ${MBEDTLS_FILES}
    ${PAL_FILES}
    ${PAL_LIBUSB_FILES})


# Add executable files
add_executable(pal_crypt_mbedtls_unit_test pal_crypt_mbedtls_unit_test.c)
add_executable(optiga_comms_ifx_i2c_unit_test optiga_comms_ifx_i2c_unit_test.c)
add_executable(optiga_lib_common_unit_test optiga_lib_common_unit_test.c)
add_executable(optiga_lib_logger_unit_test optiga_lib_logger_unit_test.c)
add_executable(optiga_cmd_unit_test optiga_cmd_unit_test.c)
add_executable(optiga_util_integration_test optiga_util_integration_test.c)
add_executable(optiga_crypt_integration_test optiga_crypt_integration_test.c)

# Add target link libraries
target_link_libraries(pal_crypt_mbedtls_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_comms_ifx_i2c_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_lib_common_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_lib_logger_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_cmd_unit_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_util_integration_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)
target_link_libraries(optiga_crypt_integration_test optiga_trust_M_lib -lrt -lusb-1.0 -lm)

# Add Ctest
include(CTest)

add_test(NAME PAL_CRYPT_MBEDTLS_UNIT_TESTS COMMAND pal_crypt_mbedtls_unit_test)
add_test(NAME OPTIGA_COMMS_IFX_I2C_UNIT_TEST COMMAND optiga_comms_ifx_i2c_unit_test)
add_test(NAME OPTIGA_LIB_COMMON_UNIT_TEST COMMAND optiga_lib_common_unit_test)
add_test(NAME OPTIGA_LIB_LOGGER_UNIT_TEST COMMAND optiga_lib_logger_unit_test)
add_test(NAME OPTIGA_CMD_UNIT_TEST COMMAND optiga_cmd_unit_test)
add_test(NAME OPTIGA_UTIL_INTEGRATION_TEST COMMAND optiga_util_integration_test)
add_test(NAME OPTIGA_CRYPT_INTEGRATION_TEST COMMAND optiga_crypt_integration_test)