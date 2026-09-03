/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/settings/settings.h>
#include <dk_buttons_and_leds.h>
#include <version.h>
#include <app_version.h>
#include <ncs_version.h>
#include <ncs_commit.h>

#include "bq25180.h"
#include "ifmcu_power.h"
#include "led_state.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ifmcu_shell, CONFIG_IFMCU_SHELL_LOG_LEVEL);

#define SETTINGS_KEY "cfg"
#define VIOSEL_STORAGE_NAME				"viosel"
#define CHARGER_CHGDIS_STORAGE_NAME		"chgdis"
#define CHARGER_SYSREG_STORAGE_NAME		"sysreg"
#define CHARGER_ICHG_STORAGE_NAME		"ichg"
#define CHARGER_VBATREG_STORAGE_NAME	"vbatreg"
#define CHARGER_ILIM_STORAGE_NAME		"ilim"

#define NRF91_RESET_DURATION 			K_MSEC(100)

#define IFMCU_FW_INFO_STRING	\
	"v" APP_VERSION_STRING "-g" STRINGIFY(APP_BUILD_VERSION)

#define FULL_FW_INFO_STRING	\
	"IFMCU Firmware v" APP_VERSION_STRING "-g" STRINGIFY(APP_BUILD_VERSION) "\r\n" \
	"Zephyr OS " STRINGIFY(BUILD_VERSION) "\r\n" \
	"nRF Connect SDK v" NCS_VERSION_STRING "-" NCS_COMMIT_STRING "\r\n" \
	"Hardware: " CONFIG_BOARD_TARGET "\r\n" \
	"Build time: " __DATE__ " " __TIME__ "\r\n"

static const struct device *temp_sensor = DEVICE_DT_GET(DT_ALIAS(die_temp));

static const struct gpio_dt_spec viosel_gpio =
	GPIO_DT_SPEC_GET_OR(DT_PATH(zephyr_user), viosel_gpios, {});

static const struct gpio_dt_spec simdet_gpio =
	GPIO_DT_SPEC_GET_OR(DT_PATH(zephyr_user), simdet_gpios, {});

static const struct gpio_dt_spec reset_pin =
	GPIO_DT_SPEC_GET_OR(DT_INST(0, zephyr_swdp_gpio), reset_gpios, {});

static void nrf91_reset_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(nrf91_reset_work, nrf91_reset_work_handler);

static void ifmcu_reset_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(ifmcu_reset_work, ifmcu_reset_work_handler);

static void ifmcu_bootloader_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(ifmcu_bootloader_work, ifmcu_bootloader_work_handler);

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

const static char * const status_descr[] = {
	[0] = "Unknown",
	[1] = "Charging",
	[2] = "Discharging",
	[3] = "Not Charging",
	[4] = "Charging Full",
};

static int viosel = -1;

/* Config GPIO voltage: 1=1.8V, 0=3.3V */
static int viosel_set(int val)
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

/* Get GPIO voltage setting */
static int viosel_get(void)
{
	return viosel;
}

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

