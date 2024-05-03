# Examples

This folders are used to demonstrate basic functionality of the security chip. 

The list of available Host Applications, Platform and current status can be found in the table below :

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
| [OPTIGA™ Trust M Matter integration](https://github.com/project-chip/connectedhomeip/tree/master/examples/lock-app/infineon/psoc6#building-with-optiga-trust-m-as-hsm) | Ninja         | Active |
| [OPTIGA™ Trust M Zephyr PAL & Application](https://github.com/Infineon/optiga-trust-m-zephyr)                                          | CMake         | Active |

# Troubleshooting

## 0x107 (handshake error)

Some of the examples, for instance [`example_optiga_util_read_data`](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/example_optiga_util_read_data.c#L61) calls
```
// OPTIGA Comms Shielded connection settings to enable the protection
OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_RESPONSE_PROTECTION);
```
Which afterwards raises the **0x107 handshake error**. In most of the cases it happens because the [Pairing](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101#pairing) didn't happen. In this case you need either to remove these two lines of the code, or [pair](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/usecases/example_pair_host_and_optiga_using_pre_shared_secret.c) your device with the secure element.
