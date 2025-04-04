<h1 align="center">
  <a href="https://github.com/pixpark/gpupixel"><img src="./docs/image/cover.png"></a>
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
     <a href="https://github.com/pixpark/gpupixel/actions/workflows/release.yml"><img src="https://github.com/pixpark/gpupixel/actions/workflows/release.yml/badge.svg"></a>
    <a href="https://github.com/pixpark/gpupixel/blob/main/LICENSE"><img alt="GPUPixel Stars" src="https://img.shields.io/github/license/pixpark/gpupixel"/></a>
</p>

<p align="center">
<a href="https://discord.gg/q2MjmqK4" target="_blank"><img alt="GPUPixel Discord" src="https://img.shields.io/badge/Chat-Discord-blue?logo=discord&logoColor=white&labelColor=grey&color=blue"/></a>
<a href="https://gpupixel.pixpark.net/about/contact#qq-group" target="_blank"><img alt="QQ Group" src="https://img.shields.io/badge/-QQ群-gray?logo=qq&logoColor=white&labelColor=gray&color=blue&style=flat"/></a>
<a href="https://gpupixel.pixpark.net/about/contact#wechat-official-account" target="_blank"><img alt="GPUPixel Wechat" src="https://img.shields.io/badge/-公众号-gray?logo=wechat&logoColor=white&labelColor=gray&color=07C160&style=flat"/></a>
<a href="https://github.com/pixpark/gpupixel#Sponsorship" target="_blank"><img alt="Sponsor" src="https://img.shields.io/badge/-Sponsor-gray?logo=githubsponsors&logoColor=white&labelColor=grey&color=FE6AB2&style=flat"/></a>
</p>

<p align="center">
<a href="https://trendshift.io/repositories/7103" target="_blank"><img src="https://trendshift.io/api/badge/repositories/7103" alt="pixpark%2Fgpupixel | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>
</p>

---

> 🌟 欢迎加入我们，通过参与[讨论](https://github.com/pixpark/gpupixel/discussions)、创建[问题](https://github.com/pixpark/gpupixel/issues/new/choose)、提交[PRs](https://github.com/pixpark/gpupixel/pulls)，让GPUPixel变得更好。

> 📢 注意：从v1.3.0-beta版本开始，VNN人脸检测库已被Mars-Face替代

## 简介

🚀 一个高性能、跨平台的图像和视频滤镜库，体积小巧。

💻 使用C++11和OpenGL/ES构建，提供美颜滤镜。

🌐 支持iOS、Android、Mac、Windows和Linux，兼容所有OpenGL/ES平台。

## 效果预览
🎬 **视频: <a href="https://youtu.be/9BY1Qx1NEPs" target="_blank">YouTube</a>**

|              **原图**              |                **磨皮**                |               **美白**               |                 **瘦脸**               |
| :--------------------------------: | :------------------------------------: | :----------------------------------: | :------------------------------------: |
| ![origin](./docs/image/origin.gif) |   ![smooth](./docs/image/smooth.gif)   |   ![white](./docs/image/white.gif)   | ![thinface](./docs/image/thinface.gif) |
|              **大眼**              |                **口红**                |               **腮红**               |                 **开关**               |
| ![bigeye](./docs/image/bigeye.gif) | ![lipstick](./docs/image/lipstick.gif) | ![blusher](./docs/image/blusher.gif) |   ![on-off](./docs/image/on-off.gif)   |

## 架构流程
![](./docs/image/arch.jpg)

## 特性对比

✨ **GPUPixel**、**GPUImage**和**Android-GPUImage**功能对比：

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

## ⚡ 性能
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

## 开始之前
⭐ 在GitHub上给我们点个星，获取新版本发布通知！

![](./docs/image/give-star.gif)

## 快速开始

🔍 查看文档：[简介](https://gpupixel.pixpark.net/zh/guide/build) | [编译](https://gpupixel.pixpark.net/zh/guide/build) | [示例](https://gpupixel.pixpark.net/zh/guide/demo) | [集成](https://gpupixel.pixpark.net/zh/guide/integrated)

## 参与贡献

🤝 通过参与[讨论](https://github.com/pixpark/gpupixel/discussions)、创建[问题](https://github.com/pixpark/gpupixel/issues/new/choose)或提交[PR](https://github.com/pixpark/gpupixel/pulls)来改进GPUPixel。查看我们的[贡献指南](docs/docs/zh/guide/contributing.md)了解如何开始。

欢迎在社交媒体和活动中分享GPUPixel。

## 贡献者
 [![](https://opencollective.com/gpupixel/contributors.svg?width=890&button=false)](https://github.com/pixpark/gpupixel/graphs/contributors)

## 赞助
💖 支持本项目：

| [☕ 在 Ko-fi 上支持](docs/docs/zh/sponsor.md#ko-fi) | [💝 在 Open Collective 上支持](docs/docs/zh/sponsor.md#open-collective) | [💰 微信赞助](docs/docs/zh/sponsor.md#微信赞赏) |
|:---:|:---:|:---:|

## 赞助者
🙏 感谢这些贡献者的慷慨支持：

<a href="https://github.com/leavenotrace">
  <img src="https://github.com/leavenotrace.png" style="border-radius:50%; width: 50px; height: 50px;" >
</a>
<a href="https://github.com/weiyu666">
  <img src="https://github.com/weiyu666.png" style="border-radius: 50%; width: 50px; height: 50px;" >
</a>

## 联系 & 支持
- 📚 [文档](https://gpupixel.pixpark.net/)：文档中心
- 🐛 [提问](https://github.com/pixpark/gpupixel/issues/new/choose)：问题报告和功能请求
- 📧 [邮件](mailto:jaaronkot@gmail.com?subject=[GitHub]Questions%20About%20GPUPixel)：联系我们
- 📞 [联系](docs/docs/zh/about/contact.md)：更多联系方式

## 感谢
### 🔗 参考项目
1. [GPUImage](https://github.com/BradLarson/GPUImage) 
2. [CainCamera](https://github.com/CainKernel/CainCamera)
3. [AwemeLike](https://github.com/ZZZZou/AwemeLike)
4. [VNN](https://github.com/joyycom/VNN)

## License
📄 此开源项目遵从[Apache-2.0 License](https://github.com/pixpark/gpupixel?tab=Apache-2.0-1-ov-file)。
