# Copyright (c) 2016-2026 Makerdiary <https://makerdiary.com>
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_BQ2518X bq2518x.c)
