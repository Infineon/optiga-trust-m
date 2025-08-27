# Copyright (c) 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories
set(PAL_FILES 
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_gpio.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_i2c.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_logger.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_os_datastore.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_os_event.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_os_lock.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_os_memory.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_os_timer.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal_shared_mutex.c
    ${PROJECT_SOURCE_DIR}/../extras/pal/linux/pal.c
)
aux_source_directory(${PROJECT_SOURCE_DIR}/../extras/pal/linux/target/rpi3 PAL_LINUX_RPI_FILES)

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
    ${PAL_FILES}
    ${PAL_LINUX_RPI_FILES})