static int settings_set(const char *key, size_t len_rd,
		   settings_read_cb read_cb, void *cb_arg)
{
	int ret;
	ssize_t rc;

	if (!strcmp(key, VIOSEL_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(int)) {
			int viosel;
			rc = read_cb(cb_arg, &viosel, sizeof(viosel));
			if (rc == sizeof(viosel)) {
				if ((viosel == -1 || viosel == 0 || viosel == 1) && viosel != viosel_get()) {
					ret = viosel_set(viosel);
					if (ret < 0) {
						LOG_ERR("Fail to set VIOSEL: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load VIOSEL = %d", viosel);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	} else if (!strcmp(key, CHARGER_CHGDIS_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(uint8_t)) {
			uint8_t chgdis_stored;
			rc = read_cb(cb_arg, &chgdis_stored, sizeof(chgdis_stored));
			if (rc == sizeof(chgdis_stored)) {
				uint8_t chgdis;
				ret = bq25180_chgdis_get(&chgdis);
				if (ret < 0) {
					LOG_ERR("Fail to get CHGDIS: (err = %d)", ret);
					return 0;
				}
				if ((chgdis_stored == 0 || chgdis_stored == 1) && chgdis_stored != chgdis) {
					ret = bq25180_chgdis_set(chgdis_stored);
					if (ret < 0) {
						LOG_ERR("Fail to set CHGDIS: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load CHGDIS = %d", chgdis_stored);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	} else if (!strcmp(key, CHARGER_SYSREG_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(uint8_t)) {
			uint8_t sysreg_stored;
			rc = read_cb(cb_arg, &sysreg_stored, sizeof(sysreg_stored));
			if (rc == sizeof(sysreg_stored)) {
				uint8_t sysreg;
				ret = bq25180_sysreg_get(&sysreg);
				if (ret < 0) {
					LOG_ERR("Fail to get SYSREG: (err = %d)", ret);
					return 0;
				}
				if (sysreg_stored < 8 && sysreg_stored != sysreg) {
					ret = bq25180_sysreg_set(sysreg_stored);
					if (ret < 0) {
						LOG_ERR("Fail to set SYSREG: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load SYSREG = %d", sysreg_stored);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	} else if (!strcmp(key, CHARGER_ICHG_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(uint32_t)) {
			uint32_t ichg_ma_stored;
			rc = read_cb(cb_arg, &ichg_ma_stored, sizeof(ichg_ma_stored));
			if (rc == sizeof(ichg_ma_stored)) {
				uint32_t ichg_ma;
				ret = bq25180_ichg_get(&ichg_ma);
				if (ret < 0) {
					LOG_ERR("Fail to get ICHG: (err = %d)", ret);
					return 0;
				}
				if (ichg_ma_stored <= 1000 && ichg_ma_stored != ichg_ma) {
					ret = bq25180_ichg_set(ichg_ma_stored);
					if (ret < 0) {
						LOG_ERR("Fail to set ICHG: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load ICHG = %d", ichg_ma_stored);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	} else if (!strcmp(key, CHARGER_VBATREG_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(uint32_t)) {
			uint32_t vbatreg_mv_stored;
			rc = read_cb(cb_arg, &vbatreg_mv_stored, sizeof(vbatreg_mv_stored));
			if (rc == sizeof(vbatreg_mv_stored)) {
				uint32_t vbatreg_mv;
				ret = bq25180_vbatreg_get(&vbatreg_mv);
				if (ret < 0) {
					LOG_ERR("Fail to get VBATREG: (err = %d)", ret);
					return 0;
				}
				if ((vbatreg_mv_stored >= 3500 && vbatreg_mv_stored <= 4650) && vbatreg_mv_stored != vbatreg_mv) {
					ret = bq25180_vbatreg_set(vbatreg_mv_stored);
					if (ret < 0) {
						LOG_ERR("Fail to set VBATREG: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load VBATREG = %d", vbatreg_mv_stored);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	} else if (!strcmp(key, CHARGER_ILIM_STORAGE_NAME)) {
		/* Ignore record when size is improper. */
		if (len_rd == sizeof(uint8_t)) {
			uint8_t ilim_stored;
			rc = read_cb(cb_arg, &ilim_stored, sizeof(ilim_stored));
			if (rc == sizeof(ilim_stored)) {
				uint8_t ilim;
				ret = bq25180_ilim_get(&ilim);
				if (ret < 0) {
					LOG_ERR("Fail to get ILIM: (err = %d)", ret);
					return 0;
				}
				if (ilim_stored < 8 && ilim_stored != ilim) {
					ret = bq25180_ilim_set(ilim_stored);
					if (ret < 0) {
						LOG_ERR("Fail to set ILIM: (err = %d)", ret);
						return 0;
					}
					LOG_DBG("Load ILIM = %d", ilim_stored);
				}
			} else {
				if (rc < 0) {
					LOG_ERR("Settings read-out error");
					return rc;
				}
			}
		}
	}

	return 0;
}

static void settings_init(void)
{
	int err;

	err = settings_subsys_init();
	if (err) {
		return;
	}

	static struct settings_handler sh = {
		.name = SETTINGS_KEY,
		.h_set = settings_set,
	};

	err = settings_register(&sh);
	if (err) {
		return;
	}

	err = settings_load();
	if (err) {
		return;
	}
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

static int cmd_cfgsync(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	/* Store VIOSEL */
	char viosel_key[] = SETTINGS_KEY "/" VIOSEL_STORAGE_NAME;
	int viosel = viosel_get();
	ret = settings_save_one(viosel_key, &viosel, sizeof(viosel));
	if (ret) {
		shell_error(sh, "Problem storing %s: (err = %d)", viosel_key, ret);
	}

	/* Store CHGDIS */
	char chgdis_key[] =  SETTINGS_KEY "/" CHARGER_CHGDIS_STORAGE_NAME;
	uint8_t chgdis;
	ret = bq25180_chgdis_get(&chgdis);
	if (ret < 0) {
		shell_error(sh, "Fail to get CHGDIS: (err = %d)", ret);
	} else {
		ret = settings_save_one(chgdis_key, &chgdis, sizeof(chgdis));
		if (ret) {
			shell_error(sh, "Problem storing %s: (err = %d)", chgdis_key, ret);
		}
	}

	/* Store SYSREG */
	char sysreg_key[] =  SETTINGS_KEY "/" CHARGER_SYSREG_STORAGE_NAME;
	uint8_t sysreg;
	ret = bq25180_sysreg_get(&sysreg);
	if (ret < 0) {
		shell_error(sh, "Fail to get SYSREG: (err = %d)", ret);
	} else {
		ret = settings_save_one(sysreg_key, &sysreg, sizeof(sysreg));
		if (ret) {
			shell_error(sh, "Problem storing %s: (err = %d)", sysreg_key, ret);
		}
	}

	/* Store ICHG */
	char ichg_key[] =  SETTINGS_KEY "/" CHARGER_ICHG_STORAGE_NAME;
	uint32_t ichg_ma;
	ret = bq25180_ichg_get(&ichg_ma);
	if (ret < 0) {
		shell_error(sh, "Fail to get ICHG: (err = %d)", ret);
	} else {
		ret = settings_save_one(ichg_key, &ichg_ma, sizeof(ichg_ma));
		if (ret) {
			shell_error(sh, "Problem storing %s: (err = %d)", ichg_key, ret);
		}
	}

	/* Store VBATREG */
	char vbatreg_key[] =  SETTINGS_KEY "/" CHARGER_VBATREG_STORAGE_NAME;
	uint32_t vbatreg_mv;
	ret = bq25180_vbatreg_get(&vbatreg_mv);
	if (ret < 0) {
		shell_error(sh, "Fail to get VBATREG: (err = %d)", ret);
	} else {
		ret = settings_save_one(vbatreg_key, &vbatreg_mv, sizeof(vbatreg_mv));
		if (ret) {
			shell_error(sh, "Problem storing %s: (err = %d)", vbatreg_key, ret);
		}
	}

	/* Store ILIM */
	char ilim_key[] =  SETTINGS_KEY "/" CHARGER_ILIM_STORAGE_NAME;
	uint8_t ilim;
	ret = bq25180_ilim_get(&ilim);
	if (ret < 0) {
		shell_error(sh, "Fail to get ILIM: (err = %d)", ret);
	} else {
		ret = settings_save_one(ilim_key, &ilim, sizeof(ilim));
		if (ret) {
			shell_error(sh, "Problem storing %s: (err = %d)", ilim_key, ret);
		}
	}

	return 0;
}

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

static int cmd_charger_status(const struct shell *sh, size_t argc, char **argv)
{
	int ret;
	uint8_t status;

	ret = bq25180_status(&status);
	if (ret < 0) {
		shell_error(sh, "Cannot get Charger Status: (err = %d)", ret);
		return ret;
	}

	shell_print(sh, "[%d]: %s", status, status_descr[status]);
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

static int cmd_shutdown(const struct shell *sh, size_t argc, char **argv)
{
	/* Put the chip in SYSTEM OFF mode, to be rebooted when USB is connected. */
	ifmcu_system_off();

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
		shell_print(sh, "%s: %s", argv[0], ret? "Inserted" : "Uninserted");
	}

error:
	ret = gpio_pin_configure_dt(&simdet_gpio, GPIO_DISCONNECTED);
	return ret;
}

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

SHELL_CMD_ARG_REGISTER(bootloader, NULL,
		   "Enter IFMCU UF2 Bootloader mode.",
		   cmd_bootloader, 1, 0);

SHELL_CMD_ARG_REGISTER(cfgsync, NULL,
		   "Store settings to the non-volatile memory", cmd_cfgsync, 1, 0);
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
	SHELL_CMD_ARG(status, NULL,
		"Get Charging Status", cmd_charger_status, 1, 0),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(charger, &sub_charger,
	"Charger commands\n"
	"Please refer to TI BQ25180 Datasheet for more details.", NULL);

SHELL_CMD_ARG_REGISTER(reset, NULL,
		   "Reset a target device.\n"
		   "Usage: reset [target]\n"
		   "[target] - nrf9151 or nrf52820 (default: nrf9151)",
		   cmd_reset, 1, 1);
SHELL_CMD_ARG_REGISTER(shutdown, NULL,
		   "Put the IFMCU (nRF52820) in SYSTEM OFF mode", cmd_shutdown, 1, 0);
SHELL_CMD_ARG_REGISTER(simdet, NULL,
		   "Display nano-SIM Card status", cmd_simdet, 1, 0);
SHELL_CMD_ARG_REGISTER(temp, NULL,
		   "Get the die temperature", cmd_temp, 1, 0);
SHELL_CMD_ARG_REGISTER(version, NULL,
		   "Show IFMCU Firmware version.\n"
		   "Use -a or --all option to show more details.",
		   cmd_version, 1, 1);
SHELL_CMD_ARG_REGISTER(viosel, NULL,
		   "Get or set VIO voltage\n"
		   "Usage: viosel [sel: 0|1]\n"
		   "[sel: 0|1] - 3.3V|1.8V", cmd_viosel, 1, 1);

int ifmcu_shell_init(void)
{
	led_state_init();

	settings_init();

	bq25180_init();

	return 0;
}

SYS_INIT(ifmcu_shell_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
