---
# https://vitepress.dev/reference/default-theme-home-page
layout: home
titleTemplate: 基于GPU的实时高性能图像滤镜库

hero:
  name: "GPUPixel"
  text: "基于GPU的实时图像滤镜库"
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
    - theme: alt
      text: Facebetter Beauty SDK
      link: https://facebetter.net

features:
  - icon: ⚡
    title: 高性能
    details: 基于C++11 和 OpenGL/ES 编写，实时高性能，可用于直播和WebRTC等
  - icon: 📱
    title: 跨平台
    details: 全平台支持：iOS, Android, Mac, Windows, Linux 等系统，支持YUV和RGBA多种格式输入输出
  - icon: 🎨
    title: 美颜滤镜
    details: 内置美白、磨皮等滤镜，可随意串联组合实现复杂处理效果
---

<script setup>
import { onMounted } from 'vue'
import HomeSponsors from '../.vitepress/theme/components/HomeSponsors.vue'
import '../.vitepress/theme/style/home-links.css'

onMounted(() => {
  // 修改 Facebetter SDK 按钮，让 SDK 显示为黄色
  const facebetterBtn = document.querySelector('.VPHero .actions a[href="https://facebetter.net"]')
  if (facebetterBtn && facebetterBtn.textContent) {
    const text = facebetterBtn.textContent.trim()
    if (text === 'Facebetter Beauty SDK') {
      facebetterBtn.innerHTML = 'Facebetter Beauty <span>SDK</span>'
    }
  }
})
</script>

<HomeSponsors />

