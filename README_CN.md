<h1 align="center">
  <a href="https://github.com/pixpark/gpupixel"><img src="./docs/image/describe.png"></a>
</h1>

<p align="center">
  <a href="./README.md">English</a> |
  <a href="./README_CN.md">简体中文</a>
</p>

<p align="center">
  <a href="https://gpupixel.pixpark.net/guide/intro">编译</a>
  <span> · </span>
  <a href="https://reactnative.dev/docs/tutorial">示例</a>
  <span> · </span>
  <a href="https://reactnative.dev/showcase">集成</a>
</p>

<p align="center">
   <a href="https://github.com/pixpark/gpupixel/stargazers"><img alt="GPUPixel Stars" src="https://img.shields.io/github/stars/pixpark/gpupixel?style=social"/></a>
    <a href="https://github.com/pixpark/gpupixel/releases/latest"><img alt="GPUPixel Release" src="https://img.shields.io/github/v/release/pixpark/gpupixel"/></a>
    <a href="#"><img alt="GPUPixel Stars" src="https://img.shields.io/badge/Platform-iOS_%7C_Android_%7C_Mac_%7C_Win_%7C_Linux-red"/></a>
     <a href="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml"><img src="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml/badge.svg"></a>
    <a href="https://github.com/pixpark/gpupixel/blob/main/LICENSE"><img alt="GPUPixel Stars" src="https://img.shields.io/github/license/pixpark/gpupixel"/></a>
</p>

<p align="center">
<a href="https://trendshift.io/repositories/7103" target="_blank"><img src="https://trendshift.io/api/badge/repositories/7103" alt="pixpark%2Fgpupixel | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>
</p>

---

