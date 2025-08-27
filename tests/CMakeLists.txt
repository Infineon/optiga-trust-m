# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(OPTIGA_Trust_M_Tests)

# Add compile otions
option(BUILD_RPI_I2C "Compile for Raspberry Pi, protocol : I2C" OFF)
option(BUILD_RPI_UART "Compile for Raspberry Pi, protocol : UART" OFF)
option(BUILD_LIBUSB "Compile for Raspberry Pi, protocol : libusb" OFF)

# Compile for mbedtls-3.x
option(BUILD_MBEDTLS_3 "Compile using mbedtls-3.x" OFF)

# message log for mbedtls
if(BUILD_MBEDTLS_3)
    message("Building using mbedtls-3.x")
else()
    message("Building using mbedtls-2.x")
endif()

include(${PROJECT_SOURCE_DIR}/core.cmake)

# Define test cases using add_test
if(BUILD_RPI_I2C)
    message("Tests running on RPI i2c")
    include(${PROJECT_SOURCE_DIR}/../extras/pal/linux/linux_i2c.cmake)
elseif(BUILD_RPI_UART)
    message("Tests running on RPI uart")
    include(${PROJECT_SOURCE_DIR}/../extras/pal/linux_uart/linux_uart.cmake)
elseif(BUILD_LIBUSB)
    message("Tests running for libusb")
    include(${PROJECT_SOURCE_DIR}/../extras/pal/libusb/libusb.cmake)
else()
    message("Tests running on Docker image")
    include(${PROJECT_SOURCE_DIR}/../extras/pal/test_pal/linux_test.cmake)
endif()

include(${PROJECT_SOURCE_DIR}/link_execute.cmake)