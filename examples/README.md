# Examples

This folders are used to demonstrate basic functionality of the security chip. 

The list of available Application Notes and Examples is following:

1. [Get started guide](https://github.com/Infineon/getstarted-optiga-trust-m)
1. ModusToolbox™ Code Examples
    - [OPTIGA™ Trust M: Cryptography](https://github.com/Infineon/mtb-example-optiga-crypto)
    - [OPTIGA™ Trust M: MQTT Client](https://github.com/Infineon/mtb-example-optiga-mqtt-client)
    - [OPTIGA™ Trust M: Power management](https://github.com/Infineon/mtb-example-optiga-power-management)
    - [OPTIGA™ Trust M: Data management](https://github.com/Infineon/mtb-example-optiga-data-management)
1. [Off-Chip TLS example (mbedTLS)](https://github.com/Infineon/mbedtls-optiga-trust-m)
1. [Linux Command Line Interface](https://github.com/Infineon/linux-optiga-trust-m)
1. Cloud:
    1. [AWS FreeRTOS example](https://github.com/Infineon/amazon-freertos-optiga-trust)
    1. [Microsoft Azure IoT example](https://github.com/Infineon/azure-optiga-trust-m)
1. [Zephyr OS driver](https://github.com/Infineon/zephyr)
1. [Arduino library](https://github.com/Infineon/arduino-optiga-trust-m)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)
1. [Python package](https://github.com/Infineon/python-optiga-trust)
1. [I2C Utilities](https://github.com/Infineon/i2c-utils-optiga-trust)

# Troubleshooting

## 0x107 (handshake error)

Some of the examples, for instance [`example_optiga_util_read_data`](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/example_optiga_util_read_data.c#L61) calls 
```
// OPTIGA Comms Shielded connection settings to enable the protection
OPTIGA_UTIL_SET_COMMS_PROTOCOL_VERSION(me, OPTIGA_COMMS_PROTOCOL_VERSION_PRE_SHARED_SECRET);
OPTIGA_UTIL_SET_COMMS_PROTECTION_LEVEL(me, OPTIGA_COMMS_RESPONSE_PROTECTION);
```
Which afterwards raises the **0x107 handshake error**. In most of the cases it happens because the [Pairing](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101#pairing) didn't happen. In this case you need either to remove these two lines of the code, or [pair](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/usecases/example_pair_host_and_optiga_using_pre_shared_secret.c) your device with the secure element.

## Fail to initialise the chip/execute a command

All examples require a common initialisation sequence, for instance like [here](https://github.com/Infineon/getstarted-optiga-trust-m/blob/master/xmc4800_iot_kit/optiga_shell.c#L85). Generic sequence and a code sample is given [here](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
