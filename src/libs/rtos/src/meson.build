# SPDX-License-Identifier: Apache-2.0
#
# RT-Thread v5.2.2 Kernel + klibc 源文件列表
#
# 注意：以下文件内部均有 #ifdef 条件编译保护，
# 实际链接的内容由 rtconfig.h 中的宏定义控制。

_rtos_kernel_src = files(
    'clock.c',
    'components.c',
    'cpu_up.c',             # 单核 (UP) 版本；若启用 SMP 需换为 cpu_mp.c
    'defunct.c',
    'idle.c',
    'ipc.c',
    'irq.c',
    'kservice.c',
    'mem.c',
    'memheap.c',
    'mempool.c',
    'object.c',
    'scheduler_comm.c',
    'scheduler_up.c',       # 单核 (UP) 版本；若启用 SMP 需换为 scheduler_mp.c
    'signal.c',
    'slab.c',
    'thread.c',
    'timer.c',
)

# klibc —— RT-Thread 内核必备的 libc 补充实现
_rtos_klibc_src = files(
    'klibc/kerrno.c',
    'klibc/kstdio.c',
    'klibc/kstring.c',
    'klibc/rt_vsnprintf_tiny.c',   # 精简版 vsnprintf；如需完整版可换为 rt_vsnprintf_std.c
    'klibc/rt_vsscanf.c',
)
