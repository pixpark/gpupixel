<h1 align="center">
  <a href="https://github.com/pixpark/gpupixel"><img src="./docs/image/describe.png"></a>
</h1>

<p align="center">
  <a href="./README.md">English</a> |
  <a href="./README_CN.md">简体中文</a>
</p>

<p align="center">
  <a href="https://gpupixel.pixpark.net/" target="_blank">Doc</a>
  <span> · </span>
  <a href="https://gpupixel.pixpark.net/zh" target="_blank">文档</a>
</p>

<p align="center">
   <a href="https://github.com/pixpark/gpupixel/stargazers"><img alt="GPUPixel Stars" src="https://img.shields.io/github/stars/pixpark/gpupixel?style=social"/></a>
    <a href="https://github.com/pixpark/gpupixel/releases/latest"><img alt="GPUPixel Release" src="https://img.shields.io/github/v/release/pixpark/gpupixel"/></a>
    <a href="#"><img alt="GPUPixel Stars" src="https://img.shields.io/badge/Platform-iOS_%7C_Android_%7C_Mac_%7C_Win_%7C_Linux-red"/></a>
     <a href="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml"><img src="https://github.com/pixpark/gpupixel/actions/workflows/cmake-multi-platform.yml/badge.svg"></a>
    <a href="https://github.com/pixpark/gpupixel/blob/main/LICENSE"><img alt="GPUPixel Stars" src="https://img.shields.io/github/license/pixpark/gpupixel"/></a>
</p>

<p align="center">
<a href="https://discord.gg/q2MjmqK4" target="_blank"><img alt="GPUPixel Discord" src="https://img.shields.io/badge/Chat-Discord-blue?logo=discord&logoColor=white&labelColor=grey&color=blue"/></a>
<a href="https://gpupixel.pixpark.net/about/contact#qq-group" target="_blank"><img alt="QQ Group" src="https://img.shields.io/badge/-QQ群-gray?logo=qq&logoColor=white&labelColor=gray&color=blue&style=flat"/></a>
<a href="https://gpupixel.pixpark.net/about/contact#wechat-official-account" target="_blank"><img alt="GPUPixel Wechat" src="https://img.shields.io/badge/-公众号-gray?logo=wechat&logoColor=white&labelColor=gray&color=07C160&style=flat"/></a>
<a href="https://gpupixel.pixpark.net/about/contact#paid-support" target="_blank"><img alt="GPUPixel 付费支持" src="https://img.shields.io/badge/-付费支持-gray?logo=payhip&logoColor=white&labelColor=grey&color=07C160&style=flat"/></a>
<a href="https://github.com/pixpark/gpupixel#Sponsorship" target="_blank"><img alt="Sponsor" src="https://img.shields.io/badge/-Sponsor-gray?logo=githubsponsors&logoColor=white&labelColor=grey&color=FE6AB2&style=flat"/></a>
</p>

<p align="center">
<a href="https://trendshift.io/repositories/7103" target="_blank"><img src="https://trendshift.io/api/badge/repositories/7103" alt="pixpark%2Fgpupixel | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>
</p>

---

