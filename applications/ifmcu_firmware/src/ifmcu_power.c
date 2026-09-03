/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/poweroff.h>
#include <cmsis_core.h>

#include "led_state.h"
#include "ifmcu_power.h"

void ifmcu_system_off(void)
{
	set_all_leds_off();

	/* Zephyr sets SEVONPEND at boot.  With it set, any pending interrupt
	 * makes the WFE inside nrf_power_system_off() return immediately, so
	 * the CPU never sleeps and the SYSTEMOFF request never takes effect:
	 * the chip spins at ~2 mA instead of powering down.  Clear it first. */
	SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;
	__DSB();
	sys_poweroff();
}
