---
title: 集成
editLink: true
outline: deep
description: 本篇将介绍各个系统平台GPUPixel库的集成调用方法
---
<Badge type="tip" text="版本: 1.3.0-beta" />

# 集成

本章将介绍GPUPixel在各个系统的集成方法，最新编译好的各个平台库可以从[这里](https://github.com/pixpark/gpupixel/releases/latest)下载

## iOS & MacOS

iOS 和 MacOS库的链接形式相同，放到一块来讲

### 添加依赖

将gpupixel库拷贝到你的工程目录

::: code-group
```bash [iOS]
├── gpupixel.framework
```
```bash [MacOS]
├── gpupixel.framework
```
:::

选择`工程 -> Targets -> Build Phases -> Link Binary With Libraries`

点击左下角的 `+` 号添加如上面的依赖，`CoreMedia.framework` 和 `AVFoundation.framework` 为系统库，提供相机采集等能力
**Framework 查找路径**

选择`工程 -> Targets -> Build Settings -> Search Paths -> Framework Search Paths`, 将 gpupixel存放路径填入

**嵌入Framework**

选择`工程 -> Targets -> General -> Frameworks, Libraries, and Embeded Content`

将gpupixel库设置为 `Embed & Sign`, 系统库选择` Do Not Embed `

### Objective-C 调用

由于GPUPixel采用C++编写，OC要调用C++需要将OC源文件拓展名由 `.m` 改为 `.mm`，这样就可以实现 OC和C++混编。

GPUPixel对外所有的头文件都包含在 `gpupixel.h`中，只要引入 

```objective-c
#import <gpupixel/gpupixel.h>
``` 

### Swift 调用
Swift 5.9 之后可以直接与C++交互，也是通过桥接头文件的方式，省去了OC在中间转一道，可以参考[这篇文章](https://cloud.tencent.com/developer/article/2312347) 或 Swift 官方文档[Mixing Swift and C++](https://www.swift.org/documentation/cxx-interop/#calling-c-functions)

## Android

下载最新 `gpupixel-release.aar`, 放入安卓工程下，如 `libs` 目录

在 gradle 中添加依赖

```gradle
dependencies {
    implementation files('libs/gpupixel-release.aar')
}
```
然后 sync 一下工程即可，参考 Android Demo

参考文章：[Android 导入AAR包](https://juejin.cn/post/7226600031569510459)

## Windows and Linux
参考 [demo](https://github.com/pixpark/gpupixel/blob/main/examples/desktop/app.cc) 和 [Cmake配置](https://github.com/pixpark/gpupixel/blob/main/examples/CMakeLists.txt)