# AT Monitor

## Overview

The AT monitor sample demonstrates how to use the [AT monitor library] and define AT monitors to receive AT notifications from the [Modem library].

The sample defines two AT monitors, one for network status notifications (`+CEREG`) and one for received signal quality parameters notifications (`+CESQ`) through the `AT_MONITOR()` macro. The sample then subscribes to both notifications and switches the modem to function mode one to register to the network. While the device is registering to the network, the sample uses one of the AT monitors to determine if the registration is complete and monitors the signal quality using the other monitor. Once the device registers with the network, the sample reads the modem PSM mode status, enables it, and reads the PSM mode status again.

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

Use the following steps to build the [AT Monitor] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/at_monitor
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
	AT Monitor sample started
	Subscribing to notifications
	Connecting to network
	Resuming link quality monitor for AT notifications
	Waiting for network
	Link quality: -73 dBm
	Network registration status: searching
	Network registration status: home
	Network connection ready
	Pausing link quality monitor for AT notifications
	Reading PSM info...
	  PSM: enabled
	  Periodic TAU string: 00000110
	  Active time string: 00100001
	Enabling PSM
	Reading PSM info...
	  PSM: enabled
	  Periodic TAU string: 00000110
	  Active time string: 00100001
	Modem response:
	+CEREG: 1,1
	OK
	Shutting down modem
	Network registration status: no network
	Bye
	```

[AT monitor library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/modem/at_monitor.html#at-monitor-readme
[Modem library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrfxlib/nrf_modem/README.html#nrf-modem
[Getting Started Guide]: ../getting-started.md
[AT Monitor]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/at_monitor
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
