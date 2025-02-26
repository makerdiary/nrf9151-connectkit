# AT Client

## Overview

The AT Client sample demonstrates the asynchronous serial communication taking place over UART to the nRF9151 modem. It acts as a proxy for sending directives to the modem using AT commands. This facilitates the reading of responses or analyzing of events related to the modem.

For more information on the AT commands, see the [nRF91x1 AT Commands Reference Guide].


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

Use the following steps to build the [AT Client] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/at_client
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

3. Once the sample starts running, you should see the following output:

	``` { .bash .no-copy linenums="1" title="Terminal" }
	All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	*** Booting nRF Connect SDK v2.9.99-98a5e50b9ac1 ***
	*** Using Zephyr OS v3.7.99-693769a5c735 ***
	The AT host sample started
	Ready
	```

4. Test the following commands from the terminal:

	``` bash linenums="1"
	AT+CGMI	# (1)!
	```

	1.  This command retrieves the manufacturer identification.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		Nordic Semiconductor ASA
		OK
		```

	``` bash linenums="2"
	AT+CGMM # (1)!
	```

	1.  This command retrieves the System in Package (SiP) model identification.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		nRF9151-LACA
		OK
		```

	``` bash linenums="3"
	AT+CGMR # (1)!
	```

	1.  This command retrieves the modem firmware revision identification.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		mfw_nrf91x1_2.0.2
		OK
		```

	``` bash linenums="4"
	AT+CGSN=1 # (1)!
	```

	1.  This command retrieves the IMEI.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		+CGSN: "35940423XXXXXXX"
		OK
		```

	``` bash linenums="5"
	AT+CFUN? # (1)!
	```

	1.  This command reads the current functional mode.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		+CFUN: <fun>
		OK
		```

		The read response parameter and its defined value are the following:

		__`<fun>`__

		* 0 – Minimum functionality mode. RF circuits are disabled by deactivating LTE and GNSS services.
		* 1 – Normal mode. The active mode is either LTE or GNSS, or both. Full functional mode. Active modes depend on `%XSYSTEMMODE` setting.
		* 2 – Receive only mode in active system mode. Active modes depend on `%XSYSTEMMODE` setting.
		* 4 – Flight mode. RF circuits are disabled by deactivating LTE and GNSS services.
		* 21 – LTE is activated.
		* 31 – GNSS is activated.
		* 41 – UICC is activated.

	``` bash linenums="6"
	AT+CFUN=1 # (1)!
	```

	1.  This command sets the functional mode of the modem to normal.
		``` { .bash .no-copy title="Terminal" }
		OK
		```

	``` bash linenums="7"
	AT+CIMI # (1)!
	```

	1.  This command reads the IMSI from the SIM card.
		``` { .bash .no-copy linenums="1" title="Terminal" }
		4600446364XXXXX
		OK
		```

!!! Tip
	You can refer to the [nRF91x1 AT Commands Reference Guide] to test more AT commands.


[nRF91x1 AT Commands Reference Guide]: https://docs.nordicsemi.com/bundle/ref_at_commands_nrf91x1/page/REF/at_commands/intro_nrf91x1.html
[Getting Started Guide]: ../getting-started.md
[AT Client]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/at_client
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