<script setup lang="ts">
import { VPDocAsideSponsors } from 'vitepress/theme'

const data = [
  {
    items: [{ img: 'https://cdn.jsdelivr.net/gh/maomao1996/picture/sponsor/wechat-color.jpg' }]
  },
  {
    items: [{ img: 'https://cdn.jsdelivr.net/gh/maomao1996/picture/sponsor/alipay-color.jpg' }]
  }
]
</script>

<template>
  <VPDocAsideSponsors :data="data" />
</template>

<style>
.vp-sponsor-grid.medium .vp-sponsor-grid-image {
  max-height: 90px;
}
</style>
