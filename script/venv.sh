#!/bin/bash
# ============================================================
# script/check-venv.sh — Python 虚拟环境检查与配置
# ============================================================
# 功能：
#   1. 检查 .venv 是否存在，若不存在则自动创建
#   2. 激活虚拟环境，确保 pip 为最新
#   3. 安装 / 更新 requirements.txt 中的依赖
# ============================================================
set -e

# 获取项目根目录（脚本所在目录的上一级）
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
VENV_DIR="$PROJECT_DIR/.venv"
REQUIREMENTS="$PROJECT_DIR/requirements.txt"

echo "============================================"
echo " Python 虚拟环境检查"
echo "============================================"

# 1. 创建虚拟环境（若不存在）
if [ ! -d "$VENV_DIR" ]; then
    echo "[INFO] 未检测到 .venv，正在创建..."
    python3 -m venv "$VENV_DIR"
    echo "[INFO] 虚拟环境已创建: $VENV_DIR"
else
    echo "[INFO] 虚拟环境已存在: $VENV_DIR"
fi

# 2. 激活虚拟环境
source "$VENV_DIR/bin/activate"

# 3. 确保 pip 可用且为最新
echo "[INFO] 正在更新 pip ..."
python -m ensurepip --upgrade 2>/dev/null || true
pip install --upgrade pip --quiet

# 4. 安装 / 更新项目依赖
if [ -f "$REQUIREMENTS" ]; then
    echo "[INFO] 正在安装/更新依赖 (requirements.txt) ..."
    pip install -r "$REQUIREMENTS" --quiet
    echo "[INFO] 依赖安装完成"
else
    echo "[WARN] 未找到 requirements.txt，跳过依赖安装"
fi

echo "[INFO] 虚拟环境就绪: $(which python)"
echo "============================================"
