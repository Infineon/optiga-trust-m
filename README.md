# Infineon OPTIGA™ Trust M Host Library for C

This is the [OPTIGA™ Trust M](https://www.infineon.com/optiga-trust) Host Library for C. It is a reference implementation that allows the use of the [OPTIGA™ Trust M](https://www.infineon.com/optiga-trust) via a high-level API and serves as a basis of our example host applications.

## Quick navigation

- [Infineon OPTIGA™ Trust M Host Library for C](#infineon-optiga-trust-m-host-library-for-c)
  - [Quick navigation](#quick-navigation)
  - [Getting started](#getting-started)
    - [Getting the Host Library](#getting-the-host-library)
    - [Building and testing the Host Library](#building-and-testing-the-host-library)
    - [Porting the Host library](#porting-the-host-library)
    - [Examples using Host Library](#examples-using-host-library)
  - [Host library overview](#host-library-overview)
  - [Host Applications](#host-applications)
  - [Contributing](#contributing)
  - [Licensing](#licensing)

## Getting started

### Getting the Host Library

Below are the instructions to get the OPTIGA&trade; Trust M Host Library for C.

The OPTIGA&trade; Trust M Host Library for C can be cloned using git command :

    git clone --recurse-submodules https://github.com/Infineon/optiga-trust-m.git

### Building and testing the Host Library

Instructions on how to build and test the host library can be found [here](tests/README.md)

### Porting the Host library 

Instructions on how to port the host library to a specific platform can be found [here](extras/pal/README.md)

### Examples using Host Library

Examples to demonstrate basic functionality of the security chip can be found [here](examples/README.md)

## Host library overview

![](docs/images/optiga_trust_m_system_block_diagram.png)

The following documentation is available for the host library.

| Topic | Link to PDF documentation                                                                                                                                              |
| ----- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| CRYPT | [OPTIGA™ Trust M Crypt API](https://github.com/Infineon/optiga-trust-m-overview/blob/main/docs/pdf/OPTIGA_Trust_M_ConfigGuide_v2.2.pdf)                                |
| UTIL  | [OPTIGA™ Trust M Util API](https://github.com/Infineon/optiga-trust-m-overview/blob/main/docs/pdf/optiga-trust-m/wiki/Trust-M-Util-API)                                |
| CMD   | [OPTIGA™ Trust M Solution Reference Manual](https://github.com/Infineon/optiga-trust-m-overview/blob/main/docs/pdf/OPTIGA_Trust_M_Solution_Reference_Manual_v3.60.pdf) |
| PAL   | [OPTIGA™ Trust M Porting Guide](extras/pal/README.md)                                                                                                                  |
| COMMS | [Infineon I2C Protocol](https://github.com/Infineon/optiga-trust-m-overview/blob/main/docs/pdf/Infineon_I2C_Protocol_v2.03.pdf)                                        |

For more information please refer to the [Wiki page](https://github.com/Infineon/optiga-trust-m/wiki) of this project

## Host Applications

| Host application                                                                                                                                         | Platform      | Status |
| -------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------- | ------ |
| [OPTIGA™ Trust M Cryptography](https://github.com/Infineon/mtb-example-optiga-crypto)                                                                    | ModusToolbox™ | Active |
| [OPTIGA™ Trust M MQTT Client](https://github.com/Infineon/mtb-example-optiga-mqtt-client)                                                                | ModusToolbox™ | Active |
| [OPTIGA™ Trust M Power management](https://github.com/Infineon/mtb-example-optiga-power-management)                                                      | ModusToolbox™ | Active |
| [OPTIGA™ Trust M Data management](https://github.com/Infineon/mtb-example-optiga-data-management)                                                        | ModusToolbox™ | Active |
| [OPTIGA™ Trust M TLS with mbedTLS](https://github.com/Infineon/mbedtls-optiga-trust-m)                                                                   | Make          | Active |
| [OPTIGA™ Trust M AWS FreeRTOS](https://github.com/Infineon/amazon-freertos-optiga-trust)                                                                 | CMake         | Active |
| [OPTIGA™ Trust M Microsoft Azure IoT](https://github.com/Infineon/azure-esp32-optiga-trust/)                                                             | Make          | Active |
| [OPTIGA™ Trust M PKCS#11 interface](https://github.com/Infineon/pkcs11-optiga-trust-m)                                                                   | CMake         | Active |
| [OPTIGA™ Trust M Matter integration](https://github.com/project-chip/connectedhomeip/tree/v1.1-branch/examples/lock-app/infineon/psoc6#build-trustm-hsm) | Ninja         | Active |
| [OPTIGA™ Trust M Zephyr PAL & Application](https://gitlab.intra.infineon.com/koeberl/zephyr-optiga-trust-m-app)                                          | CMake         | Active |


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Licensing
   
Please see our [LICENSE](LICENSE) for copyright and license information.
   
This project follows the REUSE approach, so copyright and licensing information is available for every file (including third party components) either in the file header, an individual *.license file or the .reuse/dep5 file. All licenses can be found in the [LICENSES](LICENSES) folder.
