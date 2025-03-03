---
title: 编译
editLink: true
description: 本篇将介绍各个系统平台GPUPixel库的编译方法
---
<Badge type="tip" text="版本: 1.3.0-beta" />

# 编译

本章将介绍各个系统平台GPUPixel库的编译方法

::: tip
从 [v1.3.0-beta](https://github.com/pixpark/gpupixel/releases/tag/v1.3.0-beta) 版本开始，移除了Vnn相关依赖库。最新编译好的各个平台库可以从[这里](https://github.com/pixpark/gpupixel/releases/latest)找到
:::

## iOS
工程根目录下执行如下命令

**生成工程**
::: code-group
```bash [Arm64]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=OS64 -DCMAKE_BUILD_TYPE=Release
```
:::

**编译**
::: code-group

```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::

**输出**

编译输出位于项目根目录下的 `output` 路径，包含内容如下
```bash
output
├── include   #头文件
├── library   #库文件
├── models    #模型文件
└── res       #资源文件
```
对于iOS只需要使用 `library` 下的 `.framework` 库即可，里面已经包含头文件和资源文件

## Mac
工程根目录下执行如下命令

**生成工程**
::: code-group
```bash [Apple Silicon]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 -DCMAKE_BUILD_TYPE=Release
```
```bash [Intel]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=MAC -DCMAKE_BUILD_TYPE=Debug
```
:::
 
**编译**
::: code-group

```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::

**输出**

编译输出位于项目根目录下的 `output` 路径，包含内容如下
```bash
output
├── include   #头文件
├── library   #库文件
├── models    #模型文件
└── res       #资源文件
```
对于MacOS只需要使用 `library` 下的 `.framework` 库即可，里面已经包含头文件和资源文件

## Android

**Gradle 命令编译**

如果已经配置号 gradle 命令环境，也可以使用 `gradlew` 编译

```bash
./gradlew :gpupixel:assembleRelease
```

输出位于：`src/android/java/gpupixel/build/outputs/aar`

## Windows

Windows编译需要安装 Cmake 和 Visual Studio 2017+，主要使用了VS的NMmake编译，
打开 `x64 Native Tools Command Prompt for VS 2022` 命令窗口，按以下命令编译

> 仅支持生成 x86_64版本，不支持 x86_32

**生成工程**

```bash
cmake -G "NMake Makefiles" -B build -S src -DCMAKE_BUILD_TYPE=Release
```

**编译**

```bash
cmake --build build --config Release
```

**输出**

编译输出位于项目根目录下的 `output` 路径，包含内容如下
```bash
output
├── include   #头文件
├── library   #库文件
├── models    #模型文件
└── res       #资源文件
```

## Linux (Debian 10)

由于依赖了mars-face和mnn静态库，GLIBC版本的问题，必须使用Debian 10或者 GLIBC 2.28版本的linux系统编译
**环境配置**

```bash
# install cmake 
sudo apt-get install cmake pkg-config
# install dependent lib
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev
```

**生成工程**
::: code-group

```bash [Release]
cmake -B build -S src -DCMAKE_BUILD_TYPE=Release
```

```bash [Debug]
cmake -B build -S src -DCMAKE_BUILD_TYPE=Debug
```
:::

**编译**

::: code-group

```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::
 

**输出**

编译输出位于项目根目录下的 `output` 路径，包含内容如下
```bash
output
├── include   #头文件
├── library   #库文件
├── models    #模型文件
└── res       #资源文件
```

## Github Workflows
自动化编译可以参考
[GPUPixel Github Build Workflows](https://github.com/pixpark/gpupixel/blob/main/.github/workflows/release.yml)，脚本化了上面讲到的各个平台的编译命令