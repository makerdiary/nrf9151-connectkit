# nRF9151 Connect Kit<br/><small>Versatile Prototyping Kit built around the nRF9151 SiP with LTE-M, NB-IoT, GNSS, and DECT NR+</small>

## Introduction

nRF9151 Connect Kit is a versatile prototyping kit built around the [nRF9151] System-in-Package
(SiP) which supports LTE-M, NB-IoT, GNSS, and DECT NR+, making it suitable for global use. It
has LTE bands B1-B5, B8, B12, B13, B17-B20, B25, B26, B28, B65, B66 and B85 support, Arm
TrustZone CryptoCell 310, unique modem features for further power saving and ease of use.

The design comes with an Interface MCU built using the [nRF52820], which enables debug, programming
as well as modem firmware update, no need for any external tools. The Interface MCU also includes a
USB-UART bridge for log, trace and terminal emulation, and an Interface Shell bundled with helpful
commands to access the board-specific functionality.

The board contains a [BQ25180] I2C programmable battery charger with power path management
and ultra low quiescent current, and a [TPS63901] buck-boost converter with 75-nA quiescent
current and 1.8V/3.3V configurable power supply for I/Os.

It also has USB-C, LEDs, Buttons, nano-SIM card slot, Battery connector, U.FL receptacles for U.FL
cabled LTE-M/NB-IoT/NR+ and GNSS antennas, Arm Serial Wire Debug (SWD) port and dual-row 40 pins
with loose or pre-soldered headers available.

[nRF Connect SDK] is supported, including the [Zephyr RTOS], various samples, networking protocols,
libraries, and hardware drivers, all essential for cellular IoT development. It is open source,
and can be leveraged and modified to suit your specific needs.

![](./assets/images/nrf9151_connectkit_prod_hero_reva.png)

## Key Features

* Nordic Semiconductor nRF9151 SiP

	- Fully integrated SiP with 64 MHz Arm Cortex-M33 and multimode LTE-M/NB-IoT modem with
	  DECT NR+ support and GNSS
	- 700-2200 MHz LTE bands: B1-B5, B8, B12, B13, B17-B20, B25, B26, B28, B65, B66, B85
	- Power Class 5 20 dBm
	- Power Class 3 23 dBm
	- 1.9GHz NR+ band support
	- Certified for global operation
	- Dedicated programmable application processor and memory
	- 1 MB Flash + 256 KB RAM
	- Arm TrustZone + Arm CryptoCell 310
	- 4 x SPI/UART/TWI, PDM, I2S, PWM, ADC

* On-board Interface MCU

	- Built using nRF52820 with 64 MHz Arm Cortex-M4, 256 KB Flash & 32 KB RAM
	- Built-in CMSIS-DAP support for debug, programming as well as modem firmware update,
	  no need for any external tools
	- USB-UART bridge for log, trace and terminal emulation
	- Interface Shell with helpful commands to access the board-specific functionality
	- Self-upgradable UF2 Bootloader featuring drag-and-drop programming for Interface MCU firmware update
	- Open source and more features will be released gradually over time

* BQ25180 I2C programmable Battery Charger with power path management and ultra low quiescent current 
* TPS63901 buck-boost converter with 75-nA quiescent current and 1.8V/3.3V configurable power supply for I/Os
* On-board GNSS Low Noise Amplifier (LNA) with 18.2 dB gain and only 0.7 dB noise figure for improved sensitivity
* Shipped with U.FL cabled LTE-M/NB-IoT/NR+ and GNSS antennas
* 31 multi-function GPIOs (8 can be configured as ADC inputs) through edge pins
* Arm Serial Wire Debug (SWD) port through edge pins
* USB-C, LEDs, Buttons, nano-SIM card slot, Battery connector, U.FL receptacles
* Dual-row 40 pins in 55.88mm x 20.32mm (2.2" x 0.8") DIP/SMT form factor
* Available in Loose or Pre-soldered headers options
* Built on open source, supporting nRF Connect SDK and Zephyr RTOS, etc

## Hardware Diagram

[![](./assets/images/nrf9151_connectkit_hw_diagram_front.png)][HW Diagram PDF]

[![](./assets/images/nrf9151_connectkit_hw_diagram_back.png)][HW Diagram PDF]


## What's in the box

|Part                                                      |Quantity |
|----------------------------------------------------------|---------|
|nRF9151 Connect Kit Board (Loose or Pre-soldered Headers) |1        |
|U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna             |1        |
|U.FL cabled GNSS Antenna                                  |1        |
|Quick Start Guide                                         |1        |

[nRF9151]: https://www.nordicsemi.com/Products/nRF9151
[nRF52820]: https://www.nordicsemi.com/Products/nrf52820
[BQ25180]: https://www.ti.com/product/BQ25180
[TPS63901]: https://www.ti.com/product/TPS63901
[nRF Connect SDK]: https://github.com/nrfconnect/sdk-nrf
[Zephyr RTOS]: https://github.com/zephyrproject-rtos/zephyr
[HW Diagram PDF]: ./assets/attachments/nrf9151-connectkit-hardware-diagram_reva.pdf
