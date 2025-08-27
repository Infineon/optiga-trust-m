# Copyright (c) 2025 Infineon Technologies AG
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
if(BUILD_MBEDTLS_3)
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-3.x/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-3.x/include/mbedtls
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-3.x/include/psa
else()
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include/mbedtls
    ${PROJECT_SOURCE_DIR}/../external/mbedtls/include/psa
endif()
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
if(BUILD_MBEDTLS_3)
aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls-3.x/library MBEDTLS_FILES)
else()
aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls/library MBEDTLS_FILES)
endif()
set(PAL_CRYPT_FILE ${PROJECT_SOURCE_DIR}/../extras/pal/pal_crypt_mbedtls.c)