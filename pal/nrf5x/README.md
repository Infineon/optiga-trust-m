# General

All projects built with the nrf5x PAL need to set `DL_MAX_FRAME_SIZE=250` via
a global define, because of an EasyDMA restriction on this platform. 

To use the PAL together with other I2C devices and be able to run it also on a
BLE Shield2Go make a global define `IFX_2GO_SUPPORT`.

# Nordic SDK `nrf_crypto` backend

To use the OPTIGA with the `nrf_crypto` API the Nordic SDK needs to be modified
according to the following steps.

Locate the folder <NRF_SDK>/components/libraries/crypto/. In each file listed in
the following table locate the include statement section starting with the
comment `// Include all backends`.

Append the corresponding OPTIGA backend header file include statement at the end
of each respective include block.

|File	                     |Include statement to append        |
|----------------------------|-----------------------------------|
|`nrf_crypto_ecc_backend.h`	 |`#include "optiga_backend_ecc.h"`  |
|`nrf_crypto_ecdh_backend.h` |`#include "optiga_backend_ecdh.h"` |
|`nrf_crypto_ecdsa_backend.h`|`#include "optiga_backend_ecdsa.h"`|
|`nrf_crypto_rng_backend.h`  |`#include "optiga_backend_rng.h"`  |

# Memory Allocation

The library allocates memory on the heap. To ensure correct operation the heap
must be sufficiently large, e.g. 8,192 Bytes.

# Supported Hardware Configurations

The nrf5x PAL supports the Nordic PCA10040 and PCA10056 boards with currently
two different shields. It provides a set of pre-configured pin configurations in
`pal_pin_config.h`. To select a configuration one of the following defines can
be set to `1`:

|Hardware Configuration                         |C Macro                            |
|-----------------------------------------------|-----------------------------------|
|My IoT adapter + Trust X Shield2Go in Slot 1   |`OPTIGA_PIN_CONFIG_MYIOT_SLOT1`    |
|My IoT adapter + Trust X Shield2Go in Slot 2/3 |`OPTIGA_PIN_CONFIG_MYIOT_SLOT2_3`  |
|Trust X Shield onboard OPTIGA                  |`OPTIGA_PIN_CONFIG_TRUSTX_SHIELD`  |
|Trust X Shield + Trust X Shield2Go             |`OPTIGA_PIN_CONFIG_2GO`            |

## My IoT adapter

The My IoT adapter (in Arduino form factor) supports three slots of 2Go form
factor boards. A Trust X Shield2Go can be plugged into any of the three slots.
If the Trust X Shield2Go is plugged into slot 1, select
`OPTIGA_PIN_CONFIG_MYIOT_SLOT1` else use `OPTIGA_PIN_CONFIG_MYIOT_SLOT2_3`.

## Trust X Shield

The Trust X Shield (in Arduino form factor) has an OPTIGA Trust X soldered
directly on the board. It can enable and disable `VDD` for it and select
Trust X Shield2Go board plugged into the respective slot.

If you want to use the on board OPTIGA Trust X, use
`OPTIGA_PIN_CONFIG_TRUSTX_SHIELD`, if you want to use a Trust X Shield2Go
plugged into the `OPTIGA` slot use `OPTIGA_PIN_CONFIG_MYIOT_SLOT2_3`.

### Pin Conflict with Nordic PCA10040 and Trust X Shield

When using the Nordic PCA10040 board with the Trust X Shield the LEDs
BSP_BOARD_LED_1 and BSP_BOARD_LED_2 must not be used. These pins are needed for
the correct operation of the OPTIGA Trust X.
