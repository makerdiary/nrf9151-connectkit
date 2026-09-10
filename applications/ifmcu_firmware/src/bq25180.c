/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/sys/util.h>

#include "ifmcu_power.h"
#include "led_state.h"
#include "bq25180.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(charger, CONFIG_IFMCU_CHARGER_LOG_LEVEL);

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

int bq25180_status(uint8_t *status)
{
	int ret;
	union charger_propval val;

	ret = charger_get_prop(chgdev, CHARGER_PROP_STATUS, &val);
	if (ret < 0) {
		return ret;
	}

	*status = val.status;

	return 0;

}

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
		/* Put the chip in SYSTEM OFF mode, to be rebooted when USB is connected. */
		ifmcu_system_off();
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


int bq25180_init(void)
{
	if (!device_is_ready(chgdev)) {
		LOG_ERR("Charger device is not ready");
		return -ENODEV;
	}

	k_work_init_delayable(&charging_dwork, charging_dwork_handler);
	charging_dwork_handler(&charging_dwork.work);

	return 0;
}
