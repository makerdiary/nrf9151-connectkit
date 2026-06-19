/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <modem/lte_lc.h>
#include <modem/nrf_modem_lib.h>
#include <zephyr/sys/poweroff.h>

int main(void)
{
	nrf_modem_lib_init();

	lte_lc_power_off();

	k_sleep(K_MSEC(100));

	nrf_modem_lib_shutdown();

	sys_poweroff();

	while (1) {
	}

	return 0;
}
