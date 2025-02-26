# Applications

A set of example applications are provided to show how to implement typical user scenarios using the nRF9151 Connect Kit. Applications typically include a fully integrated software stack and can serve as a starting point for developing your product. They use interface drivers and libraries from the nRF Connect SDK and its set of repositories to implement a specific use case, while [Samples] showcase a single feature or library.

<div class="grid cards" markdown>

-   __[Interface MCU Firmware](./ifmcu.md)__

	Enable a CMSIS-DAP interface for debug and programming, a USB-UART bridge for terminal emulation and an Interface Shell for accessing board-specific functionality.

-   __[Modem Shell](./modem_shell.md)__
	
	Enable you to test various connectivity features of the nRF9151 such as LTE link handling, TCP/IP connections and GNSS, etc.

-   __[Serial LTE Modem](./serial_lte_modem.md)__
	
	Emulate a stand-alone LTE modem on the nRF9151. The application accepts both the modem-specific AT commands and proprietary AT commands.

</div>


[Samples]: ../samples/index.md
