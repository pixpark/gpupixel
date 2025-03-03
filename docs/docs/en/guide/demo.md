---
title: Examples
editLink: true
description: 本章将介绍各个系统平台Demo编译和运行方法
---
<Badge type="tip" text="Version: 1.3.0-beta" />

# Examples App

This chapter will introduce the methods for compiling and running demos on various system platforms. The demo code is located in the `examples` directory, structured as follows:

```bash
examples
├── CMakeLists.txt
├── android     # Android demo
├── app.cmake   #
├── desktop     # Desktop demo, including Windows and Linux
├── ios         # iOS demo
└── mac         # MacOS demo
```

::: tip
The source code is compiled using CMake. The latest pre-compiled libraries for each platform can be found [here](https://github.com/pixpark/gpupixel/releases/latest).
:::

## iOS
Ensure that the iOS library has been compiled (see [instructions](build#ios)). Open `gpupixel/examples/ios/GPUPixelApp.xcodeproj` with Xcode.

**Project Structure**

```bash
├── ImageFilterController.h         
├── ImageFilterController.mm        # Image filter controller
├── VideoCapturer.h
├── VideoCapturer.m                 # Camera capture wrapper
├── VideoFilterController.h
├── VideoFilterController.mm        # Camera filter controller
├── ViewController.h
├── ViewController.mm               # Demo entry controller
└── sample_face.png                 # Demonstration image
```

To run, simply connect an iPhone or use the simulator following the standard iOS App compilation and running procedure.

![](../../image/ios-demo-project.png)

**Errors**

If you encounter compilation errors, check if the project's library links are correctly set up, and whether the gpupixel and vnn frameworks exist.

![](../../image/ios-project-link.png)

## MacOS
Ensure that the MacOS library has been compiled (see [instructions](build#mac)), then open `gpupixel/examples/mac/GPUPixelMacApp.xcodeproj` with Xcode.

**Project Structure**

```bash
├── VideoCameraManager.h
├── VideoCameraManager.mm   # Camera capture wrapper
├── ViewController.h
├── ViewController.mm       # Main logic for camera capture and GPUPixel filter invocation
```

Run the app following the standard Mac App compilation and running procedure.

![](../../image/mac-demo-project.png)

**Errors**

If you encounter compilation errors, check if the project's library links are correctly set up, and whether the gpupixel and vnn frameworks exist.

![](../../image/mac-demo-link.png)

## Android

Open the directory `src/android/java` with Android Studio, which will automatically download dependencies such as Gradle.

**Project Structure**

It includes the demo and the gpupixel module. The invocation logic is in `MainActivity`. Choose the app target and connect your phone to run it.

![](../../image/android-demo.png)

## Desktop

**Generating the Project**
::: code-group
```bash [Windows]
# Generate project
cmake -G "NMake Makefiles" -B examples/build -S examples -DCMAKE_BUILD_TYPE=Release
```
```bash [Linux]
# Generate project
cmake -G "Unix Makefiles" -B examples/build -S examples -DCMAKE_BUILD_TYPE=Release
```
:::

**Compiling**
```bash
# Build
cmake --build examples/build --config Release
```

**Keyboard Shortcuts**

After running the demo, use the keyboard to adjust beauty effects, specifically:

`A` `S` `D` `F` `G` `H` - Increase the level of `smoothing`, `whitening`, `slimming`, `enlarging eyes`, `lipstick`, `blush`

`Z` `X` `C` `V` `B` `N` - Decrease the level of `smoothing`, `whitening`, `slimming`, `enlarging eyes`, `lipstick`, `blush`

## Web

## Python
