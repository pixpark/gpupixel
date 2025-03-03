---
title: Introduction
editLink: true
description: GPUPixel是一个使用C++11编写的高性能图像和视频AI美颜效果库，非常容易编译和集成，并且库文件非常小。它是基于GPU的，并且带有内置的美颜效果滤镜，可以实现商业级别的效果。它支持的平台包括iOS、Mac和Android，理论上可以移植到任何支持OpenGL/ES的平台
---

 
## Version Intro
The 1.3.0-beta version has removed the VNN-related face recognition libraries and replaced them with mars-face and mnn.
 
The static libraries of mars-face and mnn are provided for Linux, Mac, and iOS platforms, while the Android and Windows platforms link to the mars-face dynamic library.