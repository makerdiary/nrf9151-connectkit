/*
 * Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/util.h>

#include "led_state.h"

#define MODULE charger
#include "module_state_event.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MODULE, CONFIG_IFMCU_CHARGER_LOG_LEVEL);

#define BQ25180_ICHG_CTRL 0x04
#define BQ25180_TMR_ILIM  0x08
#define BQ25180_SYS_REG   0x0A

#define BQ25180_ICHG_CHG_DIS 		BIT(7)
#define BQ25180_SYS_REG_CTRL_MSK    GENMASK(7, 5)
#define BQ25180_ILIM_MSK			GENMASK(2, 0)

#define CHARGING_CHECK_INTERVAL	K_MSEC(1000)

struct charger_config {
	struct i2c_dt_spec i2c;
	uint32_t initial_current_microamp;
	uint32_t max_voltage_microvolt;
	uint32_t recharge_voltage_microvolt;
	uint32_t precharge_threshold_voltage_microvolt;
};

const static char * const sysreg_descr[] = {
	[0] = "VSYS = VBAT + 225 mV (3.8 V minimum)",
	[1] = "VSYS = 4.4V",
	[2] = "VSYS = 4.5V",
	[3] = "VSYS = 4.6V",
	[4] = "VSYS = 4.7V",
	[5] = "VSYS = 4.8V",
	[6] = "VSYS = 4.9V",
	[7] = "VSYS = VBUS",
};

const static char * const ilim_descr[] = {
	[0] = "ILIM = 50mA",
	[1] = "ILIM = 100mA(max.)",
	[2] = "ILIM = 200mA",
	[3] = "ILIM = 300mA",
	[4] = "ILIM = 400mA",
	[5] = "ILIM = 500mA(max.)",
	[6] = "ILIM = 700mA",
	[7] = "ILIM = 1100mA",
};

static const struct device *chgdev = DEVICE_DT_GET(DT_NODELABEL(charger));

static struct k_work_delayable charging_dwork;

int bq25180_chgdis_set(uint8_t chgdis)
{
	int ret;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = charger_charge_enable(chgdev, !chgdis);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int bq25180_chgdis_get(uint8_t *chgdis)
{
	const struct charger_config *config = chgdev->config;
	const struct i2c_dt_spec *i2c = &config->i2c;
	int ret;
	uint8_t ichg_ctrl;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = i2c_reg_read_byte_dt(i2c, BQ25180_ICHG_CTRL, &ichg_ctrl);
	if (ret < 0) {
		return ret;
	}

	*chgdis = (ichg_ctrl & BQ25180_ICHG_CHG_DIS)? 1 : 0;

	return 0;
}

int bq25180_sysreg_set(uint8_t mode)
{
	const struct charger_config *config = chgdev->config;
	const struct i2c_dt_spec *i2c = &config->i2c;
	int ret;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	mode = FIELD_PREP(BQ25180_SYS_REG_CTRL_MSK, mode);

	ret = i2c_reg_update_byte_dt(i2c, BQ25180_SYS_REG, BQ25180_SYS_REG_CTRL_MSK, mode);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int bq25180_sysreg_get(uint8_t *mode)
{
	const struct charger_config *config = chgdev->config;
	const struct i2c_dt_spec *i2c = &config->i2c;
	int ret;
	uint8_t sysreg_ctrl;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = i2c_reg_read_byte_dt(i2c, BQ25180_SYS_REG, &sysreg_ctrl);
	if (ret < 0) {
		return ret;
	}

	*mode = FIELD_GET(BQ25180_SYS_REG_CTRL_MSK, sysreg_ctrl);

	return 0;
}

int bq25180_ichg_set(uint32_t current_ma)
{
	int ret;
	union charger_propval val;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	val.const_charge_current_ua = current_ma * 1000;

	ret = charger_set_prop(chgdev, CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA, &val);

	return ret;
}

int bq25180_ichg_get(uint32_t *current_ma)
{
	int ret;
	union charger_propval val;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = charger_get_prop(chgdev, CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA, &val);
	if (ret < 0) {
		return ret;
	}

	*current_ma = val.const_charge_current_ua / 1000;

	return 0;
}

int bq25180_vbatreg_set(uint32_t voltage_mv)
{
	int ret;
	union charger_propval val;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	val.const_charge_voltage_uv = voltage_mv * 1000;

	ret = charger_set_prop(chgdev, CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV, &val);

	return ret;
}

int bq25180_vbatreg_get(uint32_t *voltage_mv)
{
	int ret;
	union charger_propval val;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = charger_get_prop(chgdev, CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV, &val);
	if (ret < 0) {
		return ret;
	}

	*voltage_mv = val.const_charge_voltage_uv / 1000;

	return 0;
}

int bq25180_ilim_set(uint8_t level)
{
	const struct charger_config *config = chgdev->config;
	const struct i2c_dt_spec *i2c = &config->i2c;
	int ret;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	level = FIELD_PREP(BQ25180_ILIM_MSK, level);

	ret = i2c_reg_update_byte_dt(i2c, BQ25180_TMR_ILIM, BQ25180_ILIM_MSK, level);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int bq25180_ilim_get(uint8_t *level)
{
	const struct charger_config *config = chgdev->config;
	const struct i2c_dt_spec *i2c = &config->i2c;
	int ret;
	uint8_t ilim;

	if (!device_is_ready(chgdev)) {
		return -ENODEV;
	}

	ret = i2c_reg_read_byte_dt(i2c, BQ25180_TMR_ILIM, &ilim);
	if (ret < 0) {
		return ret;
	}

	*level = FIELD_GET(BQ25180_ILIM_MSK, ilim);

	return 0;
}

#ifdef CONFIG_SHELL

static int cmd_charger_chgdis(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint8_t chgdis;
	char *endptr;
	long val;

	if (argc == 1) {
		ret = bq25180_chgdis_get(&chgdis);
		if (ret < 0) {
			shell_error(sh, "Cannot get CHG_DIS: (err = %d)", ret);
			return ret;
		}

		shell_print(sh, "Battery Charging %s", chgdis? "Disabled" : "Enabled");

		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val != 0) && (val != 1))) {
		shell_error(sh, "<value> must be 0 or 1");
		return -EINVAL;
	}

	ret = bq25180_chgdis_set((uint8_t)val);
	if (ret < 0) {
		shell_error(sh, "Cannot enable or disable charging: (err = %d)", ret);
		return ret;
	}

	return 0;
}

static int cmd_charger_sysreg(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint8_t mode;
	char *endptr;
	long val;

	if (argc == 1) {
		ret = bq25180_sysreg_get(&mode);
		if (ret < 0) {
			shell_error(sh, "Cannot get SYS Voltage: (err = %d)", ret);
			return ret;
		}
		shell_print(sh, "[%d]: %s", mode, sysreg_descr[mode]);
		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val < 0) && (val > 7))) {
		shell_error(sh, "<mode> must be in the range 0 to 7");
		return -EINVAL;
	}

	ret = bq25180_sysreg_set((uint8_t)val);
	if (ret < 0) {
		shell_error(sh, "Cannot set SYS Voltage: (err = %d)", ret);
		return ret;
	}

	return 0;
}

static int cmd_charger_ichg(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint32_t current_ma;
	char *endptr;
	long val;

	if (argc == 1) {
		ret = bq25180_ichg_get(&current_ma);
		if (ret < 0) {
			shell_error(sh, "Cannot get Fast Charge Current: (err = %d)", ret);
			return ret;
		}
		shell_print(sh, "%d mA", current_ma);
		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val < 0) && (val > 1000))) {
		shell_error(sh, "<current_ma> must be in the range 0 to 1000");
		return -EINVAL;
	}

	ret = bq25180_ichg_set((uint32_t)val);
	if (ret < 0) {
		shell_error(sh, "Cannot set Fast Charge Current: (err = %d)", ret);
		return ret;
	}

	return 0;
}

static int cmd_charger_vbatreg(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint32_t voltage_mv;
	char *endptr;
	long val;

	if (argc == 1) {
		ret = bq25180_vbatreg_get(&voltage_mv);
		if (ret < 0) {
			shell_error(sh, "Cannot get Battery Regulation Voltage (%d)", ret);
			return ret;
		}
		shell_print(sh, "%d mV", voltage_mv);
		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val < 3500) && (val > 4650))) {
		shell_error(sh, "<voltage_mv> must be in the range 3500 to 4650");
		return -EINVAL;
	}

	ret = bq25180_vbatreg_set((uint32_t)val);
	if (ret < 0) {
		shell_error(sh, "Cannot set Battery Regulation Voltage: (err = %d)", ret);
		return ret;
	}

	return 0;
}

static int cmd_charger_ilim(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint8_t level;
	char *endptr;
	long val;

	if (argc == 1) {
		ret = bq25180_ilim_get(&level);
		if (ret < 0) {
			shell_error(sh, "Cannot get Input Current Limit: (err = %d)", ret);
			return ret;
		}
		shell_print(sh, "[%d]: %s", level, ilim_descr[level]);
		return 0;
	}

	endptr = argv[1];
	val = strtol(argv[1], &endptr, 10);
	if ((endptr == argv[1]) || ((val < 0) && (val > 7))) {
		shell_error(sh, "<level> must be in the range 0 to 7");
		return -EINVAL;
	}

	ret = bq25180_ilim_set((uint8_t)val);
	if (ret < 0) {
		shell_error(sh, "Cannot set Input Current Limit: (err = %d)", ret);
		return ret;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_charger,
	SHELL_CMD_ARG(chgdis, NULL,
		"Charge Disable\n"
		"Usage: charger chgdis [value: 0|1]", cmd_charger_chgdis, 1, 1),
	SHELL_CMD_ARG(sysreg, NULL,
		"Get or Set SYS Voltage\n"
		"Usage: charger sysreg [mode: 0-7]\n"
		"[mode: 0-7] - VBAT+225mV(>=3.8V)|4.4V|4.5V|4.6V|4.7V|4.8V|4.9V|VBUS",
		cmd_charger_sysreg, 1, 1),
	SHELL_CMD_ARG(ichg, NULL,
		"Get or Set Fast Charge Current (0-1000mA)\n"
		"Usage: charger ichg [current_ma: 0-1000]", cmd_charger_ichg, 1, 1),
	SHELL_CMD_ARG(vbatreg, NULL,
		"Get or Set Battery Regulation Voltage (3500-4650mV)\n"
		"Usage: charger vbatreg [voltage_mv: 3500-4650]", cmd_charger_vbatreg, 1, 1),
	SHELL_CMD_ARG(ilim, NULL,
		"Get or Set Input Current Limit\n"
		"Usage: charger ilim [level: 0-7]\n"
		"[level: 0-7] - 50mA|100mA|200mA|300mA|400mA|500mA|700mA|1100mA",
		cmd_charger_ilim, 1, 1),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER(charger, &sub_charger,
	"Charger commands\n"
	"Please refer to TI BQ25180 Datasheet for more details.", NULL);

#endif /* CONFIG_SHELL */

