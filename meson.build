project('pd-embedded', 'c', default_options : ['c_std=gnu17'])

# 进入 src 目录，执行其中的 meson.build
subdir('src')

# 声明编译器参数
common_c_args = [
    '-mcpu=cortex-m4',      # 替换为你的MCU核心，如 cortex-m3, cortex-m0plus
    '-mthumb',              # 使用Thumb指令集
    '-mfloat-abi=hard',     # 如果你的MCU有FPU，可改为 'hard'
    '-ffunction-sections',  # 优化，未使用的函数不链接
    '-fdata-sections',      # 优化，未使用的数据不链接
    '-Wall',                # 开启常用警告
    '-Wextra',              # 开启额外警告
    '-Os',                  # 优化代码体积
    '-g',                   # 保留调试信息
    '-Wno-main',            # 忽略无 main 函数警告
    '-Wno-main-return-type',# 忽略 main 函数返回类型不标准的警告
]

common_link_args = [
    '-mcpu=cortex-m4',
    '-mthumb',
    '-mfloat-abi=hard',
    '-Wl,--gc-sections',            # 删除未使用的段
    '-Wl,-Map=output.map',          # 生成内存映射文件（相对 build 目录）
    '-Wl,--no-warn-rwx-segments',   # 裸机系统忽略 RWX 段警告（Cortex-M 无 MMU 强制执行）
    '-nostartfiles',              # 不使用标准启动文件，使用自己的
]

# 声明可执行文件（EL格式）
pd_embedded = executable('pd-embedded',
    name_suffix : 'elf',            # 生成 pd-embedded.elf
    c_args : common_c_args,
    link_args : common_link_args,
    dependencies : src_dep,         # 传递依赖链
)

# 自定义目标：将 ELF 转换为 HEX（Intel HEX 格式）
custom_target('pd-embedded.hex',
    input : pd_embedded,
    output : 'pd-embedded.hex',
    command : ['arm-none-eabi-objcopy', '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
    build_by_default : false,
)

# 自定义目标：将 ELF 转换为 BIN（纯二进制格式）
custom_target('pd-embedded.bin',
    input : pd_embedded,
    output : 'pd-embedded.bin',
    command : ['arm-none-eabi-objcopy', '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    build_by_default : false,
)