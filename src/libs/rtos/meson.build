# SPDX-License-Identifier: Apache-2.0
#
# RT-Thread v5.2.2 标准版 —— Meson 构建集成
#
# 使用方法：
#   1. 将 rtconfig.h 放置于 src/config/inc/ 下（与 hc32f4xx_conf.h 同级），
#      或放置于本目录的上级任意 -I 搜索路径中。
#   2. 在本文件的 _rtos_bsp_src 列表中添加/删除 HC32 BSP 驱动文件。
#   3. 在 src/libs/meson.build 中添加 subdir('rtos') 引用本文件。

# ---------- 子目录 ----------
subdir('src')
subdir('components')

# ---------- LibCPU (ARM Common + Cortex-M4, GCC) ----------
_rtos_libcpu_src = files(
    # ARM 通用
    'libcpu/arm/common/atomic_arm.c',   # rt_hw_atomic_* 硬件原子操作
    'libcpu/arm/common/div0.c',         # 除零错误处理
    'libcpu/arm/common/showmem.c',      # 内存内容显示
    # Cortex-M4 移植
    'libcpu/arm/cortex-m4/cpuport.c',
    'libcpu/arm/cortex-m4/context_gcc.S',
)

# ---------- HC32 BSP 驱动层 ----------
# drv_common.c  提供 rt_hw_board_init()、SysTick_Handler 等板级初始化
# drv_irq.c     中断管理（中断嵌套、使能/除能）
# drv_gpio.c    GPIO/Pin 设备驱动（内部 #ifdef RT_USING_PIN 保护）
# drv_usart.c   串口设备驱动（内部 #ifdef RT_USING_SERIAL 保护）
# drv_i2c.c     I2C 设备驱动（内部 #ifdef RT_USING_I2C 保护）
_rtos_bsp_src = files(
    'bsp/hc32/libraries/hc32_drivers/drv_common.c',
    'bsp/hc32/libraries/hc32_drivers/drv_irq.c',
    'bsp/hc32/libraries/hc32_drivers/drv_gpio.c',
    # drv_usart.c — 暂不启用，因 DMA 初始化结构与项目 LL 驱动版本有差异。
    #               启用时请同步检查 hc32_dma.h 中的 stc_dma_config_t 定义。
    # 'bsp/hc32/libraries/hc32_drivers/drv_usart.c',
    # drv_i2c.c   — 暂不启用，因所用 I2C API 与项目 LL 驱动版本不匹配。
    # 'bsp/hc32/libraries/hc32_drivers/drv_i2c.c',
)

# ---------- 头文件搜索路径 ----------
# 注意：项目自有路径需要排在 BSP 路径之前，以确保项目级的 hc32f4xx_conf.h
#       （LL_USB_ENABLE = DDL_OFF）优先于 BSP 版本（DDL_ON）被找到。
_rtos_inc = include_directories(
    'inc',                                  # RT-Thread 内核公共头文件
    '.',                                    # 放置 rtconfig.h 的候选位置
    'components/drivers/include',           # rtdevice.h、drivers/*.h
    'components/finsh',                     # finsh.h
    'components/drivers/include/drivers',   # dev_serial.h、dev_pin.h 等
    'components/drivers/include/ipc',       # ringbuffer.h、workqueue.h 等
    # 注意：libc/compilers/common/include 不加入全局路径，
    # 因为其中的 sys/time.h 等会拦截 newlib 的同名头文件，
    # 导致循环依赖和类型未定义错误。newlib 工具链无需此路径。

    # 项目自有头文件（优先搜索）
    '../../config/inc',     # hc32f4xx_conf.h、rtconfig.h
    '../../driver/inc',     # hc32_ll.h 及全部 LL 外设头文件

    # BSP 驱动与板级支持（放在项目路径之后，避免覆盖项目配置）
    'bsp/hc32/libraries/hc32_drivers',      # BSP 驱动头文件
    'bsp/hc32/ev_hc32f460_lqfp100_v2/board',          # board.h、board_config.h
    'bsp/hc32/ev_hc32f460_lqfp100_v2/board/config',   # drv_config.h、uart_config.h
    'bsp/hc32/ev_hc32f460_lqfp100_v2/board/ports',    # fal_cfg.h、usb_config.h
)

# ---------- 编译选项 ----------
# __RT_KERNEL_SOURCE__ 和 __RT_IPC_SOURCE__ 用于解锁 rtsched.h 中
# 仅供内核/IPC 内部使用的调度器函数声明。全局定义无害，且简化了构建。
_rtos_c_args = [
    '-Wno-unused-parameter',
    '-Wno-missing-field-initializers',
    '-include', 'rtconfig.h',               # 确保每个编译单元都能看到 rtconfig.h
    '-D__RT_KERNEL_SOURCE__',               # 解锁内核内部接口
    '-D__RT_IPC_SOURCE__',                  # 解锁 IPC 内部接口
]

# ---------- 声明依赖对象 ----------
rtos_dep = declare_dependency(
    sources: [
        _rtos_kernel_src,
        _rtos_klibc_src,
        _rtos_components_src,
        _rtos_libcpu_src,
        _rtos_bsp_src,
    ],
    include_directories: _rtos_inc,
    compile_args: _rtos_c_args,
)