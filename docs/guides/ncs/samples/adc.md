# ADC

## Overview

This sample demonstrates how to use the ADC driver API. It reads ADC samples from two channels specified in the devicetree and prints the readings on the console. If voltage of the used reference can be obtained, the raw readings are converted to millivolts.

Here is a minimal devicetree fragment which supports this sample.

``` dts linenums="1" title="nrf9151_connectkit_nrf9151.overlay"
/ {
	zephyr,user {
		io-channels = <&adc 0>, <&adc 1>;
	};
};

&adc {
	#address-cells = <1>;
	#size-cells = <0>;

	channel@0 {
		reg = <0>;
		zephyr,gain = "ADC_GAIN_1_6";
		zephyr,reference = "ADC_REF_INTERNAL";
		zephyr,acquisition-time = <ADC_ACQ_TIME_DEFAULT>;
		zephyr,input-positive = <NRF_SAADC_AIN0>; /* P0.13 */
		zephyr,resolution = <14>;
		zephyr,oversampling = <8>;
	};

	channel@1 {
		reg = <1>;
		zephyr,gain = "ADC_GAIN_1_6";
		zephyr,reference = "ADC_REF_INTERNAL";
		zephyr,acquisition-time = <ADC_ACQ_TIME_DEFAULT>;
		zephyr,input-positive = <NRF_SAADC_VDD>;
		zephyr,resolution = <14>;
		zephyr,oversampling = <8>;
	};
};
```

As shown, __AIN0__ is assigned to Channel 0, and __VDD_GPIO__ to Channel 1.

## Requirements

Before you start, check that you have the required hardware and software:

- 1x [nRF9151 Connect Kit](https://makerdiary.com/products/nrf9151-connectkit)
- 1x USB-C Cable
- A computer running macOS, Ubuntu, or Windows 10 or newer

## Building the sample

To build the sample, follow the instructions in [Getting Started Guide] to set up your preferred building environment.

Use the following steps to build the [ADC] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151 samples/adc
	```

	The `-p` always option forces a pristine build, and is recommended for new users. Users may also use the `-p auto` option, which will use heuristics to determine if a pristine build is required, such as when building another sample.

	!!! Tip
		You can also build this sample for the `nrf9151_connectkit/nrf9151/ns` target. In such a case, the sample has Cortex-M Security Extensions (CMSE) enabled and separates the firmware between Non-Secure Processing Environment (NSPE) and Secure Processing Environment (SPE). Because of this, it automatically includes the [Trusted Firmware-M (TF-M)].

		``` bash
		west build -p always -b nrf9151_connectkit/nrf9151/ns samples/adc
		```

4. After building the sample successfully, the firmware with the name `merged.hex` can be found in the `build` directory.

## Flashing the firmware

Connect the nRF9151 Connect Kit to the computer with a USB-C cable:

![](../../../assets/images/connecting_board.png)

Then flash the sample using `west flash`:

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
	*** Booting nRF Connect SDK v2.9.99-98a5e50b9ac1 ***
	*** Using Zephyr OS v3.7.99-693769a5c735 ***
	ADC reading[0]:
	- adc@e000, channel 0: 2379 = 522 mV
	- adc@e000, channel 1: 15070 = 3311 mV
	ADC reading[1]:
	- adc@e000, channel 0: 2392 = 525 mV
	- adc@e000, channel 1: 15069 = 3311 mV
	ADC reading[2]:
	- adc@e000, channel 0: 2354 = 517 mV
	- adc@e000, channel 1: 15074 = 3312 mV
	ADC reading[3]:
	- adc@e000, channel 0: 2365 = 519 mV
	- adc@e000, channel 1: 15065 = 3310 mV
	...
	```

[Getting Started Guide]: ../getting-started.md
[ADC]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/adc
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