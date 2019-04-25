# General

All projects built with the nrf5x PAL need to set `DL_MAX_FRAME_SIZE=250` via
a global define, because the nrf5x doesn't support the default frame size.

To use the PAL together with other I2C devices and be able to run it also on a BLE Shield2Go 
make a global define `IFX_2GO_SUPPORT`.

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

# Pin Conflict with Nordic PCA10040 and Trust X Shield

When using the Nordic PCA10040 board with the Trust X Shield the LEDs
BSP_BOARD_LED_1 and BSP_BOARD_LED_2 must not be used. These pins are needed for
the correct operation of the OPTIGA Trust X.
