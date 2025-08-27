# Copyright (c) 2024 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories

set(PAL_FILES 
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

if(BUILD_MBEDTLS_3)
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_3.x_default_config.h" -DIFX_I2C_FRAME_SIZE=55 -DUSE_LIBUSB_PAL -DPAL_OS_HAS_EVENT_INIT)
else()
add_compile_options(-Wall -fprofile-arcs -ftest-coverage --coverage -DMBEDTLS_USER_CONFIG_FILE="../config/mbedtls_default_config.h" -DIFX_I2C_FRAME_SIZE=55 -DUSE_LIBUSB_PAL -DPAL_OS_HAS_EVENT_INIT)
endif()
add_link_options(-lrt -lusb-1.0 -lm -fprofile-arcs -ftest-coverage --coverage)

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