# Adding a custom udev rule

## Get information for udev rule

Plug in the USB device and run

```bash
lsusb
```

Example output:
```
Bus 001 Device 004: ID 0403:6030 Future Technology Devices International, Ltd FT260
Bus 001 Device 003: ID 0424:ec00 Microchip Technology, Inc. (formerly SMSC) SMSC9512/9514 Fast Ethernet Adapter
Bus 001 Device 002: ID 0424:9514 Microchip Technology, Inc. (formerly SMSC) SMC9514 Hub
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
```

Select the correct device. In this case, this is the first device, which is a FTDI chip.

Now, print more information based on the bus and the device ID and filter for `idVendor` and `idProduct`.
```bash
lsusb -vs  001:004 | grep -E 'idVendor|idProduct'
```

Example output:
```
  idVendor           0x0403 Future Technology Devices International, Ltd
  idProduct          0x6030
```

## Create the udev rule

Take the data for `idVendor` and `idProduct` from the output and create a file with ending `.rule` in `/etc/udev/rules.d`.

```
SUBSYSTEM=="usb", ATTRS{idVendor}=="0x0403", ATTRS{idProduct}=="0x6030", MODE="0666", ACTION=="add"
```

## Further information

For further information, please consult the documenation of your Linux OS.

* [ubuntuusers Wiki - udev](https://wiki.ubuntuusers.de/udev/) (German only, but very extensive)
* [Ubuntu udev manpage](https://manpages.ubuntu.com/manpages/noble/man7/udev.7.html)
* [Debian udev](https://wiki.debian.org/udev)

