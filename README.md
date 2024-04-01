<h1 align="center">
  <a href="https://github.com/pixpark/gpupixel"><img src="./docs/image/describe.png"></a>
</h1>

<p align="center">
  <a href="./README.md">English</a> |
  <a href="./README_CN.md">简体中文</a>
</p>

<p align="center">
   <a href="https://github.com/pixpark/gpupixel/stargazers"><img alt="GPUPixel Stars" src="https://img.shields.io/github/stars/pixpark/gpupixel?style=social"/></a>
    <a href="https://github.com/pixpark/gpupixel/releases/latest"><img alt="GPUPixel Release" src="https://img.shields.io/github/v/release/pixpark/gpupixel"/></a>
    <a href="#"><img alt="GPUPixel Stars" src="https://img.shields.io/badge/Platform-iOS_%7C_Android_%7C_Mac_%7C_Win_%7C_Linux-red"/></a>
     <a href="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml"><img src="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml/badge.svg"></a>
    <a href="https://github.com/pixpark/gpupixel/blob/main/LICENSE"><img alt="GPUPixel Stars" src="https://img.shields.io/github/license/pixpark/gpupixel"/></a>
</p>

---



> Welcome to join us to make GPUPixel better by participating [discussions](https://github.com/pixpark/gpupixel/discussions), opening [issues](https://github.com/pixpark/gpupixel/issues/new/choose), submitting [PRs](https://github.com/pixpark/gpupixel/pulls).

## Introduction

⛰️ GPUPixel is a real-time, high-performance image and video filter library, extremely easy to compile and integrate with  small library size.

🔑 GPUPixel  is written in C++11 and is based on OpenGL/ES, incorporating a built-in beauty face filter that achieves commercial-grade beauty effects.

🔌 GPUPixel supports platforms including `iOS`, `Android`, `Mac`, `Win` and `Linux`. It can theoretically be ported to any platform that supports OpenGL/ES.

## Effects Preview

👉 **Video: <a href="https://youtu.be/9BY1Qx1NEPs" target="_blank">YouTube</a> | <a href="https://www.bilibili.com/video/BV1xQ4y1L7Fh/?share_source=copy_web&vd_source=46adcb1014fa989cfcbb4cc1e866831e" target="_blank">BiliBili</a>**


|                   **Origin**                    |                    **Smooth**                     |                    **White**                     |                   **ThinFace**                    |
| :---------------------------------------------: | :-----------------------------------------------: | :----------------------------------------------: | :-----------------------------------------------: |
| <img src="./docs/image/origin.gif" width="180px"> |  <img src="./docs/image/smooth.gif" width="180px">  |  <img src="./docs/image/white.gif" width="180px">  | <img src="./docs/image/thinface.gif" width="180px"> |
|                   **BigEye**                    |                   **Lipstick**                    |                   **Blusher**                    |                    **ON-OFF**                     |
| <img src="./docs/image/bigeye.gif" width="180px"> | <img src="./docs/image/lipstick.gif" width="180px"> | <img src="./docs/image/blusher.gif" width="180px"> |  <img src="./docs/image/on-off.gif" width="180px">  |

## Architecture
![](./docs/image/arch-en.jpg)

## Features

This table compares the features supported by **GPUPixel** and **GPUImage** and **Android-GPUImage**:

✅: Supported | ❌: Not supported | ✏️: Planning

|                       | [GPUPixel](https://github.com/pixpark/gpupixel) | [GPUImage](https://github.com/BradLarson/GPUImage) | [Android-GPUImage](https://github.com/cats-oss/android-gpuimage) |
| :-------------------- | :---------------------------------------------: | :------------------------------------------------: | :----------------------------------------------------------: |
| 📷 **Filters:**        |                        ✅                        |                         ❌                          |                              ❌                               |
| Skin Smoothing Filter |                        ✅                        |                         ❌                          |                              ❌                               |
| Skin Whitening Filter |                        ✅                        |                         ❌                          |                              ❌                               |
| Face Slimming Filter  |                        ✅                        |                         ❌                          |                              ❌                               |
| Big Eyes Filter       |                        ✅                        |                         ❌                          |                              ❌                               |
| Lipstick Filter       |                        ✅                        |                         ❌                          |                              ❌                               |
| Blush Filter          |                        ✅                        |                         ❌                          |                              ❌                               |
| More Build in Filter  |                        ✅                        |                         ✅                          |                              ✅                               |
| 🎬 **Input Formats:**  |                                                 |                                                    |                                                              |
| YUV420P(I420)         |                        ✅                        |                         ❌                          |                              ❌                               |
| RGBA                  |                        ✅                        |                         ✅                          |                              ✅                               |
| JPEG                  |                        ✅                        |                         ✅                          |                              ✅                               |
| PNG                   |                        ✅                        |                         ✅                          |                              ✅                               |
| NV21(for Android)     |                        ✏️                        |                         ❌                          |                              ❌                               |
| 🎥 **Output Formats:** |                                                 |                                                    |                                                              |
| RGBA                  |                        ✅                        |                         ✅                          |                              ✅                               |
| YUV420P(I420)         |                        ✏️                        |                         ❌                          |                              ❌                               |
| 💻 **Platform:**       |                                                 |                                                    |                                                              |
| iOS                   |                        ✅                        |                         ✅                          |                              ❌                               |
| Mac                   |                        ✅                        |                         ✅                          |                              ❌                               |
| Android               |                        ✅                        |                         ❌                          |                              ✅                               |
| Win                   |                        ✅                        |                         ❌                          |                              ❌                               |
| Linux                 |                        ✅                        |                         ❌                          |                              ❌                               |


##  Performance
### iPhone
|       -        | iPhone 6P | iPhone 8 | iPhone X | iPhone 11 | iPhone 14 pro |
| :------------: | :-------: | :------: | :------: | :-------: | :-----------: |
|    **CPU**     |    5%     |    5%    |    3%    |    3%     |      3%       |
| **Time Taken** |   10ms    |   4ms    |   3ms    |    3ms    |      3ms      |
### Android
|     -      | Xiaomi 10 | Huawei Mate30 | Vivo  | SAMSUNG | Google Pixel |
| :--------: | :-------: | :-----------: | :---: | :-----: | :----------: |
|    CPU     |    3%     |      5%       |   -   |    -    |      -       |
| Time Taken |    6ms    |      5ms      |   -   |    -    |      -       |


## Lib Size

|       | iOS(.framework) | MacOS(.framework) | Android(.aar) |
| :---: | :-------------: | :---------------: | :-----------: |
| Size  |     2.4 MB      |      2.6 MB       |    2.1 MB     |


## Before You Start
⭐️ Star us on GitHub, and be instantly notified for new releases!

![](./docs/image/give-star.gif)



## Getting Started

### How to Build
Compile using CMake frome v1.1.0. Please search for instructions on how to install and configure CMake. 

The generated libraries and app will be located in the "output" directory of the project's root folder.

 
### iOS 

```shell
# Generate project
## for iOS arm64
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=toolchain/ios.toolchain.cmake -DPLATFORM=OS64
 
# Build
cmake --build build --config Debug #or Release
```
### Mac

```shell
# Generate project
## for Mac Apple Silicon
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=toolchain/ios.toolchain.cmake -DPLATFORM=MAC_ARM64
## for Mac Intel
cmake -G Xcode -B build -S src -DCMAKE_TOOLCHAIN_FILE=toolchain/ios.toolchain.cmake -DPLATFORM=MAC

# Build
cmake --build build --config Debug #or Release
```

### Android
Open the directory `src/android/java` in Android Studio.

### Windows
You need install and config Cmake and MinGW64 by yourself.
```shell
# Generate project
cmake -G "MinGW Makefiles" -B build -S src

# Build
cmake --build build -DCMAKE_BUILD_TYPE=Debug #or Release
```
### Linux (Test on ubuntu)

```shell
# install cmake 
sudo apt-get install cmake pkg-config
# install dependent lib
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev

# Generate project
cmake -B build -S src

# Build
cmake --build build -DCMAKE_BUILD_TYPE=Debug #or Release 
```

## App demo
 
### iOS and Mac
Refer to `examples/ios` and `examples/mac`
### Android
Refer to `src/android/java`
### Window and Linux

Refer to `examples/desktop` . The compilation method is the same as compiling the library.

```shell
# Generate project
cmake -G "MinGW Makefiles" -B examples/build -S examples

# Build
cmake --build build -DCMAKE_BUILD_TYPE=Debug #or Release 
```
#### App Usage 
<kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> <kbd>F</kbd> <kbd>G</kbd> <kbd>H</kbd> - Increase smooth, white, thin face, big eye, lipstick, blusher level

<kbd>Z</kbd> <kbd>X</kbd> <kbd>C</kbd> <kbd>V</kbd> <kbd>B</kbd> <kbd>N</kbd> - Decrease smooth, white, thin face, big eye, lipstick, blusher level 

## How to Use

**Declear filters**

```c++
// video data input
std::shared_ptr<SourceRawDataInput> source_raw_input_;
// beauty filter
std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
// video data output 
std::shared_ptr<TargetRawDataOutput> target_raw_output_;
```
**Create and link filters**

```c++
 gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Create filter
    source_raw_input_ = SourceRawDataInput::create();
    target_raw_output_ = TargetRawDataOutput::create();
    // Face Beauty Filter
    beauty_face_filter_ = BeautyFaceFilter::create();
    
    // Add filter
    source_raw_input_->addTarget(beauty_face_filter_)
                     ->addTarget(target_raw_output_);
 }
```

**Input YUV420P or RGBA**

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

## Contributing

Welcome to join us to make GPUPixel better by participating [discussions](https://github.com/pixpark/gpupixel/discussions), opening [issues](https://github.com/pixpark/gpupixel/issues/new/choose), submitting [PRs](https://github.com/pixpark/gpupixel/pulls).

At the same time, please consider supporting GPUPixel by sharing it on social media and at events and conferences.

## Contributors

<a href="https://github.com/pixpark/gpupixel/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=pixpark/gpupixel" />
</a>

Made with [contrib.rocks](https://contrib.rocks).
  
## Acknowledgement
### Reference Project
1. [GPUImage](https://github.com/BradLarson/GPUImage) 
2. [CainCamera](https://github.com/CainKernel/CainCamera)
3. [AwemeLike](https://github.com/ZZZZou/AwemeLike)
4. [VNN](https://github.com/joyycom/VNN)

## License
This repository is available under the [MIT License](https://github.com/pixpark/gpupixel?tab=MIT-1-ov-file#readme).
