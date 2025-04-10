#!/bin/bash
# macOS构建脚本

# 创建构建目录
mkdir -p build

# 检测处理器架构
ARCH=$(uname -m)

# 根据处理器架构选择不同的配置
if [ "$ARCH" = "x86_64" ]; then
    echo "检测到Intel处理器(x86_64)"
    # Intel处理器配置
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=MAC -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output
else
    echo "检测到ARM处理器(arm64)"
    # ARM处理器配置
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output
fi

# 编译项目 - 使用多线程编译
cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)

# 安装到output目录
cmake --install build --config Release

echo "macOS构建完成，安装目录为output" 