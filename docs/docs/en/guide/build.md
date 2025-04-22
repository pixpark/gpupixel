---

title: Build
editLink: true
description: This article will introduce the compilation methods for the GPUPixel library on various system platforms.
---


# Build

This chapter will introduce the compilation methods for the GPUPixel library on various system platforms.

::: tip
Starting from [v1.3.0-beta](https://github.com/pixpark/gpupixel/releases/tag/v1.3.0-beta) version, the Vnn-related dependency libraries have been removed. The latest compiled libraries for various platforms can be found [here](https://github.com/pixpark/gpupixel/releases/latest).
:::

## iOS
Execute the script file at the root directory of the project:

```bash
./script/build_ios.sh
```

**Output**

The compilation output is located in the `output` path under the root directory of the project.

## Mac
Execute the script file at the root directory of the project:

```bash
./script/build_macos.sh
```

**Output**

The compilation output is located in the `output` path under the root directory of the project.

## Android
Execute the script file at the root directory of the project:

```bash
./script/build_android.sh
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

**Compile**

After configuring the environment, execute the batch file at the root directory of the project:

```bash
.\script\build_windows.bat
```

**Output**

The compilation output is located in the `output` path under the root directory of the project.

## Linux (Debian or Ubuntu)

**Environment Configuration**

You can use the environment configuration script to install dependencies:

```bash
./script/setup_env_linux.sh
```

**Compile**

Execute the script file at the root directory of the project:

```bash
./script/build_linux.sh
```

**Output**

The compilation output is located in the `output` path under the root directory of the project.

## WebAssembly (WASM)

WebAssembly compilation requires the following environment:

**Environment Requirements**
- Emscripten SDK >= 3.0
- cmake >= 3.10

**Build Environment Setup**

You need to install Emscripten SDK first. There are two ways to install:

- **Option 1**: Install and activate [emsdk](https://emscripten.org/docs/getting_started/downloads.html)
- **Option 2**: Install via Homebrew (macOS):
  ```bash
  brew install emscripten
  ```

**Compile**

Execute the script file at the root directory of the project:

```bash
./script/build_wasm.sh
```

**Output**

The compilation output is located in the `output` path under the root directory of the project.

::: warning
The WASM version does not support facial keypoint detection. Filters that rely on face detection, such as face-slimming and big eyes, are unavailable.
:::