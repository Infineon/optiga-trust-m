# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(OPTIGA_Trust_M_Tests)

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
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/comms SRC_COMMS_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/comms/ifx_i2c SRC_COMMS_IFX_I2C_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/crypt SRC_CRYPT_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../src/util SRC_UTIL_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../extras/pal/test_pal PAL_DUMMY_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls/library MBEDTLS_FILES)
set(PAL_FILES ${PROJECT_SOURCE_DIR}/../extras/pal/pal_crypt_mbedtls.c)

# Add compile otions
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage)
add_link_options(-lrt -fprofile-arcs -ftest-coverage --coverage)

# Add source directories to a static library optiga_trust_M_lib
add_library(optiga_trust_M_lib STATIC 
    ${SRC_CMD_FILES}
    ${SRC_COMMON_FILES} 
    ${SRC_COMMS_FILES}
    ${SRC_COMMS_IFX_I2C_FILES}  
    ${SRC_CRYPT_FILES} 
    ${SRC_UTIL_FILES}
    ${MBEDTLS_FILES}
    ${PAL_FILES}
    ${PAL_DUMMY_FILES})

add_compile_options(-Wall -fprofile-arcs -ftest-coverage)

# Add executable files
add_executable(pal_unit_test pal_unit_test.c)
add_executable(pal_crypt_mbedtls_unit_test pal_crypt_mbedtls_unit_test.c)
add_executable(ifx_i2c_phy_layer_unit_test ifx_i2c_phy_layer_unit_test.c)
add_executable(ifx_i2c_data_layer_unit_test ifx_i2c_data_layer_unit_test.c)
add_executable(ifx_i2c_transport_layer_unit_test ifx_i2c_transport_layer_unit_test.c)
add_executable(ifx_i2c_presentation_layer_unit_test ifx_i2c_presentation_layer_unit_test.c)
add_executable(ifx_i2c_unit_test ifx_i2c_unit_test.c)
add_executable(optiga_comms_ifx_i2c_unit_test optiga_comms_ifx_i2c_unit_test.c)
add_executable(optiga_lib_common_unit_test optiga_lib_common_unit_test.c)
add_executable(optiga_lib_logger_unit_test optiga_lib_logger_unit_test.c)
add_executable(optiga_cmd_unit_test optiga_cmd_unit_test.c)
add_executable(optiga_util_integration_test optiga_util_integration_test.c)
add_executable(optiga_crypt_integration_test optiga_crypt_integration_test.c)

# Add target link libraries
target_link_libraries(pal_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(pal_crypt_mbedtls_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(ifx_i2c_phy_layer_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(ifx_i2c_data_layer_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(ifx_i2c_transport_layer_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(ifx_i2c_presentation_layer_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(ifx_i2c_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_comms_ifx_i2c_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_lib_common_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_lib_logger_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_cmd_unit_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_util_integration_test optiga_trust_M_lib -lrt)
target_link_libraries(optiga_crypt_integration_test optiga_trust_M_lib -lrt)

# Add Ctest
include(CTest)

# Define test cases using add_test
add_test(NAME PAL_UNIT_TESTS COMMAND pal_unit_test)
add_test(NAME PAL_CRYPT_MBEDTLS_UNIT_TESTS COMMAND pal_crypt_mbedtls_unit_test)
add_test(NAME IFX_I2C_PHYSICAL_LAYER_UNIT_TESTS COMMAND ifx_i2c_phy_layer_unit_test)
add_test(NAME IFX_I2C_DATA_LAYER_UNIT_TESTS COMMAND ifx_i2c_data_layer_unit_test)
add_test(NAME IFX_I2C_TRANSPORT_LAYER_UNIT_TESTS COMMAND ifx_i2c_transport_layer_unit_test)
add_test(NAME IFX_I2C_PRESENTATION_LAYER_UNIT_TESTS COMMAND ifx_i2c_presentation_layer_unit_test)
add_test(NAME IFX_I2C_UNIT_TESTS COMMAND ifx_i2c_unit_test)
add_test(NAME OPTIGA_COMMS_IFX_I2C_UNIT_TEST COMMAND optiga_comms_ifx_i2c_unit_test)
add_test(NAME OPTIGA_LIB_COMMON_UNIT_TEST COMMAND optiga_lib_common_unit_test)
add_test(NAME OPTIGA_LIB_LOGGER_UNIT_TEST COMMAND optiga_lib_logger_unit_test)
add_test(NAME OPTIGA_CMD_UNIT_TEST COMMAND optiga_cmd_unit_test)
add_test(NAME OPTIGA_UTIL_INTEGRATION_TEST COMMAND optiga_util_integration_test)
add_test(NAME OPTIGA_CRYPT_INTEGRATION_TEST COMMAND optiga_crypt_integration_test)