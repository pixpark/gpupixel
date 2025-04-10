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
工程根目录下执行脚本文件：

```bash
./script/build_ios.sh
```

**输出**

编译输出位于项目根目录下的 `output` 路径

## Mac
工程根目录下执行脚本文件：

```bash
./script/build_macos.sh
```

**输出**

编译输出位于项目根目录下的 `output` 路径
 
## Android
工程根目录下执行脚本文件：

```bash
./script/build_android.sh
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

**编译**

在配置好环境后，工程根目录下执行批处理文件：

```bash
.\script\build_windows.bat
```

**输出**

编译输出位于项目根目录下的 `output` 路径

## Linux (Debian or Ubuntu)

**环境配置**

可以使用环境配置脚本安装依赖：

```bash
./script/setup_env_linux.sh
```

**编译**

工程根目录下执行脚本文件：

```bash
./script/build_linux.sh
```

**输出**

编译输出位于项目根目录下的 `output` 路径