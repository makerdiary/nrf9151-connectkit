# Update the nRF9151 Firmware

## Overview

This guide describes how you can update the application firmware and modem firmware of the nRF9151 SiP on the nRF9151 Connect Kit through the Interface MCU.

## Requirements

Before you start, check that you have the required hardware and software:

- 1x [nRF9151 Connect Kit](https://makerdiary.com/products/nrf9151-connectkit)
- 1x USB-C Cable
- [Python] 3.10.0 or newer
- [pyOCD v0.36.0] or newer
- A computer running macOS, Ubuntu, or Windows 10 or newer

!!! Tip
	Python and pyOCD will be installed automatically after setting up your development environment in [Getting Started Guide].

## Installing pyOCD

To program the nRF9151 SiP, a tool that supports the CMSIS-DAP protocol should be installed. [pyOCD v0.36.0] or newer is highly recommended.

The latest stable version of pyOCD may be installed via [pip]. Open up a terminal and run:

=== "Windows"

    ``` bat
    py -3 -m pip install -U pyocd
    ```

=== "macOS"

    ``` bash
    python3 -m pip install -U pyocd
    ```

=== "Ubuntu"

    ``` bash
    python3 -m pip install -U pyocd
    ```

## Connecting the board

Connect the nRF9151 Connect Kit to your computer with a USB-C cable.

![](../../assets/images/connecting_board.png)

Enter the following command to list the connected device:

``` bash
pyocd list
```

You should see the output, similar to what is shown in the following:

``` { .bash .no-copy linenums="1" title="Terminal" }
#   Probe/Board              Unique ID               Target
----------------------------------------------------------------
0   ZEPHYR IFMCU CMSIS-DAP   820D9A5F0E38432F280DB   ✔︎ nrf91
	Makerdiary               nRF9151 Connect Kit
```

## Updating the application firmware

To update the nRF9151 appication firmware using pyOCD, complete the following steps:

1. Enter the following command to program the application firmware to the nRF9151 application core:

	``` bash
	pyocd load -t nrf91 <application.hex> # (1)!
	```

	1.  You can also use `pyocd flash` instead:
		``` bash
		pyocd flash -t nrf91 <application.hex>
		```

2. The RGB LED blinks blue rapidly during programming.
3. The application starts running after the firmware programming is completed. If not, press the __DFU/RST__ button to reset the nRF9151 SiP.

## Updating the modem firmware

The nRF9151 Connect Kit comes pre-programmed with the modem firmware [mfw_nrf91x1_2.0.2.zip]. To ensure optimal performance, you can update the modem firmware to the latest version.

Follow these steps to update the modem firmware:

1. [Download](https://www.nordicsemi.com/Products/nRF9151/Download?lang=en#infotabs) the latest modem firmware from the Nordic Semiconductor official website.
2. Open a terminal window.
3. Change to the directory where the new modem firmware is located.
4. Enter the following command to program the new modem firmware on the nRF9151 SiP:

	``` bash
	pyocd cmd -t nrf91 -c 'nrf91-update-modem-fw -f mfw_nrf91x1_2.0.2.zip'
	```

	If `-f` is specified, the modem firmware is written to the device, even if the correct version is already present.

5. The RGB LED blinks blue rapidly during programming.
6. To verify the modem firmware version, you can program a sample with AT commands, for example, [AT Client], and then run the following command:

	``` bash
	AT+CGMR # (1)!
	```

	1.  This command retrieves the modem firmware revision identification.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		mfw_nrf91x1_2.0.2
		OK
		```

[Python]: https://www.python.org/downloads/
[pyOCD v0.36.0]: https://github.com/pyocd/pyOCD/releases/tag/v0.36.0
[Getting Started Guide]: ../ncs/getting-started.md
[pip]: https://pip.pypa.io/en/stable/index.html
[mfw_nrf91x1_2.0.2.zip]: https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sip/nrf91x1-sip/nrf91x1-lte-modem-firmware/mfw_nrf91x1_2.0.2.zip
[AT Client]: ../ncs/samples/at_client.md
