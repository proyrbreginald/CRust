# CRust

基于 Meson 构建系统的 RT-Thread 嵌入式项目 (HC32F460)。

## 快速开始

### 1. Python 虚拟环境

```bash
# 创建虚拟环境
python3 -m venv .venv

# 激活虚拟环境
source .venv/bin/activate

# 安装依赖（pyOCD 等工具）
pip install -r requirements.txt
```

### 2. VS Code 状态栏构建

本项目已配置好 VS Code 任务，通过底部状态栏按钮即可完成全部构建流程：

| 按钮 | 功能 | 说明 |
|------|------|------|
| **Clean** | 清理 | 清理编译产物 |
| **Setup** | 首次配置 | `meson setup build --cross-file cross_file.txt` |
| **Resetup** | 重新配置 | 修改 `cross_file.txt` 或 `meson.build` 后使用 |
| **Build** | 编译 | `meson compile -C build` |
| **Bin** | 生成镜像 | 生成 `pd-embedded.hex` / `pd-embedded.bin` |
| **Flash** | 烧录 | 通过 pyOCD 烧录 ELF 到芯片 |
| **Auto** | 一键流程 | 依次执行 **Clean → Setup → Build → Flash** |

依次点击状态栏的对应按钮即可，无需手动输入 Meson 命令。

### 3. Kconfig 配置

```bash
cd src/libs/rtos

# 基于 Kconfig 生成 .config（文本交互）
menuconfig Kconfig

# 将 .config 转换为 menuconfig.h
genconfig --header-path ./src/config/inc/menuconfig.h
```