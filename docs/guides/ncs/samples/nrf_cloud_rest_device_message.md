# nRF Cloud REST Device Message

## Overview

The nRF Cloud REST Device Message sample demonstrates how to use the [nRF Cloud REST API] to send [Device Messages] using the `SendDeviceMessage` REST endpoint. Every button press sends a message to nRF Cloud.

It also demonstrates use of the [nRF Cloud Alert] and the [nRF Cloud Log] libraries. The sample sends an alert when the device first comes online. It also sends a log message indicating the sample version, as well as when the button is pressed.

You can also configure the sample to use the [nRF Cloud Provisioning Service] with the [nRF Cloud device provisioning library].

## Requirements

Before you start, check that you have the required hardware and software:

- 1x [nRF9151 Connect Kit](https://makerdiary.com/products/nrf9151-connectkit)
- 1x nano-SIM card with LTE-M or NB-IoT support
- 1x U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna (included in the box)
- 1x USB-C Cable
- A computer running macOS, Ubuntu, or Windows 10 or newer

## Don't have an nRF Cloud account?

To connect your device and use nRF Cloud services, you must create an nRF Cloud account:

1. Go to the [nRF Cloud] portal and click __Register__.
2. Enter your email address and choose a password.
3. Click __Create Account__.
4. Check for a verification email from nRF Cloud.

	!!! Tip
		If you do not see the verification email, check your junk mail folder for an email from `no-reply@verificationemail.com`.

5. Copy the six-digit verification code and paste it into the registration dialog box.

	!!! Tip
		If you accidentally closed the registration dialog box, repeat Step 1 and click __Already have a code?__. Enter your email address and verification code.

You can now log in to the nRF Cloud portal with your email and password. After logging in, you can see the __Dashboard__ view that displays your device count and service usage.

## Set up your board

1. Insert the nano-SIM card into the nano-SIM card slot.
2. Attach the U.FL cabled LTE-M/NB-IoT/NR+ Flexible Antenna.
3. Connect the nRF9151 Connect Kit to the computer with a USB-C cable.

![](../../../assets/images/connecting_board_with_lteant.gif)

## Building the sample

To build the sample, follow the instructions in [Getting Started Guide] to set up your preferred building environment.

Use the following steps to build the [nRF Cloud REST Device Message] sample on the command line.

1. Open a terminal window.

2. Go to `NCS-Project/nrf9151-connectkit` repository cloned in the [Getting Started Guide].

3. Build the sample using the `west build` command, specifying the board (following the `-b` option) as `nrf9151_connectkit/nrf9151/ns`.

	``` bash
	west build -p always -b nrf9151_connectkit/nrf9151/ns samples/nrf_cloud_rest_device_message -- -DEXTRA_CONF_FILE=overlay-nrf_provisioning.conf
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

3. Wait for the LTE link to be established. If the device is not yet claimed or properly provisioned on the nRF Cloud, you may experience errors:

	``` { .txt .no-copy linenums="1" title="Terminal" }
	All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	TF-M Float ABI: Hard
	Lazy stacking enabled

	[00:00:00.636,322] <inf> nrf_cloud_rest_device_message: Waiting for network...
	[00:01:23.915,985] <inf> nrf_cloud_rest_device_message: Connected to LTE
	[00:01:23.925,292] <inf> nrf_cloud_rest_device_message: Waiting for modem to acquire network time...
	[00:01:26.937,652] <inf> nrf_cloud_rest_device_message: Network time obtained
	[00:01:26.947,418] <inf> nrf_cloud_log: Changing cloud logging enabled to:1
	[00:01:26.957,000] <inf> nrf_cloud_rest_device_message: Initializing the nRF Provisioning library...
	[00:01:27.005,828] <inf> nrf_provisioning: Checking for provisioning commands in 5s seconds
	[00:01:27.120,697] <inf> nrf_cloud_credentials: Sec Tag: 16842753; CA: Yes, Client Cert: Yes, Private Key: Yes
	[00:01:27.133,300] <inf> nrf_cloud_credentials: CA Size: 1792, AWS: Likely, CoAP: Likely
	[00:01:27.143,981] <inf> nrf_cloud_rest_device_message: nRF Cloud credentials detected
	[00:01:27.155,487] <inf> nrf_cloud_info: Device ID: 5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:27.166,656] <inf> nrf_cloud_info: IMEI:      359404230074347
	[00:01:27.280,883] <inf> nrf_cloud_info: UUID:      5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:27.292,053] <inf> nrf_cloud_info: Modem FW:  mfw_nrf91x1_2.0.2
	[00:01:27.301,086] <inf> nrf_cloud_info: Protocol:          REST
	[00:01:27.309,692] <inf> nrf_cloud_info: Download protocol: HTTPS
	[00:01:27.318,389] <inf> nrf_cloud_info: Sec tag:           16842753
	[00:01:27.327,362] <inf> nrf_cloud_info: Host name:         api.nrfcloud.com
	[00:01:32.017,303] <inf> nrf_cloud_rest_device_message: Provisioning started
	[00:01:32.131,286] <inf> nrf_provisioning_http: Requesting commands
	[00:01:40.804,901] <inf> nrf_cloud_rest_device_message: Sending message:'{"sample_message":"Hello World, from the REST Device Message Sample! Message ID: 1739250022860"}'
	[00:01:41.973,175] <inf> nrf_provisioning_http: Connected
	[00:01:41.981,170] <err> nrf_provisioning_http: Device provided wrong auth credentials
	[00:01:41.991,943] <inf> nrf_cloud_rest_device_message: Provisioning stopped
	[00:01:42.001,586] <wrn> nrf_provisioning: Unauthorized access: device is not yet claimed.
	[00:01:42.012,695] <err> nrf_provisioning: Provisioning failed, error: -13
	[00:01:42.022,460] <inf> nrf_provisioning: Checking for provisioning commands in 86402s seconds
	[00:01:42.242,370] <inf> nrf_cloud_rest_device_message: Message sent
	[00:01:42.251,342] <inf> nrf_cloud_rest_device_message: Sent Hello World message with ID: 1739250022860
	uart:~$
	```

4. Type `nrf_provisioning token` command and press the Enter key ++enter++ to get the device's attestation token.

5. Log in to the [nRF Cloud] portal. Claim the board by performing the following steps:

	1. Select __Security Services__ -> __Claimed Devices__.
	2. Click the __Claim Device__ button. A pop-up opens.
	3. Leave the default type __Single__ selected.
	4. Paste the attestation token into the __Claim Token__ field.
	5. Select __Create new rule for onboarding to nRF Cloud__ so it is toggled on. This enables auto-onboarding and creates a rule that you can use later to onboard additional devices. Creating a rule also adds an associated provisioning group.
	6. Change the name of the rule, if desired.
	7. The default security tag for the nRF Cloud samples is `16842753`. Leave this default value as is for initial onboarding.
	8. Leave the default __Root CA Certificates__ option __All__ selected. This allows the device to use both CoAP and MQTT/REST to connect to nRF Cloud.
	9. The __Device Management Groups__, __Supported Firmware Types__, and __Device Subtype__ fields are optional. You can leave them blank.
	10. Click the __Create Rule and Claim Device__ button.

	![](../../../assets/images/nrf_cloud_claiming_devices.png)

	The device is now claimed and ready to use the nRF Cloud Provisioning Service. It appears in the Claimed Devices list in a provisioning group for the newly created rule.

6. To enforce the connection to the provisioning service, press the __DFU/RST__ button to reset the nRF9151 SiP.

7. Once the device is provisioned and connected, you should see the output, similar to what is shown in the following:

	``` { .txt .no-copy linenums="1" title="Terminal" }
	All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	TF-M Float ABI: Hard
	Lazy stacking enabled

	[00:00:00.636,199] <inf> nrf_cloud_rest_device_message: Waiting for network...
	[00:01:25.119,812] <inf> nrf_cloud_rest_device_message: Connected to LTE
	[00:01:25.129,119] <inf> nrf_cloud_rest_device_message: Waiting for modem to acquire network time...
	[00:01:28.141,510] <inf> nrf_cloud_rest_device_message: Network time obtained
	[00:01:28.151,245] <inf> nrf_cloud_log: Changing cloud logging enabled to:1
	[00:01:28.160,797] <inf> nrf_cloud_rest_device_message: Initializing the nRF Provisioning library...
	[00:01:28.209,686] <inf> nrf_provisioning: Checking for provisioning commands in 5s seconds
	[00:01:28.324,401] <inf> nrf_cloud_credentials: Sec Tag: 16842753; CA: Yes, Client Cert: Yes, Private Key: Yes
	[00:01:28.337,005] <inf> nrf_cloud_credentials: CA Size: 1792, AWS: Likely, CoAP: Likely
	[00:01:27.143,981] <inf> nrf_cloud_rest_device_message: nRF Cloud credentials detected
	[00:01:27.155,487] <inf> nrf_cloud_info: Device ID: 5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:28.370,330] <inf> nrf_cloud_info: IMEI:      359404230074347
	[00:01:28.484,375] <inf> nrf_cloud_info: UUID:      5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:28.495,483] <inf> nrf_cloud_info: Modem FW:  mfw_nrf91x1_2.0.2
	[00:01:28.504,516] <inf> nrf_cloud_info: Protocol:          REST
	[00:01:28.513,092] <inf> nrf_cloud_info: Download protocol: HTTPS
	[00:01:28.521,789] <inf> nrf_cloud_info: Sec tag:           16842753
	[00:01:28.530,731] <inf> nrf_cloud_info: Host name:         api.nrfcloud.com
	[00:01:33.221,130] <inf> nrf_cloud_rest_device_message: Provisioning started
	[00:01:33.333,007] <inf> nrf_provisioning_http: Requesting commands
	[00:01:37.018,554] <inf> nrf_cloud_rest_device_message: Sending message:'{"sample_message":"Hello World, from the REST Device Message Sample! Message ID: 1739250637810"}'
	[00:01:38.885,772] <inf> nrf_provisioning_http: Connected
	[00:01:38.896,850] <inf> nrf_provisioning_http: Processing commands
	[00:01:40.871,307] <inf> nrf_cloud_rest_device_message: Message sent
	[00:01:40.880,249] <inf> nrf_cloud_rest_device_message: Sent Hello World message with ID: 1739250637810
	[00:01:40.955,505] <inf> nrf_provisioning: Disconnected from network - provisioning paused
	[00:02:39.134,582] <inf> nrf_provisioning: Disconnected from network - provisioning paused
	[00:02:41.072,204] <inf> nrf_provisioning: Connected; home network - provisioning resumed
	[00:02:41.083,038] <inf> nrf_cloud_rest_device_message: Modem connection restored
	[00:02:41.093,109] <inf> nrf_cloud_rest_device_message: Waiting for modem to acquire network time...
	[00:02:44.105,712] <inf> nrf_cloud_rest_device_message: Network time obtained
	[00:02:44.115,661] <inf> nrf_provisioning_http: Sending response to server
	[00:02:51.306,182] <inf> nrf_provisioning_http: Requesting commands
	[00:02:53.337,402] <inf> nrf_provisioning_http: Connected
	[00:02:53.348,449] <inf> nrf_provisioning_http: Processing commands
	[00:02:53.422,943] <inf> nrf_provisioning: Disconnected from network - provisioning paused
	[00:03:46.309,051] <inf> nrf_provisioning: Disconnected from network - provisioning paused
	[00:03:48.354,522] <inf> nrf_provisioning: Connected; home network - provisioning resumed
	[00:03:48.365,356] <inf> nrf_cloud_rest_device_message: Modem connection restored
	[00:03:48.375,396] <inf> nrf_cloud_rest_device_message: Waiting for modem to acquire network time...
	[00:03:51.387,878] <inf> nrf_cloud_rest_device_message: Network time obtained
	[00:03:51.397,705] <inf> nrf_provisioning_http: Sending response to server
	[00:03:53.900,726] <inf> nrf_cloud_rest_device_message: Provisioning stopped
	[00:03:53.910,369] <inf> nrf_cloud_rest_device_message: Provisioning done, rebooting...
	[00:03:53.998,901] <inf> nrf_provisioning: Disconnected from network - provisioning paused
	uart:~$ All pins have been configured as non-secure
	Booting TF-M v2.1.0
	[Sec Thread] Secure image initializing!
	TF-M Float ABI: Hard
	Lazy stacking enabled

	[00:00:00.615,325] <inf> nrf_cloud_rest_device_message: Waiting for network...
	[00:01:25.739,044] <inf> nrf_cloud_rest_device_message: Connected to LTE
	[00:01:25.748,352] <inf> nrf_cloud_rest_device_message: Waiting for modem to acquire network time...
	[00:01:28.760,742] <inf> nrf_cloud_rest_device_message: Network time obtained
	[00:01:28.770,477] <inf> nrf_cloud_log: Changing cloud logging enabled to:1
	[00:01:28.780,029] <inf> nrf_cloud_rest_device_message: Initializing the nRF Provisioning library...
	[00:01:28.828,765] <inf> nrf_provisioning: Checking for provisioning commands in 5s seconds
	[00:01:28.943,725] <inf> nrf_cloud_credentials: Sec Tag: 16842753; CA: Yes, Client Cert: Yes, Private Key: Yes
	[00:01:28.956,298] <inf> nrf_cloud_credentials: CA Size: 1792, AWS: Likely, CoAP: Likely
	[00:01:28.966,979] <inf> nrf_cloud_rest_device_message: nRF Cloud credentials detected
	[00:01:28.978,454] <inf> nrf_cloud_info: Device ID: 5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:28.989,624] <inf> nrf_cloud_info: IMEI:      359404230074347
	[00:01:29.103,851] <inf> nrf_cloud_info: UUID:      5034474b-3731-40ab-809f-152c5c11a9a5
	[00:01:29.115,081] <inf> nrf_cloud_info: Modem FW:  mfw_nrf91x1_2.0.2
	[00:01:29.124,114] <inf> nrf_cloud_info: Protocol:          REST
	[00:01:29.132,690] <inf> nrf_cloud_info: Download protocol: HTTPS
	[00:01:29.141,357] <inf> nrf_cloud_info: Sec tag:           16842753
	[00:01:29.150,299] <inf> nrf_cloud_info: Host name:         api.nrfcloud.com
	[00:01:33.840,332] <inf> nrf_cloud_rest_device_message: Provisioning started
	[00:01:33.956,298] <inf> nrf_provisioning_http: Requesting commands
	[00:01:35.709,075] <inf> nrf_cloud_rest_device_message: Sending message:'{"sample_message":"Hello World, from the REST Device Message Sample! Message ID: 1739250870944"}'
	[00:01:40.007,720] <inf> nrf_cloud_rest_device_message: Message sent
	[00:01:40.016,662] <inf> nrf_cloud_rest_device_message: Sent Hello World message with ID: 1739250870944
	[00:01:41.155,944] <inf> nrf_provisioning_http: Connected
	[00:01:41.163,940] <inf> nrf_provisioning_http: No more commands to process on server side
	[00:01:41.174,804] <inf> nrf_cloud_rest_device_message: Provisioning stopped
	[00:01:41.184,478] <inf> nrf_provisioning: Checking for provisioning commands in 86400s seconds
	uart:~$
	```

8. Press __USR/P25__ button to have a message sent to nRF Cloud:

	``` { .txt .no-copy linenums="85" title="Terminal" }
	[00:10:27.922,180] <inf> nrf_cloud_rest_device_message: Button 1 pressed
	[00:10:27.931,610] <inf> nrf_cloud_rest_device_message: Sending message:'{"appId":"BUTTON", "messageType":"DATA", "data":"1"}'
	[00:10:37.257,049] <inf> nrf_cloud_rest_device_message: Message sent
	```

9. After the messages sent, verify that on the nRF Cloud: 

	1. Select __Device Management__ -> __Devices__.
	2. Click the ID of the device you added.
	3. On the device’s page, you should see the messages displayed on the __Terminal__.

	![](../../../assets/images/nrf_cloud_rest_device_messages.png)

[nRF Cloud]: https://nrfcloud.com/
[nRF Cloud REST API]: https://docs.nordicsemi.com/bundle/nrf-cloud/page/APIs/REST/RESTOverview.html
[Device Messages]: https://docs.nordicsemi.com/bundle/nrf-cloud/page/Devices/MessagesAndAlerts/DeviceMessages.html
[nRF Cloud Alert]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/networking/nrf_cloud_alert.html#lib-nrf-cloud-alert
[nRF Cloud Log]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/networking/nrf_cloud_log.html#lib-nrf-cloud-log
[nRF Cloud Provisioning Service]: https://docs.nordicsemi.com/bundle/nrf-cloud/page/SecurityServices/ProvisioningService/ProvisioningOverview.html
[nRF Cloud device provisioning library]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/networking/nrf_provisioning.html#lib-nrf-provisioning
[Getting Started Guide]: ../getting-started.md
[nRF Cloud REST Device Message]: https://github.com/makerdiary/nrf9151-connectkit/tree/main/samples/nrf_cloud_rest_device_message
[Trusted Firmware-M (TF-M)]: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/security/tfm.html#ug-tfm
[PuTTY]: https://apps.microsoft.com/store/detail/putty/XPFNZKSKLBP7RJ
