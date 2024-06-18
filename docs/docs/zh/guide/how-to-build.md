---
outline: deep
---

# 编译
::: tip
从 v1.1.0 版本开始，源码使用CMake编译，请自行搜索如何安装和配置CMake
生成的库和应用程序将位于项目根文件夹下的"output"目录中
:::

## iOS

```shell
cd src
mkdir build
cd build

# 生成工程
## for iOS arm64
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=OS64 ..
 
# 编译
cmake --build . --config Debug
```

## Mac

```shell
cd src
mkdir build
cd build

# 生成工程
## for Mac Apple Silicon
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 ..
## for Mac Intel
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=MAC ..

# 编译
cmake --build . --config Debug
```

## Android

使用Android Studio打开目录 `src/android/java`，安卓的工程

## Windows

你需要自己安装和配置 Cmake 以及 MinGW64.

```shell
cd src
mkdir build
cd build

# 生成工程
cmake -G "MinGW Makefiles" ..

# 编译
mingw32-make
```

## Linux (只测试了ubuntu)

```shell
# 安装cmake
sudo apt-get install cmake pkg-config
# 安装依赖
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev

# start build
cd src
mkdir build
cd build

# 生成工程
cmake ..

# 编译
make
```

---

### 