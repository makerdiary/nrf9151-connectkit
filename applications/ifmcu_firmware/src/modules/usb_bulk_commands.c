/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/gpio.h>
#include <dk_buttons_and_leds.h>
#include <cmsis_dap.h>
#include <version.h>
#include <app_version.h>
#include <ncs_version.h>

#include "led_state.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bulk_commands, CONFIG_IFMCU_BULK_LOG_LEVEL);

#define IFMCU_FW_INFO_STRING	\
	"v" APP_VERSION_STRING "-g" STRINGIFY(APP_BUILD_VERSION)

#define FULL_FW_INFO_STRING	\
	"IFMCU Firmware v" APP_VERSION_STRING "-g" STRINGIFY(APP_BUILD_VERSION) "\r\n" \
	"Zephyr OS " STRINGIFY(BUILD_VERSION) "\r\n" \
	"nRF Connect SDK " STRINGIFY(NCS_BUILD_VERSION) "\r\n" \
	"Hardware: " CONFIG_BOARD_TARGET "\r\n" \
	"Build time: " __DATE__ " " __TIME__ "\r\n"

#define ID_DAP_VENDOR19					(0x80 + 19)
#define ID_DAP_VENDOR20					(0x80 + 20)
#define ID_DAP_VENDOR21					(0x80 + 21)
#define ID_DAP_VENDOR22					(0x80 + 22)

#define ID_DAP_VENDOR_NRF91_RESET	    ID_DAP_VENDOR19
#define ID_DAP_VENDOR_IFMCU_RESET	    ID_DAP_VENDOR20
#define ID_DAP_VENDOR_IFMCU_BOOTLOADER	ID_DAP_VENDOR21
#define ID_DAP_VENDOR_IFMCU_VERSION	    ID_DAP_VENDOR22

#define DAP_COMMAND_SUCCESS				0x00
#define DAP_COMMAND_FAILED				0xFF

#define NRF91_RESET_DURATION 			K_MSEC(100)

#define USB_BULK_PACKET_SIZE  			64

static const struct gpio_dt_spec reset_pin =
	GPIO_DT_SPEC_GET_OR(DT_INST(0, zephyr_swdp_gpio), reset_gpios, {});

static void nrf91_reset_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(nrf91_reset_work, nrf91_reset_work_handler);

static void ifmcu_reset_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(ifmcu_reset_work, ifmcu_reset_work_handler);

static void ifmcu_bootloader_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(ifmcu_bootloader_work, ifmcu_bootloader_work_handler);

/* Handler to reset the nRF91. */
static void nrf91_reset_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	if (!(reset_pin.port && device_is_ready(reset_pin.port))) {
		LOG_ERR("reset pin not available");
		return;
	}

	/* assert both reset and button signals */
	gpio_pin_configure_dt(&reset_pin, GPIO_OUTPUT_LOW);
	/* wait for reset to be registered */
	k_sleep(NRF91_RESET_DURATION);
	gpio_pin_configure_dt(&reset_pin, GPIO_DISCONNECTED);
}

/* Handler to enter the UF2 Bootloader of Interface MCU. */
static void ifmcu_bootloader_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	NRF_POWER->GPREGRET = 0x57;
	sys_reboot(SYS_REBOOT_COLD);
}

/* Handler to reset the Interface MCU. */
static void ifmcu_reset_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	sys_reboot(SYS_REBOOT_WARM);
}

/* This is a placeholder implementation until proper CMSIS-DAP support is available.
 * Only custom vendor commands are supported.
 */
