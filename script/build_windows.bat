@echo off
:: Windows构建脚本 - 使用MSVC编译器

:: 创建构建目录
if not exist build mkdir build

:: 配置项目 - Release模式
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output

:: 编译项目 - 使用多线程编译
cmake --build build --config Release --parallel %NUMBER_OF_PROCESSORS%

:: 安装到output目录
cmake --install build --config Release

echo Windows构建完成，安装目录为output 