#
# Copyright (c) 2020 Nordic Semiconductor
# Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(Git)
project(IFMCU)

# NORDIC SDK APP START
target_sources(app PRIVATE 
  src/main.c
  )
# NORDIC SDK APP END

# Include application events and disk files
zephyr_library_include_directories(
  src/events
  src/modules
  )

# Application sources
add_subdirectory(src/events)
add_subdirectory(src/modules)