> 🌟 Join us in making GPUPixel better through [discussions](https://github.com/pixpark/gpupixel/discussions), [issues](https://github.com/pixpark/gpupixel/issues/new/choose), and [PRs](https://github.com/pixpark/gpupixel/pulls).

> 📢 Note: VNN face detection library has been replaced with Mars-Face from v1.3.0-beta

## Introduction

🚀 GPUPixel is a real-time, high-performance image and video filter library that's extremely easy to compile and integrate with a small footprint.

💻 GPUPixel is written in C++11 and built on OpenGL/ES, featuring built-in beauty face filters that deliver commercial-grade results.

🌐 GPUPixel supports multiple platforms including `iOS`, `Android`, `Mac`, `Win` and `Linux`, and can be ported to virtually any platform that supports OpenGL/ES.

## Effects Preview

🎬 **Video: <a href="https://youtu.be/9BY1Qx1NEPs" target="_blank">YouTube</a>**


|                   **Origin**                    |                    **Smooth**                     |                    **White**                     |                   **ThinFace**                    |
| :---------------------------------------------: | :-----------------------------------------------: | :----------------------------------------------: | :-----------------------------------------------: |
| <img src="./docs/image/origin.gif" width="180px"> |  <img src="./docs/image/smooth.gif" width="180px">  |  <img src="./docs/image/white.gif" width="180px">  | <img src="./docs/image/thinface.gif" width="180px"> |
|                   **BigEye**                    |                   **Lipstick**                    |                   **Blusher**                    |                    **ON-OFF**                     |
| <img src="./docs/image/bigeye.gif" width="180px"> | <img src="./docs/image/lipstick.gif" width="180px"> | <img src="./docs/image/blusher.gif" width="180px"> |  <img src="./docs/image/on-off.gif" width="180px">  |

## Architecture
![](./docs/image/arch.jpg)

## Features

✨ This table compares the features supported by **GPUPixel**, **GPUImage**, and **Android-GPUImage**:

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


## Performance
⚡ ### iPhone
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

📦 |       | iOS(.framework) | MacOS(.framework) | Android(.aar) |
| :---: | :-------------: | :---------------: | :-----------: |
| Size  |     2.4 MB      |      2.6 MB       |    2.1 MB     |


## Before You Start
⭐ Star us on GitHub to receive instant notifications about new releases!

![](./docs/image/give-star.gif)



## Getting Started

🔍 See the docs: [Introduction](https://gpupixel.pixpark.net/guide/build) | [Build](https://gpupixel.pixpark.net/guide/build) | [Examples](https://gpupixel.pixpark.net/guide/demo) | [Integration](https://gpupixel.pixpark.net/guide/integrated)

## Contributing

🤝 Help make GPUPixel better by joining our [discussions](https://github.com/pixpark/gpupixel/discussions), opening [issues](https://github.com/pixpark/gpupixel/issues/new/choose), or submitting [PRs](https://github.com/pixpark/gpupixel/pulls).

Please also consider supporting GPUPixel by sharing it on social media and at events and conferences.

## Contributors
 [![](https://opencollective.com/gpupixel/contributors.svg?width=890&button=false)](https://github.com/pixpark/gpupixel/graphs/contributors)

## Sponsorship
💖 If you like this project, consider supporting us through the following methods:

| [<img src="./docs/image/support_me_on_kofi_red.png" alt="Ko-fi" style="width: 150px;" />](https://ko-fi.com/jaaron) | [<img src="./docs/image/open-collective-vector-logo.png" alt="open collective" style="width: 150px;" />](https://opencollective.com/gpupixel) | [<img src="./docs/image/wechat-logo.png" alt="wechat" style="width: 150px;" />](https://gpupixel.pixpark.net/sponsor#wechat) |
|:---:|:---:|:---:|

## Donors

🙏 Thank you to the following contributors for their generous support of the project:

<a href="https://github.com/leavenotrace">
  <img src="https://github.com/leavenotrace.png" style="border-radius:50%; width: 50px; height: 50px;" >
</a>
<a href="https://github.com/weiyu666">
  <img src="https://github.com/weiyu666.png" style="border-radius: 50%; width: 50px; height: 50px;" >
</a>

## Contact & Support
- 📚 [Docs](https://gpupixel.pixpark.net/) : Online documentation
- 🐛 [Issues](https://github.com/pixpark/gpupixel/issues/new/choose) : Report bugs or request features
- 📧 [Email](mailto:jaaronkot@gmail.com?subject=[GitHub]Questions%20About%20GPUPixel) : Send us a message
- 📞 [Contact](https://gpupixel.pixpark.net/zh/about/contact) : Get in touch with us

## Acknowledgements
🔗 ### Reference Projects
1. [GPUImage](https://github.com/BradLarson/GPUImage) 
2. [CainCamera](https://github.com/CainKernel/CainCamera)
3. [AwemeLike](https://github.com/ZZZZou/AwemeLike)
4. [VNN](https://github.com/joyycom/VNN)

## License
📄 This repository is available under the [MIT License](https://github.com/pixpark/gpupixel?tab=MIT-1-ov-file#readme).

