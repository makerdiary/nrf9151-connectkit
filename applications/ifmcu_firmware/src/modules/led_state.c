/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/drivers/led.h>
#include "led_state.h"

#define MODULE led_state
#include "module_state_event.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MODULE, CONFIG_IFMCU_LEDS_LOG_LEVEL);

#define LED_GPIO_NODE_ID		DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)
#define LED_GPIO_NUM			DT_CHILD_NUM(LED_GPIO_NODE_ID)
#define LED_PWM_NODE_ID			DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)
#define LED_PWM_NUM				DT_CHILD_NUM(LED_PWM_NODE_ID)

#define LED_ID_PGOOD_STATE		0	/* Green LED */
#define LED_ID_CHARGER_STATE	1	/* RGB LED - Green */
#define LED_ID_UART_STATE		0	/* RGB LED - Red */
#define LED_ID_DAP_STATE	    2   /* RGB LED - Blue */

#define LED_STATE_TICK			K_MSEC(50)
#define LED_MAX_BRIGHTNESS		80
#define LED_BREATH_STEP         2

static const struct device *led_gpio = DEVICE_DT_GET(LED_GPIO_NODE_ID);
static const struct device *led_pwm = DEVICE_DT_GET(LED_PWM_NODE_ID);

static bool pgood_led_activity = false;
static bool charger_led_activity = false;
static bool uart_led_activity = false;
static bool dap_led_activity = false;
static led_state_t pgood_led_state = LED_STATE_OFF;
static led_state_t charger_led_state = LED_STATE_OFF;
static led_state_t uart_led_state = LED_STATE_OFF;
static led_state_t dap_led_state = LED_STATE_OFF;

static void led_state_timer_expiry_fn(struct k_timer *timer);
static K_TIMER_DEFINE(led_state_timer, led_state_timer_expiry_fn, NULL);

/* Set VIN Power Good LED state */
void set_pgood_led_state(led_state_t state)
{
    pgood_led_activity = true;
    pgood_led_state = state;
    return;
}

/* Set Charger LED state */
void set_charger_led_state(led_state_t state)
{
    charger_led_activity = true;
    charger_led_state = state;
    return;
}

/* Set DAP LED state */
void set_dap_led_state(led_state_t state)
{
    dap_led_activity = true;
    dap_led_state = state;
    return;
}

/* Set UART LED state */
void set_uart_led_state(led_state_t state)
{
    uart_led_activity = true;
    uart_led_state = state;
    return;
}

static void led_state_timer_expiry_fn(struct k_timer *timer)
{
	if (pgood_led_activity) {

		static uint8_t pgood_led_level = 0;

		if (pgood_led_state == LED_STATE_ON) {

			pgood_led_level = LED_MAX_BRIGHTNESS;

		} else if ((pgood_led_state == LED_STATE_BLINK) || (pgood_led_state == LED_STATE_BLINK_PERMANENT)) {
			// Toggle LED level
			pgood_led_level = pgood_led_level > 0 ? 0 : LED_MAX_BRIGHTNESS;

			// If in blink mode stop after one cycle
			if ((pgood_led_level == 0) && (pgood_led_state == LED_STATE_BLINK)) {
				pgood_led_activity = false;
				pgood_led_state = LED_STATE_OFF;
			}
		} else {
			// LED next state is LED_STATE_OFF
			pgood_led_level = 0;
			pgood_led_activity = false;
		}

		// update LED
		(void) led_set_brightness(led_gpio, LED_ID_PGOOD_STATE, pgood_led_level);
	}

	if (charger_led_activity) {

		static uint8_t charger_led_level = 0;

		if (charger_led_state == LED_STATE_ON) {

			charger_led_level = LED_MAX_BRIGHTNESS;

		}else if ((charger_led_state == LED_STATE_BLINK) || (charger_led_state == LED_STATE_BLINK_PERMANENT)) {
			// Toggle LED level
			charger_led_level = charger_led_level > 0 ? 0 : LED_MAX_BRIGHTNESS;

			// If in blink mode stop after one cycle
			if ((charger_led_level == 0) && (charger_led_state == LED_STATE_BLINK)) {
				charger_led_activity = false;
				charger_led_state = LED_STATE_OFF;
			}
		} else if (charger_led_state == LED_STATE_BREATH) {

			static uint8_t charger_led_dir = 1U;

			if (charger_led_dir) {
				charger_led_level += LED_BREATH_STEP;
				if (charger_led_level > LED_MAX_BRIGHTNESS) {
					charger_led_level -= LED_BREATH_STEP;
					charger_led_dir = 0U;
				}
			} else {
				if (charger_led_level > LED_BREATH_STEP) {
					charger_led_level -= LED_BREATH_STEP;
				} else {
					charger_led_level = 0;
					charger_led_dir = 1U;
				}
			}
		} else {
			// LED next state is LED_STATE_OFF
			charger_led_level = 0;
			charger_led_activity = false;
		}

		// update LED
		(void) led_set_brightness(led_pwm, LED_ID_CHARGER_STATE, charger_led_level);
	}

	if (uart_led_activity) {

		static uint8_t uart_led_level = 0;

		if (uart_led_state == LED_STATE_ON) {
			uart_led_level = LED_MAX_BRIGHTNESS;
		} else if ((uart_led_state == LED_STATE_BLINK) || (uart_led_state == LED_STATE_BLINK_PERMANENT)) {
			// Toggle LED level
			uart_led_level = uart_led_level > 0 ? 0 : LED_MAX_BRIGHTNESS;

			// If in blink mode stop after one cycle
			if ((uart_led_level == 0) && (uart_led_state == LED_STATE_BLINK)) {
				uart_led_activity = false;
				uart_led_state = LED_STATE_OFF;
			}
		} else {
			// LED next state is LED_STATE_OFF
			uart_led_level = 0;
			uart_led_activity = false;
		}

		// update LED
		(void) led_set_brightness(led_pwm, LED_ID_UART_STATE, uart_led_level);
	}

	if (dap_led_activity) {

		static uint8_t dap_led_level = 0;

		if ((dap_led_state == LED_STATE_BLINK) || (dap_led_state == LED_STATE_BLINK_PERMANENT)) {
			// Toggle LED level
			dap_led_level = dap_led_level > 0 ? 0 : LED_MAX_BRIGHTNESS;

			// If in blink mode stop after one cycle
			if ((dap_led_level == 0) && (dap_led_state == LED_STATE_BLINK)) {
				dap_led_activity = false;
				dap_led_state = LED_STATE_OFF;
			}
		} else {
			// LED next state is LED_STATE_OFF
			dap_led_level = 0;
			dap_led_activity = false;
		}

		// update LED
		(void) led_set_brightness(led_pwm, LED_ID_DAP_STATE, dap_led_level);
	}
}

static bool app_event_handler(const struct app_event_header *aeh)
{
	if (is_module_state_event(aeh)) {
		const struct module_state_event *event = cast_module_state_event(aeh);

		if (check_state(event, MODULE_ID(main), MODULE_STATE_READY)) {
			/* tell the rest of the system that we are busy. */
			module_set_state(MODULE_STATE_READY);

			k_timer_start(&led_state_timer, LED_STATE_TICK, LED_STATE_TICK);

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
