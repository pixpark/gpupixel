---
title: Integration
editLink: true
outline: deep
description: This article will introduce the integration and invocation methods of the GPUPixel library across various system platforms.
---
<Badge type="tip" text="Version: 1.3.0-beta" />

# Integration

This chapter will introduce the integration methods of GPUPixel across different systems. The latest precompiled libraries for each platform can be downloaded from [here](https://github.com/pixpark/gpupixel/releases/latest).

## iOS & MacOS

The linking methods for iOS and MacOS libraries are the same, so they are discussed together.

### Adding Dependencies

Copy the GPUPixel library to your project directory.

::: code-group
```bash [iOS]
├── gpupixel.framework
```
```bash [MacOS]
├── gpupixel.framework
```
:::

Select `Project -> Targets -> Build Phases -> Link Binary With Libraries`.

Click the `+` button at the bottom left to add the dependencies as shown above. `CoreMedia.framework` and `AVFoundation.framework` are system libraries that provide capabilities such as camera capture.

**Framework Search Path**

Select `Project -> Targets -> Build Settings -> Search Paths -> Framework Search Paths`, and enter the path where GPUPixel is stored.

**Embedding Framework**

Select `Project -> Targets -> General -> Frameworks, Libraries, and Embedded Content`.

Set the GPUPixel library to `Embed & Sign`, and choose `Do Not Embed` for the system libraries.

### Objective-C Invocation

Since GPUPixel is written in C++, to call C++ from Objective-C, you need to change the file extension of the Objective-C source files from `.m` to `.mm`. This allows mixing Objective-C and C++.

All GPUPixel headers are included in `gpupixel.h`. Simply import

```objective-c
#import <gpupixel/gpupixel.h>
```

to access all methods. For detailed method invocations, refer to [here](#).

![](../../image/oc-to-oc++.png)

### Swift Invocation

After Swift 5.9, direct interaction with C++ is possible through a bridging header, eliminating the need for Objective-C as an intermediary. You can refer to [this article](https://cloud.tencent.com/developer/article/2312347) or the official Swift documentation on [Mixing Swift and C++](https://www.swift.org/documentation/cxx-interop/#calling-c-functions).

## Android

Download the latest `gpupixel-release.aar` and place it in your Android project directory, such as the `libs` folder.

Add the dependency in Gradle:

```gradle
dependencies {
    implementation files('libs/gpupixel-release.aar')
}
```

Then sync the project. For reference, see the Android Demo.

Reference article: [Importing AAR Packages in Android](https://juejin.cn/post/7226600031569510459)

## Windows and Linux

Refer to the [demo](https://github.com/pixpark/gpupixel/blob/main/examples/desktop/app.cc) and [CMake configuration](https://github.com/pixpark/gpupixel/blob/main/examples/CMakeLists.txt).