/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BQ2518X_H_
#define _BQ2518X_H_

#ifdef __cplusplus
extern "C" {
#endif

enum bq2518x_device_id {
	BQ25180_DEVICE_ID = 0x00,
	BQ25186_DEVICE_ID = 0x01,
	BQ25188_DEVICE_ID = 0x04,
};

int bq2518x_init(void);
int bq2518x_chgdis_set(uint8_t chgdis);
int bq2518x_chgdis_get(uint8_t *chgdis);
int bq2518x_sysreg_set(uint8_t mode);
int bq2518x_sysreg_get(uint8_t *mode);
int bq2518x_ichg_set(uint32_t current_ma);
int bq2518x_ichg_get(uint32_t *current_ma);
int bq2518x_vbatreg_set(uint32_t voltage_mv);
int bq2518x_vbatreg_get(uint32_t *voltage_mv);
int bq2518x_ilim_set(uint8_t level);
int bq2518x_ilim_get(uint8_t *level);
int bq2518x_status(uint8_t *status);
int bq2518x_devid(uint8_t *devid);

#ifdef __cplusplus
}
#endif

#endif /* _BQ2518X_H_ */
