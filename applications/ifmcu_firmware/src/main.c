/*
 * Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sample_usbd.h>

#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart/uart_bridge.h>
#include <zephyr/kernel.h>

#include <zephyr/sys/byteorder.h>
#include <zephyr/usb/bos.h>
#include <zephyr/usb/msos_desc.h>
#include <zephyr/dap/dap_link.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include "msosv2.h"

DAP_LINK_CONTEXT_DEFINE(ifmcu_dap_ctx, DEVICE_DT_GET_ONE(zephyr_swdp_gpio));

const struct device *const uart_bridge = DEVICE_DT_GET_ONE(zephyr_uart_bridge);

static struct usbd_context *ifmcu_usbd;

static void ifmcu_usbd_msg_cb(struct usbd_context *const ctx, const struct usbd_msg *msg)
{
	LOG_INF("USBD message: %s", usbd_msg_type_string(msg->type));

	switch (msg->type) {
	case USBD_MSG_VBUS_READY:
		if (usbd_enable(ctx)) {
			LOG_ERR("Failed to enable device support");
		}
		break;

	case USBD_MSG_VBUS_REMOVED:
		if (usbd_disable(ctx)) {
			LOG_ERR("Failed to disable device support");
		}
		break;

	case USBD_MSG_RESUME:
		break;

	case USBD_MSG_SUSPEND:
		break;

	case USBD_MSG_RESET:
		break;

	case USBD_MSG_UDC_ERROR:
		break;

	case USBD_MSG_STACK_ERROR:
		break;

	case USBD_MSG_CONFIGURATION:
		break;
	case USBD_MSG_CDC_ACM_LINE_CODING:
	case USBD_MSG_CDC_ACM_CONTROL_LINE_STATE:
		uart_bridge_settings_update(msg->dev, uart_bridge);
		break;

	default:
		LOG_ERR("Unexpected USB device message type: %d", msg->type);
		break;
	}
}

int main(void)
{
	int ret;

	ret = dap_link_init(&ifmcu_dap_ctx);
	if (ret) {
		LOG_ERR("Failed to initialize DAP controller, %d", ret);
		return ret;
	}

	ret = dap_link_backend_usb_init(&ifmcu_dap_ctx);
	if (ret) {
		LOG_ERR("Failed to initialize DAP Link USB backend, %d", ret);
		return ret;
	}

	ifmcu_usbd = sample_usbd_setup_device(ifmcu_usbd_msg_cb);
	if (ifmcu_usbd == NULL) {
		LOG_ERR("Failed to setup USB device");
		return -ENODEV;
	}

	ret = usbd_add_descriptor(ifmcu_usbd, &bos_vreq_msosv2);
	if (ret) {
		LOG_ERR("Failed to add MSOSv2 capability descriptor");
		return ret;
	}

	ret = usbd_init(ifmcu_usbd);
	if (ret) {
		LOG_ERR("Failed to initialize device support");
		return ret;
	}

	ret = usbd_enable(ifmcu_usbd);
	if (ret) {
		LOG_ERR("Failed to enable device support");
		return ret;
	}

	return 0;
}
