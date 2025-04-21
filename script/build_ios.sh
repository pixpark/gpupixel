#!/bin/bash
# iOS构建脚本

# 创建构建目录
mkdir -p build

# 配置项目 - 使用iOS工具链
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=output

# 编译项目 - 使用多线程编译
cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)

# 安装到output目录
cmake --install build --config Release

echo "iOS构建完成, 安装目录为output" 