static void charging_dwork_handler(struct k_work *work)
{
	int ret;
	union charger_propval val0, val1;

	ret = charger_get_prop(chgdev, CHARGER_PROP_ONLINE, &val0);
	if (ret < 0) {
		LOG_ERR("Could not get the charger online property. (rc: %d)", ret);
		goto out;
	}

	ret = charger_get_prop(chgdev, CHARGER_PROP_STATUS, &val1);
	if (ret < 0) {
		LOG_ERR("Could not get the charger status property. (rc: %d)", ret);
		goto out;
	}

	if (val0.online == CHARGER_ONLINE_OFFLINE) {
		set_pgood_led_state(LED_STATE_OFF);
		set_charger_led_state(LED_STATE_OFF);
	} else {
		set_pgood_led_state(LED_STATE_ON);
		if (val1.status == CHARGER_STATUS_CHARGING) {
			set_charger_led_state(LED_STATE_BREATH);
		} else if (val1.status == CHARGER_STATUS_FULL) {
			set_charger_led_state(LED_STATE_OFF);
		} else {
			LOG_WRN("unknown charger state: oneline=%d status=%d",
				val0.online, val1.status);
			goto out;
		}
	}

out:
    k_work_schedule(&charging_dwork, CHARGING_CHECK_INTERVAL);
}

static int init_fn(void)
{
	if (!device_is_ready(chgdev)) {
		LOG_ERR("Charger device is not ready");
		return -ENODEV;
	}

	k_work_init_delayable(&charging_dwork, charging_dwork_handler);
	charging_dwork_handler(&charging_dwork.work);

	return 0;
}

static bool app_event_handler(const struct app_event_header *aeh)
{
	if (is_module_state_event(aeh)) {
		const struct module_state_event *event = cast_module_state_event(aeh);

		if (check_state(event, MODULE_ID(main), MODULE_STATE_READY)) {
			/* tell the rest of the system that we are busy. */
			module_set_state(MODULE_STATE_READY);

			int ret = init_fn();
			if (ret) {
				LOG_ERR("Failed to initialize the charger, %d", ret);
			}

			/* tell the rest of the system that we are done. */
			module_set_state(MODULE_STATE_STANDBY);
		}
		return false;
	}
	/* we should not reach this point */
	__ASSERT_NO_MSG(false);

	return false;
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, module_state_event);
