---
layout: home
titleTemplate: Real-time high performance image filter engine based on GPU
hero:
  name: "GPUPixel"
  text: "Real-time image filter engine based on GPU"
  tagline: 'high performance and cross-platform'
  image:
      src: /cover.png
      alt: cover
  actions:
    - theme: brand
      text: Get Started
      link: /guide/intro
    - theme: alt
      text: GitHub
      link: https://github.com/pixpark/gpupixel
    - theme: alt
      text: Facebetter Beauty SDK
      link: https://facebetter.net

features:
  - icon: ⚡
    title: High Performance
    details: Written in C++11 and based on OpenGL/ES, real-time high performance, suitable for live streaming and WebRTC
  - icon: 📱
    title: Cross-Platform
    details: Full platform support -> iOS, Android, Mac, Windows, Linux, etc. Supports multiple formats like YUV and RGBA for input and output
  - icon: 🎨
    title: Beauty Filters
    details: Built-in whitening and smoothing filters. Filters can be chained together to achieve complex processing effects
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

