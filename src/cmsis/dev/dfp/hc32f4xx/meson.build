# 私有源文件
_hc32f4xx_src = files('./src/system_hc32f460.c', './src/reset.c', './src/gcc/vector_table.c')

# 私有头文件路径
_hc32f4xx_inc = include_directories('./inc')

# 链接脚本文件的绝对路径
linker_script_path = meson.current_source_dir() / 'src/gcc/link/link.ld'

# 添加依赖
hc32f4xx_dep = declare_dependency(
    compile_args : ['-DHC32F460'],
    link_args : ['-T', linker_script_path],
    sources : _hc32f4xx_src,
    include_directories : _hc32f4xx_inc,
)