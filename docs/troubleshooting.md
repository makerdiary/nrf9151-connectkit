# Troubleshooting

It may happen that you run into trouble while getting started with your new nRF9151 Connect Kit. This page is here to help you solve the most common problems. If you have a new problem, you can [create a new issue] or discuss it on [GitHub Discussions].

## Which COM port is connected to nRF9151 SiP?

The Interface MCU exposes two COM ports, the first one acts as a UART bridge between the host and the nRF9151 SiP, and the second one is connected to the Interface Shell whose terminal prompt appears as __`ifsh:~$`__.

## udev rules on Linux

On Linux, particularly Ubuntu 16.04+, you must configure udev rules to allow pyOCD to access debug probes from user space. Otherwise you will need to run pyOCD as root, using sudo, which is very highly discouraged. (You should never run pyOCD as root on any OS.)

To install, copy the rules file in the [udev] folder. to `/etc/udev/rules.d/` on Ubuntu:

``` bash linenums="1"
sudo cp 50.zephyr-dap.rules /etc/udev/rules.d
```

To see your changes without a reboot, you can force the udev system to reload:

``` bash linenums="2"
sudo udevadm control --reload
```

``` bash linenums="3"
udo udevadm trigger
```

By default, the rules provide open access to the CMSIS-DAP interface for all users (0666 permissions). If you share your Linux system with other users, or just don't like the idea of write permission for everybody, you can replace `MODE:="0666"` with `OWNER:="yourusername"` to create the device owned by you, or with `GROUP:="somegroupname"` and mange access using standard Unix groups.

## How to get a compatible SIM Card?

1. __Choose a Provider__: Identify a mobile network operator (MNO) or IoT service provider that offers LTE-M or NB-IoT services in your region.

2. __Request a SIM Card__: Contact the provider to request a compatible SIM card. Provide details about your use case (e.g., IoT deployment, data requirements).

3. __Activate the SIM Card__: Follow the provider's activation process, which may include registering the SIM card and configuring APN settings.

[create a new issue]: https://github.com/makerdiary/nrf9151-connectkit/issues/new
[GitHub Discussions]: https://github.com/makerdiary/nrf9151-connectkit/discussions
[udev]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/applications/ifmcu_firmware/udev
