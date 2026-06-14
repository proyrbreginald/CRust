# 私有源文件
_app_src = files(
    './src/startup.c',
    './src/main.c',
)

# 私有头文件路径
_app_inc = include_directories('./inc')

# 添加依赖
app_dep = declare_dependency(
    sources : _app_src,
    include_directories : _app_inc,
)