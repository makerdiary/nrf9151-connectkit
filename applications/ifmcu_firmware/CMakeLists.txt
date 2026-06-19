#
# Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(IFMCU)

include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

# NORDIC SDK APP START
target_sources(app PRIVATE 
  src/main.c
  src/bq25180.c
  src/ifmcu_dap.c
  src/ifmcu_shell.c
  src/ifmcu_uart_bridge.c
  src/led_state.c
  )
# NORDIC SDK APP END
