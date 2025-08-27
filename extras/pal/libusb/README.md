# libusb Platform Abstraction Layer (PAL)

This PAL uses the [libusb]](https://github.com/libusb/libusb) library for communication to the OPTIGA™ Trust M device via a USB-to-UART-interface (e.g. FTDI).

## Prerequisites

### Linux

#### Installation of the libusb library

Under Linux, libusb can be installed with the following command.

```bash
sudo apt-get install libusb-1.0-0-dev
```

#### Access to the USB device

To access the USB-to-UART-interface as a normal user, the following steps have to be done.

1. Add user to group `plugdev`

```bash
sudo usermod -a -G plugdev <user>
```

2. Add a udev rule for the USB device

See [udev/90-optigatrust.rules](./udev/90-optigatrust.rules) for the udev rule of the OPTIGA™ Trust M Perso2Go board.

To create your own custom udev rule, see [udev/README.md](./udev/README.md).

Add the udev rule to `/etc/udev/rules.d` (root permission needed).

```bash
sudo cp ./udev/90-optigatrust.rules /etc/udev/rules.d
```

Run the following command to reload the udev rules.

```bash
udevadm control --reload-rules
```

## References

* [libusb project page](https://libusb.info/)
* [libusb GitHub repository](https://github.com/libusb/libusb)