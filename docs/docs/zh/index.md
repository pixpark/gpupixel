---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: "GPUPixel"
  text: "基于GPU的实时AI图像美颜滤镜库"
  tagline: '高性能, 跨平台'
  image:
      src: /cover.png
      alt: cover
  actions:
    - theme: brand
      text: 快速开始
      link: /zh/guide/intro
    - theme: alt
      text: GitHub
      link: https://github.com/pixpark/gpupixel

features:
  - icon: 🎨
    title: 美颜滤镜
    details: 内置美白, 磨皮, 瘦脸, 大眼, 口红, 腮红等滤镜
  - icon: ⚡
    title: 高性能
    details: 基于C++11 和 OpenGL/ES 编写，实时高性能，可用于直播和WebRTC等
  - icon: 📱
    title: 跨平台
    details: 全平台支持：iOS, Android, Mac, Windows, Linux 等系统
  - icon: 📦
    title: 输入输出
    details: 支持YUV和RGBA多种格式输入输出，输入源支持包括相机视频和图像
  - icon: 🔗
    title: 滤镜组合
    details: 滤镜可随意串联组合，实现复杂处理效果
  - icon: 🛠
    title: 自定义滤镜
    details: 架构简洁，逻辑清晰，方便拓展自定义滤镜
---

