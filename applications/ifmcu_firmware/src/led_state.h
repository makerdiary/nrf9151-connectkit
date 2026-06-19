/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LED_STATE_H_
#define _LED_STATE_H_

/**
 * @file
 * @defgroup led_state LED state
 * @{
 * @brief LED state.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief led states in the application. */
typedef enum led_state {
    LED_STATE_OFF = 0,
    LED_STATE_ON,
    LED_STATE_BLINK,
	LED_STATE_BLINK_PERMANENT,
    LED_STATE_BREATH,
    LED_STATE_COUNT
} led_state_t;

int led_state_init(void);
void set_pgood_led_state(led_state_t state);
void set_charger_led_state(led_state_t state);
void set_uart_led_state(led_state_t state);
void set_dap_led_state(led_state_t state);
void set_all_leds_off(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _LED_STATE_H_ */
