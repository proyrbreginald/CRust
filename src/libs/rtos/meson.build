# ---------- 子目录 ----------
subdir('board')
subdir('components')
subdir('src')

# ---------- LibCPU (ARM Common + Cortex-M4, GCC) ----------
_rtos_libcpu_src = files(
    # ARM 通用
    'libcpu/arm/common/atomic_arm.c',   # rt_hw_atomic_* 硬件原子操作
    'libcpu/arm/common/div0.c',         # 除零错误处理
    'libcpu/arm/common/showmem.c',      # 内存内容显示
    # Cortex-M4 移植
    'libcpu/arm/cortex-m4/context_gcc.S',
    'libcpu/arm/cortex-m4/cpuport.c',
)

# ---------- 头文件搜索路径 ----------
# 注意：项目自有路径需要排在前面，以确保项目级的 hc32f4xx_conf.h
#       （LL_USB_ENABLE = DDL_OFF）优先于其他版本被找到。
_rtos_inc = include_directories(
    'components/drivers/include',           # rtdevice.h、drivers/*.h
    'components/finsh',                     # finsh.h
    'components/drivers/include/drivers',   # dev_serial.h、dev_pin.h 等
    'components/drivers/include/ipc',       # ringbuffer.h、workqueue.h 等
    'inc',                                  # RT-Thread 内核公共头文件
    # 注意：libc/compilers/common/include 不加入全局路径，
    # 因为其中的 sys/time.h 等会拦截 newlib 的同名头文件，
    # 导致循环依赖和类型未定义错误。newlib 工具链无需此路径。
)

# ---------- 编译选项 ----------
# __RT_KERNEL_SOURCE__ 和 __RT_IPC_SOURCE__ 用于解锁 rtsched.h 中
# 仅供内核/IPC 内部使用的调度器函数声明。全局定义无害，且简化了构建。
_rtos_c_args = [
    '-Wno-unused-parameter',
    '-Wno-missing-field-initializers',
    '-D__RT_KERNEL_SOURCE__',               # 解锁内核内部接口
    '-D__RT_IPC_SOURCE__',                  # 解锁 IPC 内部接口
]

# ---------- 声明依赖对象 ----------
rtos_dep = declare_dependency(
    sources: [
        _rtos_libcpu_src,
        _rtos_klibc_src,
        _rtos_kernel_src,
        _rtos_components_src,
    ],
    include_directories: _rtos_inc,
    compile_args: _rtos_c_args,
    dependencies: rtos_board_dep,
)