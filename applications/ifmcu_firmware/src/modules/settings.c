/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 * Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/settings/settings.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/util.h>

#define MODULE settings
#include "module_state_event.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MODULE, CONFIG_IFMCU_SETTINGS_LOG_LEVEL);

#define SETTINGS_KEY "cfg"
#define VIOSEL_STORAGE_NAME				"viosel"
#define CHARGER_CHGDIS_STORAGE_NAME		"chgdis"
#define CHARGER_SYSREG_STORAGE_NAME		"sysreg"
#define CHARGER_ICHG_STORAGE_NAME		"ichg"
#define CHARGER_VBATREG_STORAGE_NAME	"vbatreg"
#define CHARGER_ILIM_STORAGE_NAME		"ilim"

extern int viosel_set(int val);
extern int viosel_get(void);
extern int bq25180_chgdis_set(uint8_t chgdis);
extern int bq25180_chgdis_get(uint8_t *chgdis);
extern int bq25180_sysreg_set(uint8_t mode);
extern int bq25180_sysreg_get(uint8_t *mode);
extern int bq25180_ichg_set(uint32_t current_ma);
extern int bq25180_ichg_get(uint32_t *current_ma);
extern int bq25180_vbatreg_set(uint32_t voltage_mv);
extern int bq25180_vbatreg_get(uint32_t *voltage_mv);
extern int bq25180_ilim_set(uint8_t level);
extern int bq25180_ilim_get(uint8_t *level);

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
		if (len_rd == sizeof(uint8_t)) {
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
		if (len_rd == sizeof(uint8_t)) {
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

SHELL_CMD_ARG_REGISTER(cfgsync, NULL,
		   "Store settings to the non-volatile memory", cmd_cfgsync, 1, 0);

static bool app_event_handler(const struct app_event_header *aeh)
{
	if (is_module_state_event(aeh)) {
		const struct module_state_event *event =
			cast_module_state_event(aeh);

		if (check_state(event, MODULE_ID(main), MODULE_STATE_READY)) {
			settings_init();
		}

		return false;
	}

	/* If event is unhandled, unsubscribe. */
	__ASSERT_NO_MSG(false);

	return false;
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, module_state_event);
