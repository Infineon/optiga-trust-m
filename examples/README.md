# Examples

This folders are used to demonstrate basic functionality of the security chip. 

The list of available Application Notes and Examples is following:

1. [Get started guide](https://github.com/Infineon/getstarted-optiga-trust-m)
1. [Off-Chip TLS example (mbedTLS)](https://github.com/Infineon/mbedtls-optiga-trust-m)
1. [AWS FreeRTOS example](https://github.com/Infineon/amazon-freertos-optiga-trust)
1. Secure Firmware Update and Secure Boot (link is pending)
1. [Arduino library](https://github.com/Infineon/arduino-optiga-trust-m)
1. [Personalize OPTIGA™ Trust](https://github.com/Infineon/personalize-optiga-trust)
1. [OpenSSL Engine and Command Line Interface (for RPi3)](https://github.com/Infineon/cli-optiga-trust-m)
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
Which afterwards raises the **0x107 handshake error**. In most of the cases it happens because the [Pairing](https://github.com/Infineon/optiga-trust-m/wiki/Shielded-Connection-101#pairing) didn't happen. In this case you need either to remove these two lines of the code, or pair your device with the secure element, it described [here](https://github.com/Infineon/optiga-trust-m/blob/master/examples/optiga/usecases/example_pair_host_and_optiga_using_pre_shared_secret.c).

## Fail to initialise the chip/execute a command

All examples required a common initialisation sequence, for instance like [here](https://github.com/Infineon/getstarted-optiga-trust-m/blob/master/xmc4800_iot_kit/optiga_shell.c#L85). Generic sequence and a code sample is given [here](https://github.com/Infineon/optiga-trust-m/wiki/Initialisation-hints)
