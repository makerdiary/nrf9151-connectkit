/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(utils, CONFIG_IFMCU_UTILS_LOG_LEVEL);

static const struct device *temp_sensor = DEVICE_DT_GET(DT_ALIAS(die_temp));

static const struct gpio_dt_spec viosel_gpio =
	GPIO_DT_SPEC_GET_OR(DT_PATH(zephyr_user), viosel_gpios, {});

static const struct gpio_dt_spec simdet_gpio =
	GPIO_DT_SPEC_GET_OR(DT_PATH(zephyr_user), simdet_gpios, {});

static int viosel = -1;

static int cmd_temp(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	struct sensor_value val;

	if (!device_is_ready(temp_sensor)) {
		shell_error(sh, "sensor: device %s not ready.", temp_sensor->name);
		return -ENODEV;
	}

	/* fetch sensor samples */
	ret = sensor_sample_fetch(temp_sensor);
	if (ret) {
		shell_error(sh, "Failed to fetch sample (%d).", ret);
		return ret;
	}

	ret = sensor_channel_get(temp_sensor, SENSOR_CHAN_DIE_TEMP, &val);
	if (ret) {
		shell_error(sh, "Failed to get data (%d).", ret);
		return ret;
	}

    shell_print(sh, "%d.%d °C", val.val1, val.val2/100000);

	return 0;
}

static int cmd_simdet(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	if (!device_is_ready(simdet_gpio.port)) {
		shell_error(sh, "SIMDET GPIO controller not ready.");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&simdet_gpio, GPIO_INPUT);
	if (ret < 0) {
		shell_error(sh, "Could not configure SIMDET GPIO.");
		return ret;
	}

	k_sleep(K_MSEC(1)); /* Wait for the rail to come up and stabilize */

	ret = gpio_pin_get_dt(&simdet_gpio);
	if (ret < 0) {
		shell_error(sh, "Could not read SIMDET GPIO.");
		goto error;
	} else {
		shell_print(sh, "SIM Card: %s", ret? "Inserted" : "Not Inserted");
	}

error:
	ret = gpio_pin_configure_dt(&simdet_gpio, GPIO_DISCONNECTED);
	return ret;
}

int viosel_set(int val)
{
	int ret;

	if (!device_is_ready(viosel_gpio.port)) {
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&viosel_gpio, val? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}

	viosel = val;

	return 0;
}

int viosel_get(void)
{
	return viosel;
}

static int cmd_viosel(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	char *endptr;
	long val;

	if (argc == 1) {
		if (viosel < 0) {
			shell_print(sh, "VIO = 3.3V def.");
		} else {
			shell_print(sh, "VIO = %s", viosel?  "1.8V" : "3.3V");

		}
		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val != 0) && (val != 1))) {
		shell_error(sh, "<sel> must be 0 or 1");
		return -EINVAL;
	}

	ret = gpio_pin_configure_dt(&viosel_gpio, val? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		shell_error(sh, "Cannot configure VIOSEL (%d)", ret);
		return ret;
	}

	viosel = (int)val;

	return 0;
}

SHELL_CMD_ARG_REGISTER(temp, NULL,
		   "Get the die temperature", cmd_temp, 1, 0);
SHELL_CMD_ARG_REGISTER(simdet, NULL,
		   "Check if SIM Card inserted", cmd_simdet, 1, 0);
SHELL_CMD_ARG_REGISTER(viosel, NULL,
		   "Get or set VIO voltage\n"
		   "Usage: viosel [sel: 0|1]\n"
		   "[sel: 0|1] - 3.3V|1.8V", cmd_viosel, 1, 1);
