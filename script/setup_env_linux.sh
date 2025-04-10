#!/bin/bash

# ========================================
# 脚本名称: setup_deps.sh
# 功能：强制安装所有开发依赖（不检查现有环境）
# 支持系统：Ubuntu/Debian/CentOS/RHEL
# ========================================

# 仅支持 Linux 系统
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "错误：本脚本仅支持 Linux 系统"
    exit 1
fi

# 检测发行版类型
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
elif type lsb_release >/dev/null 2>&1; then
    OS=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
else
    echo "无法检测 Linux 发行版"
    exit 1
fi

# ========================================
# 安装所有依赖（不检查现有环境）
# ========================================
install_all_deps() {
    case $OS in
        ubuntu|debian)
            echo "[Ubuntu/Debian] 强制安装所有开发依赖..."
            sudo apt-get update
            # 安装编译工具链
            sudo apt-get install -y build-essential
            # 安装 CMake
            sudo apt-get install -y cmake
            # 安装 X11/OpenGL 依赖
            sudo apt-get install -y \
                libx11-dev libxrandr-dev libxinerama-dev \
                libxcursor-dev libxi-dev \
                libglu1-mesa-dev freeglut3-dev mesa-common-dev
            ;;

        centos|rhel|fedora|amzn)
            echo "[RedHat 系] 强制安装所有开发依赖..."
            # 安装编译工具链
            if command -v dnf >/dev/null 2>&1; then
                sudo dnf groupinstall -y "Development Tools"
                sudo dnf install -y gcc-c++
            elif command -v yum >/dev/null 2>&1; then
                sudo yum groupinstall -y "Development Tools"
                sudo yum install -y gcc-c++
            else
                echo "未找到 yum/dnf 包管理器"
                exit 1
            fi

            # 安装 CMake
            sudo $PKG_MGR install -y cmake

            # 安装 X11/OpenGL 依赖
            sudo $PKG_MGR install -y \
                libX11-devel libXrandr-devel libXinerama-devel \
                libXcursor-devel libXi-devel \
                mesa-libGLU-devel freeglut-devel mesa-common-devel
            ;;

        *)
            echo "不支持的发行版: $OS"
            exit 1
            ;;
    esac
}

# ========================================
# 主逻辑
# ========================================
# 确定包管理器（RedHat 系需要）
if [[ "$OS" == "centos" || "$OS" == "rhel" || "$OS" == "amzn" ]]; then
    PKG_MGR=$(command -v dnf || command -v yum)
fi

# 强制安装所有依赖（无论现有环境如何）
install_all_deps

# 验证安装结果
echo "验证安装结果："
command -v gcc && echo " - GCC 已安装: $(gcc --version | head -n1)"
command -v cmake && echo " - CMake 已安装: $(cmake --version | head -n1)"
echo "所有指定开发库已强制安装完成"