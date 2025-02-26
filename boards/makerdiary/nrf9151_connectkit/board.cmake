# Copyright (c) 2016-2025 Makerdiary <https://makerdiary.com>
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BOARD_NRF9151_CONNECTKIT_NRF9151 OR CONFIG_BOARD_NRF9151_CONNECTKIT_NRF9151_NS)
  if(CONFIG_BOARD_NRF9151_CONNECTKIT_NRF9151_NS)
    set(TFM_PUBLIC_KEY_FORMAT "full")
  endif()
  if(CONFIG_TFM_FLASH_MERGED_BINARY)
    set_property(TARGET runners_yaml_props_target PROPERTY hex_file tfm_merged.hex)
  endif()
  board_runner_args(pyocd "--target=nrf91" "--frequency=4000000")
elseif(CONFIG_BOARD_NRF9151_CONNECTKIT_NRF52820)
  board_runner_args(uf2 "--board-id=NRF9151-CONNECT-KIT-IFMCU")
  # TODO: change to nrf52820 when such device is available in pyocd
  board_runner_args(pyocd "--target=nrf52833")
  include(${ZEPHYR_BASE}/boards/common/uf2.board.cmake)
endif()

include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
