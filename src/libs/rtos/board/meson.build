# SPDX-License-Identifier: Apache-2.0
#
# RT-Thread board glue (project-specific, not the RT-Thread BSP)

_rtos_board_src = files('board.c')

_rtos_board_inc = include_directories('.')

rtos_board_dep = declare_dependency(
    sources: _rtos_board_src,
    include_directories: _rtos_board_inc,
)
