#!/bin/bash
# ============================================================
# script/genconfig.sh — 从 .config 生成 menuconfig.h 头文件
# ============================================================
# 功能：
#   1. 检查 .config 是否存在
#   2. 用 genconfig 将 .config 转换为 C 头文件
#   3. 去除宏定义中的 "CONFIG_" 前缀
# ============================================================
set -e

# 获取项目根目录
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
CONFIG_FILE="$PROJECT_DIR/.config"
OUTPUT_HEADER="$PROJECT_DIR/src/config/inc/menuconfig.h"

# 先确保虚拟环境已就绪
source "$SCRIPT_DIR/venv.sh"

echo "============================================"
echo " Kconfig 头文件生成"
echo "============================================"

# 检查 .config 是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    echo "[ERROR] 未找到 .config 文件！"
    echo "[ERROR] 请先运行 Menuconfig 任务进行配置。"
    exit 1
fi

cd "$PROJECT_DIR"

echo "[INFO] 确保输出目录存在 ..."
mkdir -p "$(dirname "$OUTPUT_HEADER")"

echo "[INFO] 从 .config 生成头文件 ..."
genconfig --header-path "$OUTPUT_HEADER"

echo "[INFO] 去除 CONFIG_ 前缀 ..."
sed -i 's/#define CONFIG_/#define /g' "$OUTPUT_HEADER"

echo "[INFO] 头文件已生成: $OUTPUT_HEADER"
