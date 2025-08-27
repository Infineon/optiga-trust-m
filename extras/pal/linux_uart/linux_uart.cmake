# Copyright (c) 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories

set(PAL_FILES 
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

if(BUILD_MBEDTLS_3)
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_3.x_default_config.h" -DOPTIGA_USE_SOFT_RESET -DPAL_OS_HAS_EVENT_INIT -DOPTIGA_SYNC_COMMS)
else()
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_default_config.h" -DOPTIGA_USE_SOFT_RESET -DPAL_OS_HAS_EVENT_INIT -DOPTIGA_SYNC_COMMS)
endif()
add_link_options(-lrt -fprofile-arcs -ftest-coverage --coverage)

add_library(optiga_trust_M_lib STATIC 
    ${SRC_CMD_FILES}
    ${SRC_COMMON_FILES} 
    ${SRC_CRYPT_FILES} 
    ${SRC_UTIL_FILES}
    ${MBEDTLS_FILES}
    ${PAL_CRYPT_FILE}
    ${PAL_FILES})