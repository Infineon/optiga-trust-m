# General

All projects built with the nrf5x PAL need to set `DL_MAX_FRAME_SIZE=250` via
a global define, because the nrf5x doesn't support the default frame size.

To use the PAL together with other I2C devices and be able to run it also on a BLE Shield2Go 
make a global define `IFX_2GO_SUPPORT`.

# Memory Allocation

The library allocates memory on the heap. To ensure correct operation the heap
must be sufficiently large, e.g. 8,192 Bytes.

# Pin Conflict with Nordic PCA10040 and Trust X Shield

When using the Nordic PCA10040 board with the Trust X Shield the LEDs
BSP_BOARD_LED_1 and BSP_BOARD_LED_2 must not be used. These pins are needed for
the correct operation of the OPTIGA Trust X.
