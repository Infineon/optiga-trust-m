# SPDX-FileCopyrightText: 2024 Infineon Technologies AG
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(OPTIGA_Trust_M_Tests)

# Add compile otions
option(BUILD_RPI_I2C "Compile for Raspberry Pi, protocol : I2C" OFF)
option(BUILD_RPI_UART "Compile for Raspberry Pi, protocol : UART" OFF)
option(BUILD_LIBUSB "Compile for Raspberry Pi, protocol : libusb" OFF)

# Define test cases using add_test
if(BUILD_RPI_I2C)
    message("Tests running on RPI i2c")
    include(${PROJECT_SOURCE_DIR}/linux_i2c.cmake)
elseif(BUILD_RPI_UART)
    message("Tests running on RPI uart")
    include(${PROJECT_SOURCE_DIR}/linux_uart.cmake)
elseif(BUILD_LIBUSB)
    message("Tests running for libusb")
    include(${PROJECT_SOURCE_DIR}/libusb.cmake)
else()
    message("Tests running on Docker image")
    include(${PROJECT_SOURCE_DIR}/linux_test.cmake)
endif()