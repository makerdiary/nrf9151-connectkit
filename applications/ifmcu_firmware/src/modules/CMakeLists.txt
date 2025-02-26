#
# Copyright (c) 2020 Nordic Semiconductor
# Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

target_sources_ifdef(CONFIG_POWEROFF
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/power_handler.c)

target_sources_ifdef(CONFIG_SERIAL
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/uart_handler.c)

target_sources_ifdef(CONFIG_LED
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/led_state.c)

target_sources_ifdef(CONFIG_CHARGER
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/charger.c)

target_sources_ifdef(CONFIG_SETTINGS
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/settings.c)

target_sources_ifdef(CONFIG_IFMCU_CDC_ENABLE
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/usb_cdc_handler.c)

target_sources_ifdef(CONFIG_IFMCU_CMSIS_DAP_BULK_ENABLE
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/usb_bulk_interface.c)

target_sources_ifdef(CONFIG_IFMCU_CMSIS_DAP_VENDOR_COMMANDS
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/usb_bulk_commands.c)

target_sources_ifdef(CONFIG_IFMCU_UTILS_ENABLE
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils.c)
