# 私有源文件
_rtt_src = files(
    './src/rtt.c',
    './src/rtt_port.c',
)

# 私有头文件路径
_rtt_inc = include_directories('./inc')

# 声明依赖对象
rtt_dep = declare_dependency(
    sources : _rtt_src,
    include_directories : _rtt_inc,
)
