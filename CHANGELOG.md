## 5.3.0 (2024-08-28)
* Add options for default configuration for MBEDTLS.

## 5.2.0 (2024-08-22)
* Add shared memory pthread mutex for multi-process and multi-threads applications for Linux.
* Improve pal_os_event logging.
* Improve logging for Linux UART.
* Improve logging for Windows UART.

## 5.1.0 (2024-07-29)
* Improved signature encoding for secp521r1 signature.
* Improved logging output.
* Fixed race condition in pal_os_event callback in libusb PAL.
* Added checks to signature padding check.
* Added default configuration for MBedTLS.
* Removed warnings during library compilation.
* Resolved initialization fails when triggering reset.

## 5.0.1 (2024-05-21)
* Fix to allow compilation using zephyr PAL.
* Changes in README.md for host applications.
* Added missing license headers.

## 5.0.0 (2024-04-10)
* New folder structure.
* Mbedtls is now configured to be used a submodule from https://github.com/Mbed-TLS/mbedtls/tree/v2.28.7.
* Added support for Zehpyr OS.
* README.md update.
* Fix Metadata configuration for OID 0xE0E8.
* Fix buffer length when reading PBS for shielded I2C connection.

## 4.2.0 (2024-02-16)
* Update Documentation to match the launch of OPTIGA™ Trust M MTR
  * README.md
  * Datasheet v3.61
  * Solution Reference Manual v3.60
  * Config Guide v2.2
* Include new Shields and Get-Started (PSoC62S2)
* Fix Typo in Protected-Update Documentation & Parser

## 4.1.0 (2023-02-14)
* PSoC PALs were removed from the PALs directory and shall be now part of the application

## 3.1.4 (2022-08-02)
* Fixed 62S2 Pin assignment for the Power Control
* Fixed wrong ASN.1 ECDSA Signature size (r and s components) calculation.
* Datasheet Update
* (Linux PAL) Fixed the Segmentation fault for 64bits Linux system
* (Linux PAL) Added libgpiod support

## 3.1.3 (2022-06-29)
* Fix i2c initialization routine for the PSoC6 FreeRTOS component
* Add the OPTIGA™ Trust IoT Security Development Kit link to the readme

## 3.1.2 (2021-10-25)
* After the 3.01.2558 release of the core, I2C address structure member in the PAL changed its place, this release fixes it.

## 3.1.1 (2021-10-22)
* Fix building "Error[Pe513]: a value of type "void (*)(void *, uint16_t)" cannot be assigned to an entity of type "void *"" Error in the optiga/comms/ifx_i2c/ifx_i2c_physical_layer.c file

## 3.1.0 (2021-10-19)
* Based on the 3.01.2558 release of the core communication stack plus additional tools and PALs
* Missing of error handling for pal_i2c_init failure keeps the local host application in hung state when optiga_util_open_application is invoked.
* ifx_i2c_context structure and other structure members are not arranged in the descending order of their sizes, leading to incorrect memory access in case of few compilers.
* Re-entrancy issues in execution handler of OPTIGA™ command layer and ifx_i2c layer in linux environment, when the CPU load is high.
* In Protected update tool, unicast option is considered as Octet string instead of hex array, creating an invalid Manifest.
* Few of the resources created/initialized as part of optiga_util_create / optiga_crypt_create / optiga_util_open_application are not de-allocated / destroyed as part of optiga_util_destroy / optiga_crypt_destroy / optiga_util_close_application, creating problems in multi-process linux execution environment.
