---
# https://vitepress.dev/reference/default-theme-home-page
layout: home
layoutClass: 'm-home-layout'

hero:
  name: "GPUPixel"
  text: "video and image process libs"
  tagline: Welcome to contribute code and suggestions
  image:
    src: /book.svg
    alt: PixPark
  actions:
    - theme: brand
      text: Docs
      link: /docs/gpupixel/start
    - theme: alt
      text: Github
      link: https://github.com/pixpark/gpupixel

features:
  - icon: 💡
    title: 美颜
    details: Video and image processing library like GPUImage support iOS | macOS | Android
    link: https://github.com/pixpark/gpupixel
    linkText: Source code
  - icon: 💡
    title: 特效
    details: Pano Camera like google camera for Android.
    link: https://github.com/pixpark/sphere-camera
    linkText: Source code
  - icon: 💡
    title: 滤镜
    details: Yet another video player, support iOS | Android | macOS | Win
    link: https://github.com/pixpark/alita-player
    linkText: Source code
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