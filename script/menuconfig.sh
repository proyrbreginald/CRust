#!/bin/bash
# ============================================================
# script/menuconfig.sh — Kconfig 交互式配置（menuconfig）
# ============================================================
# 功能：
#   在终端中打开基于文本的 Kconfig 配置菜单，供用户交互修改。
#   修改结果保存到项目根目录的 .config 文件中。
# ============================================================
set -e

# 获取项目根目录
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 先确保虚拟环境已就绪
source "$SCRIPT_DIR/venv.sh"

echo "============================================"
echo " Kconfig 交互式配置"
echo "============================================"
echo "[INFO] 正在打开配置菜单..."
echo "[INFO] 保存后请运行 Genconfig 任务生成头文件"
echo "============================================"

# 进入项目根目录执行，确保 Kconfig 中的相对路径正确
cd "$PROJECT_DIR"

# 启动文本交互式配置界面
menuconfig Kconfig

echo "[INFO] 配置已保存到 $(pwd)/.config"
