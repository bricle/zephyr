# Copyright (c) 2020 Google LLC.
# SPDX-License-Identifier: Apache-2.0

board_runner_args(openocd --target-handle=_CHIPNAME.cpu0)
board_runner_args(pyocd "--target=ATSAME51J20A" "--frequency=4000000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)