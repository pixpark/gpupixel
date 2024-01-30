<h1 align="center">
  <a href="https://github.com/pixpark/gpupixel">GPUPixel</a>
</h1>

<p align="center">

  English | [简体中文](./README_cn.md) 

   <a href="https://github.com/pixpark/gpupixel/stargazers"><img alt="GPUPixel Stars" src="https://img.shields.io/github/stars/pixpark/gpupixel?style=social"/></a>
    <a href="https://github.com/pixpark/gpupixel/releases/latest"><img alt="GPUPixel Release" src="https://img.shields.io/github/v/release/pixpark/gpupixel"/></a>
    <a href="#"><img alt="GPUPixel Stars" src="https://img.shields.io/badge/Platform-iOS_%7C%20macOS%20%7C%20Android-red"/></a>
    <a href="https://github.com/pixpark/gpupixel/blob/main/LICENSE"><img alt="GPUPixel Stars" src="https://img.shields.io/github/license/pixpark/gpupixel"/></a>
</p>

---


> 🎉🎉🎉 GPUPixel has added support for the Windows and Linux from [v1.1.0](https://github.com/pixpark/gpupixel/releases/tag/1.1.0), and now it is fully cross-platform supported: iOS | Android | Mac | Win | Linux 👏🏻👏🏻

[GPUPixel]() @ [PixPark](https://github.com/pixpark) 

## Introduction
⛰️ GPUPixel is a high-performance image and video AI beauty effect library written in C++11, Extremely easy to compile and integrate, with a very small library size.

🔑 It is GPU-based and comes with built-in beauty effects filters that can achieve commercial-grade results. 

🔌 It supports platforms including `iOS`, `Android`, `Mac`, `Win`, `Linux` and it can theoretically be ported to any platform that supports OpenGL/ES.

>💡 The face key points detection currently utilizes the [Face++](https://www.faceplusplus.com.cn/) library, but it will be replaced with either **[VNN](https://github.com/joyycom/VNN)** in the future.

## Effects Preview

👉 **Video: <a href="https://youtu.be/9BY1Qx1NEPs" target="_blank">YouTube</a> | <a href="https://www.bilibili.com/video/BV1xQ4y1L7Fh/?share_source=copy_web&vd_source=46adcb1014fa989cfcbb4cc1e866831e" target="_blank">BiliBili</a>**


|                   **Origin**                    |                    **Smooth**                     |                    **White**                     |                   **ThinFace**                    |
| :---------------------------------------------: | :-----------------------------------------------: | :----------------------------------------------: | :-----------------------------------------------: |
| <img src="./docs/image/origin.gif" width="180"> |  <img src="./docs/image/smooth.gif" width="180">  |  <img src="./docs/image/white.gif" width="180">  | <img src="./docs/image/thinface.gif" width="180"> |
|                   **BigEye**                    |                   **Lipstick**                    |                   **Blusher**                    |                    **ON-OFF**                     |
| <img src="./docs/image/bigeye.gif" width="180"> | <img src="./docs/image/lipstick.gif" width="180"> | <img src="./docs/image/blusher.gif" width="180"> |  <img src="./docs/image/on-off.gif" width="180">  |

## Architecture
![](./docs/image/arch-en.jpg)

## Features Compared

This table compares the features supported by **GPUPixel** and **GPUImage**:

✅: Supported | ❌: Not supported | ✏️: Planning

|                       | [GPUPixel](https://github.com/pixpark/gpupixel) | [GPUImage](https://github.com/BradLarson/GPUImage) |
| :-------------------- | :---------------------------------------------: | :------------------------------------------------: |
| 🍎**Filters:**         |                        ✅                        |                         ❌                          |
| Skin Smoothing Filter |                        ✅                        |                         ❌                          |
| Skin Whitening Filter |                        ✅                        |                         ❌                          |
| Face Slimming Filter  |                        ✅                        |                         ❌                          |
| Big Eyes Filter       |                        ✅                        |                         ❌                          |
| Lipstick Filter       |                        ✅                        |                         ❌                          |
| Blush Filter          |                        ✅                        |                         ❌                          |
| More Build in Filter  |                        ✅                        |                         ✅                          |
| 🍓**Input Formats:**   |                                                 |                                                    |
| YUV420P(I420)         |                        ✅                        |                         ❌                          |
| RGBA                  |                        ✅                        |                         ✅                          |
| JPEG                  |                        ✅                        |                         ✅                          |
| PNG                   |                        ✅                        |                         ✅                          |
| NV21(for Android)     |                        ✏️                        |                         ❌                          |
| 🍉**Output Formats:**  |                                                 |                                                    |
| RGBA                  |                        ✅                        |                         ✅                          |
| YUV420P(I420)         |                        ✏️                        |                         ❌                          |
| 🥑**Platform:**        |                                                 |                                                    |
| iOS                   |                        ✅                        |                         ✅                          |
| Mac                   |                        ✅                        |                         ✅                          |
| Android               |                        ✅                        |                         ❌                          |
| Win                   |                       👉 ✅                       |                         ❌                          |
| Linux                 |                       👉 ✅                       |                         ❌                          |


##  Performance
### iPhone
|       -        | iPhone 6P | iPhone 8 | iPhone X | iPhone 11 | iPhone 14 pro |
| :------------: | :-------: | :------: | :------: | :-------: | :-----------: |
|    **CPU**     |    5%     |    5%    |    3%    |    3%     |      3%       |
| **Time Taken** |   10ms    |   4ms    |   3ms    |    3ms    |      3ms      |
### Android
|     -      | Xiaomi 10 | Huawei Meta30 | Vivo  | SAMSUNG | Google Pixel |
| :--------: | :-------: | :-----------: | :---: | :-----: | :----------: |
|    CPU     |    3%     |      5%       |   -   |    -    |      -       |
| Time Taken |    6ms    |      5ms      |   -   |    -    |      -       |


## Library Size

|       | iOS(.framework) | MacOS(.framework) | Android(.aar) |
| :---: | :-------------: | :---------------: | :-----------: |
| Size  |     2.4 MB      |      2.6 MB       |    2.1 MB     |

## Before You Start
Star us on GitHub, and be instantly notified for new releases!
- Website
- Docs
- Deployment Docs
- FAQ

## Quick Start
### How to Build
Compile using CMake frome v1.1.0. Please search for instructions on how to install and configure CMake.
The generated libraries and applications will be located in the "output" directory of the project's root folder.
### iOS
```shell
cd src
mkdir build
cd build

# Generate project
## for iOS arm64
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=OS64 ..
 
# Build
cmake --build . --config Debug
```
### Mac

```shell
cd src
mkdir build
cd build

# Generate project
## for Mac Apple Silicon
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=MAC_ARM64 ..
## for Mac Intel
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../toolchain/ios.toolchain.cmake -DPLATFORM=MAC ..

# Build
cmake --build . --config Debug
```

### Android
1. Open the directory `src/android/java` in Android Studio 

### Windows
You need install and config Cmake and MinGW64
```shell
cd src
mkdir build
cd build

# Generate project
cmake -G "MinGW Makefiles" ..

# Build
mingw32-make
```
### Linux (only test on ubuntu 22.04 lts)

```shell
# install cmake 
sudo apt-get install cmake pkg-config
# install glfw lib
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev

# start build
cd src
mkdir build
cd build

# Generate project
cmake ..

# Build
make
```
---

### App demo
### iOS
Refer to `examples/ios`
### Mac
Refer to `examples/mac`
### Android
Refer to `examples/android` or `src/android/java`
### Window and Linux
Refer to `examples/desktop`
The compilation method is the same as compiling the library.

## How to Use

**`.h` file**

```c++
// video data input
std::shared_ptr<SourceRawDataInput> source_raw_input_;
// beauty filter
std::shared_ptr<FaceBeautyFilter> face_beauty_filter_;
// video data output 
std::shared_ptr<TargetRawDataOutput> target_raw_output_;
```
**Create Filter, `.cpp` file**

```c++
 gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Create filter
    source_raw_input_ = SourceRawDataInput::create();
    target_raw_output_ = TargetRawDataOutput::create();
    // Face Beauty Filter
    face_beauty_filter_ = FaceBeautyFilter::create();
    
    // Add filter
    source_raw_input_->addTarget(face_beauty_filter_)->addTarget(target_raw_output_);
 }
```

**Input Image Data I420 or RGBA**

```c++
// ...
// YUVI420
 source_raw_input_->uploadBytes(width,
                                height, 
                                bufferY,
                                strideY, 
                                bufferU, 
                                strideU,
                                bufferV, 
                                strideV);
// ...
// bytes: RGBA data
 source_raw_input_->uploadBytes(bytes,
                                width, 
                                height, 
                                stride);
```

**Output Data Callback**

```c++
// I420 callback
target_raw_output_->setI420Callbck([=](const uint8_t *data, 
                                        int width, 
                                        int height, 
                                        int64_t ts) {
    size_t y_size = width * height;
    const uint8_t *uData = data + y_size;
    const uint8_t *vData = data + y_size + y_size / 4;
    // Do something you want
});

// RGBA callback->
target_raw_output_->setPixelsCallbck([=](const uint8_t *data, 
                                        int width, 
                                        int height, 
                                        int64_t ts) {
    size_t rgba_size = width * height*4;
    // Do something you want
});

// Output data callbck
```

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=pixpark/gpupixel&type=Date)](https://star-history.com/#pixpark/gpupixel&Date)

## Contributing
Welcome to contribute code🙏🏻.

At the same time, please consider supporting Dify by sharing it on social media and at events and conferences.


## Acknowledgement
### Reference Project
1. [GPUImage](https://github.com/BradLarson/GPUImage) 
2. [CainCamera](https://github.com/CainKernel/CainCamera)
3. [AwemeLike](https://github.com/ZZZZou/AwemeLike)
4. [VNN](https://github.com/joyycom/VNN)

## License
This repository is available under the [MIT License](https://github.com/pixpark/gpupixel?tab=MIT-1-ov-file#readme).