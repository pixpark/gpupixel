---
title: 简介
editLink: true
description: GPUPixel是一个使用C++11编写的高性能图像和视频AI美颜效果库，非常容易编译和集成，并且库文件非常小。它是基于GPU的，并且带有内置的美颜效果滤镜，可以实现商业级别的效果。它支持的平台包括iOS、Mac和Android，理论上可以移植到任何支持OpenGL/ES的平台
---

<Badge type="tip" text="版本: 1.3.0-beta" />

## 版本简介 
1.3.0-beta版本 移除了vnn 相关的人脸识别库，改用mars-face和mnn，其中 linux | mac | ios 平台提供了 mars-face 和 mnn 静态库，android和windows链接的是 mars-face动态库