size_t dap_execute_vendor_cmd(uint8_t *in, uint8_t *out)
{
	LOG_DBG("got command 0x%02X", in[0]);

	set_dap_led_state(LED_STATE_BLINK);

	if (in[0] < ID_DAP_VENDOR0) {
		return dap_execute_cmd(in, out);
	}
	if (in[0] == ID_DAP_VENDOR_IFMCU_BOOTLOADER) {
		if (!k_work_busy_get(&ifmcu_bootloader_work.work)) {
			k_work_reschedule(&ifmcu_bootloader_work, K_NO_WAIT);
		}
		goto success;
	}
	if (in[0] == ID_DAP_VENDOR_IFMCU_RESET) {
		if (!k_work_busy_get(&ifmcu_reset_work.work)) {
			k_work_reschedule(&ifmcu_reset_work, K_NO_WAIT);
		}
		goto success;
	}
	if (in[0] == ID_DAP_VENDOR_NRF91_RESET) {
		if (!k_work_busy_get(&nrf91_reset_work.work)) {
			k_work_reschedule(&nrf91_reset_work, K_NO_WAIT);
		}
		goto success;
	}
	if (in[0] == ID_DAP_VENDOR_IFMCU_VERSION) {
		const size_t VERSION_STRING_LEN = USB_BULK_PACKET_SIZE - 2;

		int len = snprintf(out + 2, VERSION_STRING_LEN, IFMCU_FW_INFO_STRING);
		if (len < 0 || len >= VERSION_STRING_LEN) {
			LOG_ERR("Failed to format version string");
			goto error;
		}
		out[0] = in[0];
		out[1] = len;
		return len + 2;
	}

error:
	/* default reply: command failed */
	out[0] = in[0];
	out[1] = DAP_COMMAND_FAILED;
	return 2;
success:
	out[0] = in[0];
	out[1] = DAP_COMMAND_SUCCESS;
	return 2;
}

#if defined(CONFIG_DK_LIBRARY)
static void button_handler(uint32_t button_states, uint32_t has_changed)
{
	if (has_changed & button_states & DK_BTN1_MSK) {
		if (!k_work_busy_get(&nrf91_reset_work.work)) {
			k_work_reschedule(&nrf91_reset_work, K_NO_WAIT);
		}
	}
}

static int button_handling_init(void)
{
	int err = dk_buttons_init(button_handler);

	if (err) {
		LOG_ERR("dk_buttons_init, error: %d", err);
	}
	return 0;
}

SYS_INIT(button_handling_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
#endif /* defined(CONFIG_DK_LIBRARY) */

static int cmd_version(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 1) {
		shell_print(sh, IFMCU_FW_INFO_STRING);
		return 0;
	}

	/* No need to enable the getopt and getopt_long for just one option. */
	if (strcmp("-a", argv[1]) && strcmp("--all", argv[1]) != 0) {
		shell_error(sh, "Unsupported option: %s", argv[1]);
		return -EIO;
	}

    shell_print(sh, FULL_FW_INFO_STRING);

	return 0;
}

static int cmd_bootloader(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

    shell_print(sh, "Entering IFMCU UF2 Bootloader mode...");

	if (!k_work_busy_get(&ifmcu_bootloader_work.work)) {
		k_work_reschedule(&ifmcu_bootloader_work, K_NO_WAIT);
	}

	return 0;
}

static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 1 || (strcmp("nrf9151", argv[1]) == 0)) {
		/* Reset nRF9151 default */
		if (!k_work_busy_get(&nrf91_reset_work.work)) {
			k_work_reschedule(&nrf91_reset_work, K_NO_WAIT);
		}
		return 0;
	}

	if (strcmp("nrf52820", argv[1]) == 0) {
		if (!k_work_busy_get(&ifmcu_reset_work.work)) {
			k_work_reschedule(&ifmcu_reset_work, K_NO_WAIT);
		}
	} else {
		shell_error(sh, "Unknown target: %s", argv[1]);
		return -EIO;
	}

	return 0;
}

SHELL_CMD_ARG_REGISTER(version, NULL,
		   "Show IFMCU Firmware version.\n"
		   "Use -a or --all option to show more details.",
		   cmd_version, 1, 1);
SHELL_CMD_ARG_REGISTER(bootloader, NULL,
		   "Enter IFMCU UF2 Bootloader mode.",
		   cmd_bootloader, 1, 0);
SHELL_CMD_ARG_REGISTER(reset, NULL,
		   "Reset a target device.\n"
		   "Usage: reset [target]\n"
		   "[target] - nrf9151 or nrf52820 (default: nrf9151)",
		   cmd_reset, 1, 1);
