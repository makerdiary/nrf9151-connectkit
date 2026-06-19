# CoAP Client

## Overview

The CoAP Client sample demonstrates the communication between a public CoAP server and a CoAP client application that is running on the nRF9151 Connect Kit.

## How it works

The CoAP Client sample performs the following actions:

- Connect to the configured public CoAP test server (specified by the Kconfig option `CONFIG_COAP_SERVER_HOSTNAME`).
- Send periodic GET request for a test resource (specified by the Kconfig option `CONFIG_COAP_RESOURCE`) that is available on the server.
- Display the received data about the resource on a terminal emulator.

The public CoAP server used in this sample is Californium CoAP server (`coap://californium.eclipseprojects.io:5683`). This server runs Eclipse Californium, which is an open source implementation of the CoAP protocol that is targeted at the development and testing of IoT applications.

This sample uses the resource __obs__ (Californium observable resource) in the communication between the CoAP client and the public CoAP server. The communication follows the standard request/response pattern and is based on the change in the state of the value of the resource. The sample queries one resource at a time. To configure other resources, use the Kconfig option `CONFIG_COAP_RESOURCE`.

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

Use the following steps to build the [CoAP Client] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/coap_client
	```

	The `-p` always option forces a pristine build, and is recommended for new users. Users may also use the `-p auto` option, which will use heuristics to determine if a pristine build is required, such as when building another sample.

	!!! Note
		This sample has Cortex-M Security Extensions (CMSE) enabled and separates the firmware between Non-Secure Processing Environment (NSPE) and Secure Processing Environment (SPE). Because of this, it automatically includes the [Trusted Firmware-M (TF-M)].

4. After building the sample successfully, the firmware with the name `tfm_merged.hex` can be found in the `build/coap_client/zephyr` directory.

## Flashing the firmware

[Set up your board](#set-up-your-board) before flashing the firmware. You can flash the sample using `west flash`:

``` bash
west flash
```

!!! Tip
	In case you wonder, the `west flash` will execute the following command:

	``` bash
	pyocd load --target nrf91 --frequency 4000000 build/coap_client/zephyr/tfm_merged.hex
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
	[INF] All pins have been configured as non-secure
	[NOT] Booting TF-M v2.3.0**
	[NOT] Built Thu 18 Jun 2026 07:02:04 UTC
	*** Booting nRF Connect SDK v3.3.99-95ed8f7e7406 ***
	*** Using Zephyr OS v4.4.0-14033cef1f73 ***
	[00:00:00.258,605] <inf> coap_client_sample: The CoAP client sample started
	[00:00:00.258,636] <inf> coap_client_sample: Bringing network interface up and connecting to the network
	[00:00:00.590,240] <inf> coap_client_sample: Waiting for network connectivity
	[00:01:20.251,007] <inf> coap_client_sample: Network connectivity established
	[00:01:21.055,175] <inf> coap_client_sample: IPv4 Address found 20.47.97.44
	[00:01:21.055,480] <inf> coap_client_sample: Initializing CoAP client
	[00:01:21.057,678] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:21.418,334] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:04:42
	[00:01:26.060,058] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:26.587,524] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:04:47
	[00:01:31.062,377] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:31.710,662] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:04:52
	[00:01:36.064,605] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:36.507,812] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:04:57
	[00:01:41.066,802] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:41.631,958] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:05:02
	[00:01:46.068,969] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:46.747,100] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:05:07
	[00:01:51.071,228] <inf> coap_client_sample: CoAP GET request sent sent to californium.eclipseprojects.io, resource: obs
	[00:01:51.558,258] <inf> coap_client_sample: CoAP response: code: 0x45, payload: 07:05:12
	...
	```

[Getting Started Guide]: ../getting-started.md
[CoAP Client]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/coap_client
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
