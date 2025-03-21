---
title: 编译
editLink: true
description: 本篇将介绍各个系统平台GPUPixel库的编译方法
---

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

Windows编译需要安装以下环境：

**环境要求**
- Microsoft Visual Studio >= 2017
- cmake >= 3.10

**编译环境配置**

推荐使用 Windows Terminal，在Terminal中选择对应的VS环境Tab后进行编译。
![Windows Terminal](../../image/win-terminal.png)
如不使用Windows Terminal，可按如下方式配置：

- **64位编译**：在设置中找到`vcvars64.bat`（适用于 VS 2017 的 x64 本机工具命令提示）并单击，打开VS编译x64架构程序的虚拟环境
 
::: warning
仅支持生成 x86_64 Release 版本，暂不支持 x86_32
:::

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

## Linux (Debian or Ubuntu)

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