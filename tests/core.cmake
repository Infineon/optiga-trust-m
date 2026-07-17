# Copyright (c) 2026 Infineon Technologies AG
#
# SPDX-License-Identifier: MIT

# Add include directories
#
# Only Mbed TLS 4.x (with TF-PSA-Crypto) is supported. 
include_directories(
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../include/pal
    ${PROJECT_SOURCE_DIR}/../include/common
    ${PROJECT_SOURCE_DIR}/../include/comms
    ${PROJECT_SOURCE_DIR}/../include/cmd
    ${PROJECT_SOURCE_DIR}/../include/ifx_i2c
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/library
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/include/psa
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/utilities
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/dispatch
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/platform
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/drivers/builtin/include
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/drivers/builtin/src
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/core
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto/extras
    ${PROJECT_SOURCE_DIR}/../config
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

set(TF_PSA_CRYPTO_DIR
    ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/tf-psa-crypto)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
execute_process(
    COMMAND ${Python3_EXECUTABLE}
        ${TF_PSA_CRYPTO_DIR}/scripts/generate_driver_wrappers.py
        --project-root ${TF_PSA_CRYPTO_DIR}
        ${TF_PSA_CRYPTO_DIR}/core
    WORKING_DIRECTORY ${TF_PSA_CRYPTO_DIR}
    RESULT_VARIABLE _tfpsa_gen_res
    OUTPUT_VARIABLE _tfpsa_gen_out
    ERROR_VARIABLE  _tfpsa_gen_err)
if(NOT _tfpsa_gen_res EQUAL 0)
    message(FATAL_ERROR
        "generate_driver_wrappers.py failed (exit ${_tfpsa_gen_res}).\n"
        "--- stdout ---\n${_tfpsa_gen_out}"
        "--- stderr ---\n${_tfpsa_gen_err}"
        "Ensure Python 3, Jinja2, jsonschema are installed and that "
        "the tf-psa-crypto/framework submodule is initialised.")
endif()
execute_process(
    COMMAND ${Python3_EXECUTABLE}
        ${TF_PSA_CRYPTO_DIR}/scripts/generate_config_checks.py
        ${TF_PSA_CRYPTO_DIR}/core
    WORKING_DIRECTORY ${TF_PSA_CRYPTO_DIR}
    RESULT_VARIABLE _tfpsa_gen_res
    OUTPUT_VARIABLE _tfpsa_gen_out
    ERROR_VARIABLE  _tfpsa_gen_err)
if(NOT _tfpsa_gen_res EQUAL 0)
    message(FATAL_ERROR
        "generate_config_checks.py failed (exit ${_tfpsa_gen_res}).\n"
        "--- stdout ---\n${_tfpsa_gen_out}"
        "--- stderr ---\n${_tfpsa_gen_err}")
endif()

set(MBEDTLS_4_DIR ${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x)
execute_process(
    COMMAND ${Python3_EXECUTABLE}
        ${MBEDTLS_4_DIR}/scripts/generate_config_checks.py
        ${MBEDTLS_4_DIR}/library
    WORKING_DIRECTORY ${MBEDTLS_4_DIR}
    RESULT_VARIABLE _mbed_gen_res
    OUTPUT_VARIABLE _mbed_gen_out
    ERROR_VARIABLE  _mbed_gen_err)
if(NOT _mbed_gen_res EQUAL 0)
    message(FATAL_ERROR
        "mbedtls-4.x/scripts/generate_config_checks.py failed (exit ${_mbed_gen_res}).\n"
        "--- stdout ---\n${_mbed_gen_out}"
        "--- stderr ---\n${_mbed_gen_err}"
        "Ensure external/mbedtls-4.x/framework submodule is initialised.")
endif()

aux_source_directory(${PROJECT_SOURCE_DIR}/../external/mbedtls-4.x/library MBEDTLS_FILES)
aux_source_directory(${TF_PSA_CRYPTO_DIR}/core                MBEDTLS_FILES)
aux_source_directory(${TF_PSA_CRYPTO_DIR}/platform            MBEDTLS_FILES)
aux_source_directory(${TF_PSA_CRYPTO_DIR}/utilities           MBEDTLS_FILES)
aux_source_directory(${TF_PSA_CRYPTO_DIR}/extras              MBEDTLS_FILES)
aux_source_directory(${TF_PSA_CRYPTO_DIR}/drivers/builtin/src MBEDTLS_FILES)
set(OPTIGA_MBEDTLS_LIBS "")

set(PAL_CRYPT_FILE ${PROJECT_SOURCE_DIR}/../extras/pal/pal_crypt_psa.c)
