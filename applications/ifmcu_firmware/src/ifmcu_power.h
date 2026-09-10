/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _IFMCU_POWER_H_
#define _IFMCU_POWER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Turn every LED off and put the nRF52820 into SYSTEM OFF.
 *
 * VBUS detection or the DFU/RST button wakes the chip again, rebooting it
 * with full functionality.  Never returns.
 */
void ifmcu_system_off(void);

#ifdef __cplusplus
}
#endif

#endif /* _IFMCU_POWER_H_ */
