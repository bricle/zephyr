# SPDX-License-Identifier: Apache-2.0

# keep first
board_runner_args(pyocd "--target=STM32F407ZGTx")

# keep first
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
