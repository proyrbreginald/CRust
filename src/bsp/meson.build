# ============================================================
# src/bsp/meson.build — 板级支持包 (Bare-metal BSP)
# ============================================================

# 私有源文件
_bsp_src = files(
    './src/bsp_init.c',
    './src/bsp_timer.c',
)

# 私有头文件路径
_bsp_inc = include_directories('./inc')

# 添加依赖
bsp_dep = declare_dependency(
    sources : _bsp_src,
    include_directories : _bsp_inc,
)
