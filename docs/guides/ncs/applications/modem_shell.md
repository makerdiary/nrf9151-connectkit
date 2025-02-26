# Modem Shell

## Overview

The Modem Shell (MoSh) application enables you to test various connectivity features of the nRF9151 such as LTE link handling, TCP/IP connections, data throughput (iperf3 and curl), SMS, GNSS, FOTA updates and PPP.

MoSh uses the LTE link control driver to establish an LTE connection and initializes the Zephyr shell to provide a shell command-line interface for users.

!!! Tip
	The nRF9151 Connect Kit comes pre-programmed with the [Modem Shell] application, enabling you to explore its various features right out of the box.

## Requirements

Before you start, check that you have the required hardware and software:

- 1x [nRF9151 Connect Kit](https://makerdiary.com/products/nrf9151-connectkit)
- 1x nano-SIM card with LTE-M or NB-IoT support
- 1x U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna (included in the box)
- 1x U.FL cabled GNSS Antenna (included in the box)
- 1x USB-C Cable
- A computer running macOS, Ubuntu, or Windows 10 or newer

## Set up your board

1. Insert the nano-SIM card into the nano-SIM card slot.
2. Attach the U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna.
3. Attach the U.FL cabled GNSS Antenna.
3. Connect the nRF9151 Connect Kit to the computer with a USB-C cable.

![](../../../assets/images/connecting_board_all.gif)

## Building the application

To build the application, follow the instructions in [Getting Started Guide] to set up your preferred building environment.

Use the following steps to build the [Modem Shell] application on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the application using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns applications/modem_shell
	```

	The `-p` always option forces a pristine build, and is recommended for new users. Users may also use the `-p auto` option, which will use heuristics to determine if a pristine build is required, such as when building another application.

	!!! Note
		This application has Cortex-M Security Extensions (CMSE) enabled and separates the firmware between Non-Secure Processing Environment (NSPE) and Secure Processing Environment (SPE). Because of this, it automatically includes the [Trusted Firmware-M (TF-M)].

4. After building the application successfully, the firmware with the name `merged.hex` can be found in the `build` directory.

## Flashing the firmware

[Set up your board](#set-up-your-board) before flashing the firmware. You can flash the application using `west flash`:

``` bash
west flash
```

!!! Tip
	In case you wonder, the `west flash` will execute the following command:

	``` bash
	pyocd load --target nrf91 --frequency 4000000 build/merged.hex
	```

## Testing

After programming the application, test it by performing the following steps:

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

3. Wait for the LTE link to be established. Observe the output of the terminal. You should see the output, similar to what is shown in the following:

	``` { .txt .no-copy linenums="1" title="Terminal" }
	All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	TF-M isolation level is: 0x00000001
	TF-M Float ABI: Hard
	Lazy stacking enabled
	*** Booting nRF Connect SDK v2.9.99-98a5e50b9ac1 ***
	*** Using Zephyr OS v3.7.99-693769a5c735 ***

	Reset reason: PIN reset
	mosh:~$
	MOSH version:       v2.9.99-98a5e50b9ac1
	MOSH build id:      custom
	MOSH build variant: dev
	HW version:         nRF9151 LACA A0A
	Modem FW version:   mfw_nrf91x1_2.0.2
	Modem FW UUID:      320176d5-9f40-45fc-923b-2661ec18d547


	Modem domain event: Light search done
	Network registration status: searching
	Currently active system mode: NB-IoT
	LTE cell changed: ID: 180539199, Tracking area: 7464
	Modem domain event: CE-level 0
	RRC mode: Connected
	PDN event: PDP context 0 activated
	PDN event: PDP context 0, PDN type IPv4 only allowed
	Modem domain event: Search done
	Network registration status: Connected - home network
	PSM parameter update: TAU: 1800, Active time: -1 seconds
	Modem config for system mode: LTE-M - NB-IoT - GNSS
	Modem config for LTE preference: LTE-M is preferred, but PLMN selection is more important
	Currently active system mode: NB-IoT
	Battery voltage:       4520 mV
	Modem temperature:     28 C
	Device ID:             nrf-359404230074347
	Operator full name:   ""
	Operator short name:  ""
	Operator PLMN:        "46000"
	Current cell id:       180539199 (0x0AC2CF3F)
	Current phy cell id:   367
	Current band:          8
	Current TAC:           7464 (0x1D28)
	Current rsrp:          74: -67dBm
	Current snr:           35: 11dB
	Mobile network time and date: 25/02/12,14:49:28+32
	PDP context info 1:
	CID:                0
	PDN ID:             0
	PDP context active: yes
	PDP type:           IP
	APN:                cmnbiot
	IPv4 MTU:           1280
	IPv4 address:       100.19.132.199
	IPv6 address:       ::
	IPv4 DNS address:   120.196.165.7, 221.179.38.7
	IPv6 DNS address:   ::, ::
	RRC mode: Idle
	mosh:~$
	```

4. Type ++tab++ to list all supported commands. When you type a command with `--help`, the terminal shows its usage, for example `ping --help`:

	``` { .txt .no-copy linenums="59" title="Terminal" }
	mosh:~$
	at           clear        cloud_rest   curl         date         device       dl           fota         gnss
	gpio_count   heap         help         history      iperf3       kernel       link         location     ping
	print        rem          resize       rest         retval       shell        sleep        sms          sock
	startup_cmd  th           uart         version
	mosh:~$ ping --help
	Usage: ping [options] -d destination

	-d, --destination, [str] Name or IP address
	Options:
	-t, --timeout, [int]     Ping timeout in milliseconds
	-c, --count, [int]       The number of times to send the ping request
	-i, --interval, [int]    Interval between successive packet transmissions
							in milliseconds
	-l, --length, [int]      Payload length to be sent
	-I, --cid, [int]         Use this option to bind pinging to specific CID.
							See link cmd for interfaces
	-6, --ipv6,              Force IPv6 usage with the dual stack interfaces
	-r, --rai                Set RAI options for ping socket. In order to use RAI,
							it must be enabled with 'link rai' command.
	-h, --help,              Shows this help information
	```

5. Use `ping` command to test the reachability of a host on an IP network. For example:

	``` bash
	ping -d makerdiary.com	# (1)!
	```

	1.  
		``` { .txt .no-copy linenums="1" title="Terminal" }
		mosh:~$ ping -d makerdiary.com
		Initiating ping to: makerdiary.com
		Modem domain event: CE-level 0
		RRC mode: Connected
		Source IP addr: 100.19.132.199
		Destination IP addr: 23.227.38.32
		Pinging makerdiary.com results: time=0.316secs, payload sent: 0, payload received 0
		Pinging makerdiary.com results: time=0.927secs, payload sent: 0, payload received 0
		Pinging makerdiary.com results: time=0.912secs, payload sent: 0, payload received 0
		Pinging makerdiary.com results: time=0.907secs, payload sent: 0, payload received 0
		Ping statistics for makerdiary.com:
			Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
		Approximate round trip times in milli-seconds:
			Minimum = 316ms, Maximum = 927ms, Average = 765ms
		Pinging DONE
		```

6. Use `gnss` command to test the GNSS functionality:

	``` bash
	gnss start
	```

	Observe that the following information is displayed upon acquiring a fix:

	``` { .txt .no-copy linenums="1" title="Terminal" }
	Fix valid:          true
	Leap second valid:  false
	Sleep between PVT:  false
	Deadline missed:    false
	Insuf. time window: false
	Velocity valid:     true
	Scheduled download: false
	Execution time:     52587 ms
	Time:              22.02.2025 07:29:24.141
	Latitude:          22.694009
	Longitude:         113.941902
	Accuracy:          3.5 m
	Altitude:          103.4 m
	Altitude accuracy: 5.5 m
	Speed:             0.1 m/s
	Speed accuracy:    0.6 m/s
	V. speed:          0.2 m/s
	V. speed accuracy: 0.6 m/s
	Heading:           0.0 deg
	Heading accuracy:  180.0 deg
	PDOP:              4.6
	HDOP:              2.6
	VDOP:              3.7
	TDOP:              3.4
	Google maps URL:   https://maps.google.com/?q=22.694009,113.941902
	SV:  22 C/N0: 38.7 el: 42 az: 331 signal: 1 in fix: 1 unhealthy: 0
	SV:  17 C/N0: 37.7 el: 43 az: 325 signal: 1 in fix: 1 unhealthy: 0
	SV:  14 C/N0: 39.7 el: 63 az: 342 signal: 1 in fix: 1 unhealthy: 0
	SV: 195 C/N0: 40.2 el: 63 az:  55 signal: 3 in fix: 1 unhealthy: 0
	SV: 194 C/N0: 38.9 el: 61 az:  92 signal: 3 in fix: 1 unhealthy: 0
	SV:   1 C/N0: 40.1 el: 37 az:  35 signal: 1 in fix: 1 unhealthy: 0
	SV:   3 C/N0: 32.6 el: 40 az:  97 signal: 1 in fix: 1 unhealthy: 0
	SV: 199 C/N0: 31.3 el:  0 az:   0 signal: 3 in fix: 0 unhealthy: 0
	SV:  19 C/N0: 24.5 el:  0 az:   0 signal: 1 in fix: 0 unhealthy: 0
	...
	```

	To turn off the GNSS, run the following command:

	``` bash
	gnss stop
	```

	!!! Tip "Optimize GNSS reception"

		* GNSS signals do not usually penetrate ceilings or other structures that well. For best GNSS performance, the antenna should be placed outside on a flat surface in an open space far from sources of interference and other structures that can block the satellite signals.
		* The GNSS patch antenna achieves the highest gain when placed horizontally on a surface (x-y) facing the z-axis since it can receive all propagated GNSS signals. A lower gain is experienced if the patch antenna is mounted at an angle.

7. Try more commands to explore different connectivity features of the Modem Shell application.

[Modem Shell]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/applications/modem_shell
[Getting Started Guide]: ../getting-started.md
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
