/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BQ25180_H_
#define _BQ25180_H_

#ifdef __cplusplus
extern "C" {
#endif

int bq25180_init(void);
int bq25180_chgdis_set(uint8_t chgdis);
int bq25180_chgdis_get(uint8_t *chgdis);
int bq25180_sysreg_set(uint8_t mode);
int bq25180_sysreg_get(uint8_t *mode);
int bq25180_ichg_set(uint32_t current_ma);
int bq25180_ichg_get(uint32_t *current_ma);
int bq25180_vbatreg_set(uint32_t voltage_mv);
int bq25180_vbatreg_get(uint32_t *voltage_mv);
int bq25180_ilim_set(uint8_t level);
int bq25180_ilim_get(uint8_t *level);
int bq25180_status(uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif /* _BQ25180_H_ */
