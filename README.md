# CRust

基于 Meson 构建系统的 RT-Thread 嵌入式项目 (HC32F460)。

## 快速开始

### 1. Python 虚拟环境

```bash
# 创建虚拟环境
python3 -m venv .venv

# 激活虚拟环境
source .venv/bin/activate

# 安装 pip
python -m ensurepip --upgrade

# 安装依赖（pyOCD 等工具）
pip install -r requirements.txt

# 导出最简依赖
pip list --not-required --format=freeze > requirements.txt
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
| **Flash** | 烧录 | 通过 pyOCD 烧录 ELF 到芯片（自动检查虚拟环境） |
| **Auto** | 一键流程 | 依次执行 **Clean → Setup → Build → Flash** |
| **Venv** | 虚拟环境 | Python 虚拟环境检查/配置 |
| **Menuconfig** | Kconfig 配置 | 交互式 Kconfig 配置菜单 |
| **Genconfig** | 生成头文件 | 从 `.config` 生成 `menuconfig.h` |

依次点击状态栏的对应按钮即可，无需手动输入 Meson 命令。

> **提示**：`Flash` 任务已自动依赖 `Venv`，烧录前会自动检查虚拟环境状态。
> Kconfig 配置流程：先点 **Menuconfig** 交互配置 → 再点 **Genconfig** 生成头文件。

### 3. Kconfig 配置

```bash
# 交互式配置（推荐通过 VS Code 状态栏的 Menuconfig 按钮）
./script/kconfig-mconf.sh

# 从 .config 生成头文件（推荐通过 Genconfig 按钮）
./script/kconfig-gen.sh
```

也可手动分步执行：

```bash
# 1. 基于 Kconfig 生成 .config（文本交互）
menuconfig Kconfig

# 2. 将 .config 转换为 menuconfig.h
genconfig --header-path ./src/config/inc/menuconfig.h

# 3. 将 CONFIG_ 过滤掉
sed -i 's/#define CONFIG_/#define /g' src/config/inc/menuconfig.h
```

### 4. 辅助脚本说明

| 脚本 | 功能 |
|------|------|
| `script/check-venv.sh` | 检查 `.venv` 虚拟环境，不存在则自动创建并安装依赖 |
| `script/kconfig-mconf.sh` | 启动交互式 Kconfig 配置菜单（自动调用 `check-venv.sh`） |
| `script/kconfig-gen.sh` | 从 `.config` 生成 `menuconfig.h` 头文件（自动调用 `check-venv.sh`） |