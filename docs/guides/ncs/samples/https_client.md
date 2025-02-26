# HTTPS Client

## Overview

The HTTPS Client sample demonstrates a minimal implementation of HTTP communication. It shows how to set up a TLS session towards an HTTPS server and how to send an HTTP request.

## How it works

The sample first initializes the [Modem library] and AT communications. Next, it provisions a root CA certificate to the modem using the [Modem key management library]. The sample then establishes a connection to the network, sets up the necessary TLS socket options, and connects to an HTTPS server. It sends an HTTP HEAD request and prints the response code in the terminal.

The sample connects to `example.com`, which requires an X.509 certificate. This certificate is provided in the `https_client/cert` folder. The certificate is automatically converted to a HEX format in the `CMakeLists.txt` file. The generated `.inc` file is then included in the code, where it is provisioned to the modem.

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

Use the following steps to build the [HTTPS Client] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/https_client
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
	HTTPS client sample started
	Bringing network interface up
	Provisioning certificate
	Certificate mismatch
	Provisioning certificate to the modem
	Connecting to the network
	+CGEV: EXCE STATUS 0
	%MDMEV: SEARCH STATUS 1
	+CEREG: 2,"1D23","0D70394D",9
	%MDMEV: PRACH CE-LEVEL 0
	+CSCON: 1
	+CGEV: ME PDN ACT 0,0
	+CNEC_ESM: 50,0
	%MDMEV: SEARCH STATUS 2
	+CEREG: 1,"1D23","0D70394D",9,,,"00100001","00000110"
	Network connectivity established and IP address assigned
	Looking up example.com
	Resolved 23.215.0.136 (AF_INET)
	Connecting to example.com:443
	Sent 61 bytes
	Received 345 bytes

	>        HTTP/1.1 200 OK

	Finished, closing socket.
	+CEREG: 0
	+CGEV: ME DETACH
	+CSCON: 0
	Network connectivity lost
	Disconnected from the network
	```

[Modem library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrfxlib/nrf_modem/README.html#nrf-modem
[Modem key management library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/modem/modem_key_mgmt.html#modem-key-mgmt
[Getting Started Guide]: ../getting-started.md
[HTTPS Client]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/https_client
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