> 欢迎加入我们，通过参与[讨论](https://github.com/pixpark/gpupixel/discussions)、创建[问题](https://github.com/pixpark/gpupixel/issues/new/choose)、提交[PRs](https://github.com/pixpark/gpupixel/pulls)，让GPUPixel变得更好。👏

## 简介

⛰️ GPUPixel是一个实时、高性能图像和视频AI美颜效果库，非常容易编译和集成，并且库文件非常小。

🔑 它是使用C++11编写的，基于OpenGL/ES，并且带有内置的美颜效果滤镜，可以实现商业级别的效果。

🔌 它支持的平台包括`iOS`、`Android`、`Mac`、`Win`和`Linux`，理论上可以移植到任何支持OpenGL/ES的平台。

## 效果预览
👉 **视频: <a href="https://youtu.be/9BY1Qx1NEPs" target="_blank">YouTube</a> | <a href="https://www.bilibili.com/video/BV1xQ4y1L7Fh/?share_source=copy_web&vd_source=46adcb1014fa989cfcbb4cc1e866831e" target="_blank">BiliBili</a>**

|              **原图**              |                **磨皮**                |               **美白**               |                 **瘦脸**               |
| :--------------------------------: | :------------------------------------: | :----------------------------------: | :------------------------------------: |
| ![origin](./docs/image/origin.gif) |   ![smooth](./docs/image/smooth.gif)   |   ![white](./docs/image/white.gif)   | ![thinface](./docs/image/thinface.gif) |
|              **大眼**              |                **口红**                |               **腮红**               |                 **开关**               |
| ![bigeye](./docs/image/bigeye.gif) | ![lipstick](./docs/image/lipstick.gif) | ![blusher](./docs/image/blusher.gif) |   ![on-off](./docs/image/on-off.gif)   |

## 架构流程
![](./docs/image/arch-en.svg)

## 特性对比

此表比较了**GPUPixel**、**GPUImage**和**Android GPUImage**支持的功能：

✅: 支持 | ❌: 不支持 | ✏️: 计划中

|                       | [GPUPixel](https://github.com/pixpark/gpupixel) | [GPUImage](https://github.com/BradLarson/GPUImage) | [Android-GPUImage](https://github.com/cats-oss/android-gpuimage) |
| :-------------------- | :---------------------------------------------: | :------------------------------------------------: | :----------------------------------------------------------: |
| 📷 **滤镜：**        |                        ✅                        |                         ❌                          |                              ❌                               |
| 磨皮                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 美白                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 瘦脸                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 大眼                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 口红                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 腮红                  |                        ✅                        |                         ❌                          |                              ❌                               |
| 内建滤镜              |                        ✅                        |                         ✅                          |                              ✅                               |
| 🎬 **输入格式：**    |                                                   |                                                     |                                                                |
| YUV420P(I420)         |                        ✅                        |                         ❌                          |                              ❌                               |
| RGBA                  |                        ✅                        |                         ✅                          |                              ✅                               |
| JPEG                  |                        ✅                        |                         ✅                          |                              ✅                               |
| PNG                   |                        ✅                        |                         ✅                          |                              ✅                               |
| NV21(for Android)     |                        ✏️                        |                         ❌                          |                              ❌                               |
| 🎥 **输出格式：**    |                                                   |                                                     |                                                                |
| RGBA                  |                        ✅                        |                         ✅                          |                              ✅                               |
| YUV420P(I420)         |                        ✏️                        |                         ❌                          |                              ❌                               |
| 💻 **平台系统：**    |                                                   |                                                     |                                                                |
| iOS                   |                        ✅                        |                         ✅                          |                              ❌                               |
| Mac                   |                        ✅                        |                         ✅                          |                              ❌                               |
| Android               |                        ✅                        |                         ❌                          |                              ✅                               |
| Win                   |                        ✅                        |                         ❌                          |                              ❌                               |
| Linux                 |                        ✅                        |                         ❌                          |                              ❌                               |

##  性能
### iPhone
|       -        | iPhone 6P | iPhone 8 | iPhone X | iPhone 11 | iPhone 14 pro |
| :------------: | :-------: | :------: | :------: | :-------: | :-----------: |
|    **CPU**     |    5%     |    5%    |    3%    |    3%     |      3%       |
|    **耗时**    |   10ms    |   4ms    |   3ms    |    3ms    |      3ms      |
### Android
|     -      | Xiaomi 10 | Huawei Mate30 | Vivo  | SAMSUNG | Google Pixel |
| :--------: | :-------: | :-----------: | :---: | :-----: | :----------: |
|    CPU     |    3%     |      5%       |   -   |    -    |      -       |
|    耗时    |    6ms    |      5ms      |   -   |    -    |      -       |

## 库体积

|       | iOS(.framework) | MacOS(.framework) | Android(.aar) |
| :---: | :-------------: | :---------------: | :-----------: |
| Size  |     2.4 MB      |      2.6 MB       |    2.1 MB     |


## 开始之前
⭐️ 老铁给个星星，求点赞！您也可以第一时间收到新版本的通知！

![](./docs/image/give-star.gif)

## 快速开始

查看文档：[简介](https://gpupixel.pixpark.net/zh/guide/build) | [编译](https://gpupixel.pixpark.net/zh/guide/build) | [示例](https://gpupixel.pixpark.net/zh/guide/demo) | [集成](https://gpupixel.pixpark.net/zh/guide/integrated)

## 参与贡献

欢迎加入我们，通过参与[讨论](https://github.com/pixpark/gpupixel/discussions)、创建[问题](https://github.com/pixpark/gpupixel/issues/new/choose)、提交[PRs](https://github.com/pixpark/gpupixel/pulls)，让GPUPixel变得更好。

同时，希望您可以通过在社交媒体、活动、会议上分享GPUPixel来支持本项目。

## 贡献者

[![](https://opencollective.com/gpupixel/contributors.svg?width=890&button=false)](https://github.com/pixpark/gpupixel/graphs/contributors)

## 联系 & 支持
- [文档](https://gpupixel.pixpark.net/)：在线文档。
- [提问](https://github.com/pixpark/gpupixel/issues/new/choose)：👉 Github问题。
- [邮件](mailto:jaaronkot@gmail.com?subject=[GitHub]Questions%20About%20GPUPixel)：👉 向我们发送邮件。
- [微信]()： 
<p align="center">
<img src="./docs/image/wechat.png" alt="wechat" width="60%"/>
</p>

## 感谢
### 参考项目
1. [GPUImage](https://github.com/BradLarson/GPUImage) 
2. [CainCamera](https://github.com/CainKernel/CainCamera)
3. [AwemeLike](https://github.com/ZZZZou/AwemeLike)
4. [VNN](https://github.com/joyycom/VNN)

## 许可证
此开源项目遵从[MIT许可证](https://github.com/pixpark/gpupixel?tab=MIT-1-ov-file#readme)。
