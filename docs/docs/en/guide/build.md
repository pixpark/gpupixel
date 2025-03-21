---

title: Build
editLink: true
description: This article will introduce the compilation methods for the GPUPixel library on various system platforms.
---


# Build

This chapter will introduce the compilation methods for the GPUPixel library on various system platforms.

::: tip
Starting from version [v1.1.0](https://github.com/pixpark/gpupixel/releases/tag/v1.1.1), the source code is compiled using CMake. The latest precompiled libraries for various platforms can be found [here](https://github.com/pixpark/gpupixel/releases/latest).
:::

## iOS
Execute the following commands at the root directory of the project.

**Generate Project**
::: code-group
```bash [Arm64]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=OS64
```
:::

**Compile**
::: code-group

```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::

**Output**

The compilation output is located in the `output` path under the root directory of the project, containing the following:
```bash
output
├── include   # Header files
├── library   # Library files
├── models    # Models files
└── res # Resource files
```
For iOS, you only need to use the `.framework` library under `library`, which already contains header files and resource files.

## MacOS
Execute the following commands at the root directory of the project.

**Generate Project**
::: code-group
```bash [Apple Silicon]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 
```
```bash [Intel]
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=../toolchain/ios.toolchain.cmake -DPLATFORM=MAC -DCMAKE_BUILD_TYPE=Release
```
:::

**Compile**
::: code-group

```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::

**Output**

The compilation output is located in the `output` path under the root directory of the project, containing the following:
```bash
output
├── include   # Header files
├── library   # Library files
├── models    # Models files
└── res # Resource files
```
For macOS, you only need to use the `.framework` library under `library`, which already contains header files and resource files.

## Android

Open the directory `src/android/java` with Android Studio, which will automatically start downloading dependencies such as Gradle.

**Project Structure**
 
**Gradle Command Compile**

If the Gradle command environment has been configured, you can also use `gradlew` to compile.

```bash
./gradlew :gpupixel:assembleRelease
```

The output is located at: `src/android/java/gpupixel/build/outputs/aar`

## Windows

Windows compilation requires the following environment:

**Environment Requirements**
- Microsoft Visual Studio >= 2017
- cmake >= 3.10

**Build Environment Setup**

It is recommended to use Windows Terminal and select the corresponding VS environment tab for compilation.
![Windows Terminal](../../image/win-terminal.png)
If not using Windows Terminal, you can configure as follows:

- **64-bit compilation**: Find and click `vcvars64.bat` (x64 Native Tools Command Prompt for VS 2017) in settings to open the VS virtual environment for compiling x64 architecture programs

::: warning
Only supports generating x86_64 Release version, x86_32 is not supported currently
:::

**Generate Project**

```bash
cmake -G "NMake Makefiles" -B build -S src -DCMAKE_BUILD_TYPE=Release
```

**Compile**
```bash
cd build 
nmake
```

**Output**

The compilation output is located in the `output` path under the root directory of the project, containing the following:
```bash
output
├── include   # Header files
├── library   # Library files
├── models    # Models files
└── res # Resource files
```

## Linux (Debian or Ubuntu)

**Environment Configuration**

```bash
# Install cmake
sudo apt-get install cmake pkg-config
# Install dependent libraries
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev
```

**Generate Project**
```bash [Release]
cmake -B build -S src -DCMAKE_BUILD_TYPE=Release
```

```bash [Debug]
cmake -B build -S src -DCMAKE_BUILD_TYPE=Debug
```
:::

**Compile**
::: code-group
```bash [Release]
cmake --build build --config Release
```

```bash [Debug]
cmake --build build --config Debug
```
:::

**Output**

The compilation output is located in the `output` path under the root directory of the project, containing the following:
```bash
output
├── include   # Header files
├── library   # Library files
├── models    # Models files
└── res # Resource files
```