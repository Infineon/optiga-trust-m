# Copyright (c) 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories

aux_source_directory(${PROJECT_SOURCE_DIR}/../extras/pal/test_pal PAL_FILES)

if(BUILD_MBEDTLS_3)
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_3.x_default_config.h" -DOPTIGA_USE_SOFT_RESET -DPAL_OS_HAS_EVENT_INIT)
else()
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_default_config.h" -DOPTIGA_USE_SOFT_RESET -DPAL_OS_HAS_EVENT_INIT)
endif()
add_link_options(-lrt -fprofile-arcs -ftest-coverage --coverage)

add_library(optiga_trust_M_lib STATIC 
    ${SRC_CMD_FILES}
    ${SRC_COMMON_FILES} 
    ${SRC_COMMS_FILES}
    ${SRC_COMMS_IFX_I2C_FILES}  
    ${SRC_CRYPT_FILES} 
    ${SRC_UTIL_FILES}
    ${MBEDTLS_FILES}
    ${PAL_CRYPT_FILE}
    ${PAL_FILES})