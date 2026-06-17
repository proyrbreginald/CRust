# SPDX-License-Identifier: Apache-2.0
#
# RT-Thread v5.2.2 组件源文件列表
#
# 组件说明：
#   finsh         —— 命令行外壳 (MSH/Finsh)
#   drivers/core  —— 设备驱动框架核心
#   drivers/ipc   —— 设备间通信 (pipe/ringbuffer/workqueue/…)
#   drivers/serial—— 串口设备框架
#   drivers/pin   —— GPIO 引脚设备框架
#   drivers/i2c   —— I2C 设备框架
#   libc/common   —— libc 桥接层 (ctype/stdlib/string/time/…)
#
# 每个源文件内部均有 #ifdef 条件编译，由 rtconfig.h 控制实际生效的代码。

_rtos_components_src = files(
    # ---- Finsh / MSH 命令行 ----
    'finsh/cmd.c',
    'finsh/msh_file.c',          # 内部有 #ifdef DFS_USING_POSIX 保护
    'finsh/msh_parse.c',
    'finsh/msh.c',
    'finsh/shell.c',

    # ---- 设备驱动核心 ----
    # device.c — 基础设备模型（必需）
    'drivers/core/device.c',
    # bus.c/dm.c/driver.c/platform.c — 需要 RT_USING_DM/RT_USING_DEV_BUS，
    # 当前 rtconfig.h 未启用，暂不加入。启用后取消下面注释：
    # 'drivers/core/bus.c',
    # 'drivers/core/dm.c',
    # 'drivers/core/driver.c',
    # 'drivers/core/platform.c',

    # ---- 设备间通信 (IPC) ----
    'drivers/ipc/completion_comm.c',
    'drivers/ipc/completion_up.c',
    'drivers/ipc/condvar.c',
    'drivers/ipc/dataqueue.c',
    'drivers/ipc/ringblk_buf.c',
    'drivers/ipc/ringbuffer.c',
    'drivers/ipc/waitqueue.c',
    'drivers/ipc/workqueue.c',

    # ---- 串口设备框架 (暂不启用，使用项目自有 LL UART) ----
    # 'drivers/serial/dev_serial.c',

    # ---- GPIO / Pin 设备框架 (暂不启用，使用项目自有 LL GPIO) ----
    # 'drivers/pin/dev_pin.c',

    # ---- I2C 设备框架 (暂不启用，使用项目自有 LL I2C) ----
    # 'drivers/i2c/dev_i2c_core.c',
    # 'drivers/i2c/dev_i2c_dev.c',
    # 'drivers/i2c/dev_i2c_bit_ops.c',
    # 'drivers/i2c/dev_soft_i2c.c',

    # ---- newlib syscalls (ARM GCC 工具链) ----
    # 提供 _sbrk/_write/_read 等桩函数，使 newlib 的 malloc/printf 正常运作。
    # 需要 posix/stdlib.h 头文件支持，当前因路径冲突暂不启用。
    # 启用时请将 components/libc/compilers/common/include 加入头文件路径。
    # 'libc/compilers/newlib/syscalls.c',
)
