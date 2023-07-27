---
# https://vitepress.dev/reference/default-theme-home-page
layout: home
layoutClass: 'm-home-layout'

hero:
  name: "GPUPixel"
  text: "高性能跨平台图像处理SDK"
  tagline: ''
  image:
    src: /book.svg
    alt: GPUPixel
  actions:
    - theme: brand
      text: 参考文档
      link: /docs
    - theme: alt
      text: Github
      link: https://github.com/pixpark/gpupixel
    - theme: alt
      text: Demo
      link: https://github.com/pixpark/gpupixel

features:
  - icon: 🧚‍♀️
    title: 美颜滤镜
    details: 内置美颜滤镜，包括磨皮, 美白, 瘦脸, 大眼, 口红, 腮红等滤镜
    link: http://localhost:5174/gpupixel/guide/introduction.html
    linkText: 现在开始
  - icon: 🏃‍♂️
    title: 多输入输出
    details: '输入支持: YUV420P, RGBA, JPEG, PNG; </br> 输出: YUV420, RGBA'
    link: http://localhost:5174/gpupixel/guide/introduction.html
    linkText: 现在开始
  - icon: 🥳
    title: 跨平台
    details: '基于OpenGL/ES,高性能, 跨平台支持: iOS | Android | macOS'
    link: http://localhost:5174/gpupixel/guide/introduction.html
    linkText: 现在开始
---

<style>
.m-home-layout .image-src:hover {
  transform: translate(-50%, -50%) rotate(666turn);
  transition: transform 59s 1s cubic-bezier(0.3, 0, 0.8, 1);
}

.m-home-layout .details small {
  opacity: 0.8;
}

.m-home-layout .item:last-child .details {
  display: flex;
  justify-content: flex-end;
  align-items: end;
}
</style>