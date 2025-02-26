# Modem Battery

## Overview

The Modem Battery sample demonstrates how to obtain the following battery related information from the modem using the [Modem battery library]:

- Modem battery voltage
- Modem battery voltage low level notifications
- Power-off warnings

## How it works

The sample first calls the `modem_battery_low_level_handler_set()` and `modem_battery_pofwarn_handler_set()` functions to set the respective handlers, and then sets up the workqueues used to handle the low level battery voltage notifications and the power-off warning notifications. The sample then initializes the [Modem library]. Next, it enters a state machine that measures the modem battery voltage at every iteration and then executes an activity based on that. The first activity (`init_activity()`) sets the modem to receive-only mode if the battery voltage drops below the low level threshold or to normal mode if it rises above the threshold and updates the state accordingly.

Then, depending on the modem’s battery voltage, the modem switches mode according to the following conditions:

- If the modem is in normal mode and the battery voltage drops below the low level threshold, the modem switches to receive-only mode.
- If the modem is in receive-only mode and the battery voltage is below the low level threshold, the application periodically checks the battery voltage and switches state if it rises above the low level threshold.
- If the modem is in receive-only mode and the battery voltage is above the low level threshold, the application executes a connectivity evaluation and if the conditions are either normal (`7`), good (`8`), or excellent (`9`), the modem switches to normal mode. When connectivity conditions are poor, the number of packets re-transmitted is higher thus causing an increase in power consumption, which must be avoided when battery voltage is not at an adequately safe level. Full connectivity is recommended when battery voltage is at a sufficient level.
- If the modem is in normal mode and the battery voltage is above the low level threshold, the application executes some IP traffic (DNS lookup) to maintain an active LTE connection. The modem performs background monitoring of the battery voltage and sends a notification of low level in case it drops below the set threshold.
- If the modem battery voltage drops below the power-off warning level, the modem is automatically set offline by the hardware.
- If the modem battery voltage rises above the power-off warning level, the modem is set to receive-only mode and initialized.

## Requirements

Before you start, check that you have the required hardware and software:

- 1x [nRF9151 Connect Kit](https://makerdiary.com/products/nrf9151-connectkit)
- 1x nano-SIM card with LTE-M or NB-IoT support
- 1x U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna (included in the box)
- 1x USB-C Cable
- A computer running macOS, Ubuntu, or Windows 10 or newer

## Set up your board

1. Insert the nano-SIM card into the nano-SIM card slot.
2. Attach the U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna.
3. Connect the nRF9151 Connect Kit to the computer with a USB-C cable.

![](../../../assets/images/connecting_board_with_lteant.gif)

## Building the sample

To build the sample, follow the instructions in [Getting Started Guide] to set up your preferred building environment.

Use the following steps to build the [Modem Battery] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/modem_battery
	```

	The `-p` always option forces a pristine build, and is recommended for new users. Users may also use the `-p auto` option, which will use heuristics to determine if a pristine build is required, such as when building another sample.

	!!! Note
		This sample has Cortex-M Security Extensions (CMSE) enabled and separates the firmware between Non-Secure Processing Environment (NSPE) and Secure Processing Environment (SPE). Because of this, it automatically includes the [Trusted Firmware-M (TF-M)].

4. After building the sample successfully, the firmware with the name `merged.hex` can be found in the `build` directory.

## Flashing the firmware

[Set up your board](#set-up-your-board) before flashing the firmware. You can flash the sample using `west flash`:

``` bash
west flash
```

!!! Tip
	In case you wonder, the `west flash` will execute the following command:

	``` bash
	pyocd load --target nrf91 --frequency 4000000 build/merged.hex
	```

## Testing

After programming the sample, test it by performing the following steps:

1. Open up a serial terminal, specifying the correct serial port that your computer uses to communicate with the nRF9151 SiP:

	=== "Windows"

		1. Start [PuTTY].
		2. Configure the correct serial port and click __Open__:

			![](../../../assets/images/putty-settings.png)

	=== "macOS"

		Open up a terminal and run:

		``` bash
		screen <serial-port-name> 115200
		```

	=== "Ubuntu"

		Open up a terminal and run:

		``` bash
		screen <serial-port-name> 115200
		```

2. Press the __DFU/RST__ button to reset the nRF9151 SiP.

3. Observe the output of the terminal. You should see the output, similar to what is shown in the following:

	``` { .txt .no-copy linenums="1" title="Terminal" }
	All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	*** Booting nRF Connect SDK v2.9.99-98a5e50b9ac1 ***
	*** Using Zephyr OS v3.7.99-693769a5c735 ***
	Battery sample started
	Initializing modem library
	Battery voltage: 4520
	Setting modem to normal mode...
	Normal mode set.
	Connected.
	Battery voltage: 4524
	Executing DNS lookup for 'example.com'...
	Battery voltage: 4520
	Executing DNS lookup for 'google.com'...
	Battery voltage: 4516
	Executing DNS lookup for 'apple.com'...
	Battery voltage: 4516
	Executing DNS lookup for 'amazon.com'...
	Battery voltage: 4520
	Executing DNS lookup for 'microsoft.com'...
	...
	```

[Modem battery library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/modem/modem_battery.html#modem-battery-readme
[Modem library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrfxlib/nrf_modem/README.html#nrf-modem
[Getting Started Guide]: ../getting-started.md
[Modem Battery]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/modem_battery
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
