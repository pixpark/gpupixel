#!/bin/bash
# Linux构建脚本

# 创建构建目录
mkdir -p build

# 配置项目 - Release模式
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output

# 编译项目 - 使用多线程编译
cmake --build build --config Release --parallel $(nproc)

# 安装到output目录
cmake --install build --config Release

echo "Linux构建完成，安装目录为output" 