# 1. 指定基础镜像，官方推荐使用特定版本号而非 latest，以保证环境一致性
FROM ubuntu:24.04

# 2. 设置环境变量，禁用安装时的交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 3. 设置容器内的工作目录
WORKDIR /workspace

# 4. 更新软件源
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list
RUN apt update && apt upgrade -y

# 5. 安装依赖
# RUN apt install -y \
#     wget \
#     python3 \
#     python3-pip \
#     scons \
#     meson \
#     ninja-build \
#     nano

# 6. 清理缓存
RUN apt clean && rm -rf /var/lib/apt/lists/*

# 构建镜像：docker build -t test:ubuntu-24 .
# 启动容器：docker run --rm -it -v $(pwd):/workspace test:ubuntu-24 /bin/bash
# 查看容器：docker ps -a
# 删除容器：docker rm <id>
# 删除镜像：docker rmi <id>