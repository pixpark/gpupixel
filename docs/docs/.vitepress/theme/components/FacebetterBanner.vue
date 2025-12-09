<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { useData, useRoute } from 'vitepress'

const { isDark } = useData()
const route = useRoute()
const isVisible = ref(false)
const nameStorage = 'FACEBETTER-BANNER-2024'
const target = 'https://facebetter.net'

// 判断当前语言：如果路径以 /zh/ 开头则为中文，否则为英文
const isZh = computed(() => {
  return route.path.startsWith('/zh/')
})

// 根据语言返回不同的文本
const texts = computed(() => {
  if (isZh.value) {
    return {
      tagline: 'Better face beauty effects sdk',
      promo: '年付享2个月免费',
      action: '立即了解'
    }
  } else {
    return {
      tagline: 'Better face beauty effects sdk',
      promo: 'Get 2 months free with annual payment',
      action: 'Learn More'
    }
  }
})

const logoSrc = computed(() => {
  return '/fb-logo-dark.svg'
})

function closeBanner() {
  // Hide the banner
  isVisible.value = false
  // Save action in the local storage
  localStorage.setItem(nameStorage, String(true))
  document.documentElement.classList.remove('has-facebetter-banner')
}

onMounted(() => {
  if (localStorage.getItem(nameStorage) != null) {
    return
  }

  isVisible.value = true
  document.documentElement.classList.add('has-facebetter-banner')
})
</script>

<template>
  <div class="banner" v-if="isVisible">
    <a target="_blank" :href="target" class="banner-link">
      <img :src="logoSrc" alt="Facebetter" class="logo-img" />
      <div class="banner-content">
        <div class="logo-text">
          <span class="tagline">{{ texts.tagline }}</span>
        </div>
        <div class="promo-text">
          <span class="promo-highlight">{{ texts.promo }}</span>
        </div>
      </div>
      <div class="action-btn">
        {{ texts.action }}
        <svg
          xmlns="http://www.w3.org/2000/svg"
          width="15"
          height="15"
          style="margin-left: 8px"
          viewBox="0 0 15 15"
        >
          <path
            fill="currentColor"
            d="M8.293 2.293a1 1 0 0 1 1.414 0l4.5 4.5a1 1 0 0 1 0 1.414l-4.5 4.5a1 1 0 0 1-1.414-1.414L11 8.5H1.5a1 1 0 0 1 0-2H11L8.293 3.707a1 1 0 0 1 0-1.414"
          />
        </svg>
      </div>
    </a>
    <div class="close-btn" @click.stop.prevent="closeBanner">
      <span class="close">&times;</span>
    </div>
  </div>
</template>

<style>
html.has-facebetter-banner {
  --vp-layout-top-height: 56px;
}
</style>

<style scoped>
.banner {
  position: fixed;
  box-sizing: border-box;
  top: 0;
  left: 0;
  right: 0;
  z-index: 50;
  height: var(--vp-layout-top-height);
  font-weight: 600;
  color: #fff;
  background: linear-gradient(135deg, #3a3a4a 0%, #2d2d3d 50%, #3a3a4a 100%);
  border-bottom: 1px solid rgba(251, 191, 36, 0.2);
  display: flex;
  flex-direction: row;
  align-items: center;
  padding: 0;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.banner-link {
  display: flex;
  align-items: center;
  justify-content: center;
  text-decoration: none;
  color: inherit;
  width: 100%;
  height: 100%;
  padding: 0;
  gap: 24px;
  max-width: 1200px;
  margin: 0 auto;
}

.logo-img {
  height: 26px;
  width: auto;
  flex-shrink: 0;
  opacity: 0.95;
  transition: opacity 0.3s ease;
}

.banner-link:hover .logo-img {
  opacity: 1;
}

.banner-content {
  display: flex;
  flex-direction: row;
  align-items: center;
  gap: 20px;
  flex-shrink: 0;
}

.logo-text {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 15px;
  white-space: nowrap;
}

.tagline {
  color: #fbbf24;
  font-weight: 500;
  letter-spacing: 0.3px;
}

.promo-text {
  font-size: 13px;
  color: #e0e0e0;
  white-space: nowrap;
}

.promo-highlight {
  color: #fbbf24;
  font-weight: 600;
}


.action-btn {
  display: flex;
  align-items: center;
  background: linear-gradient(135deg, #fbbf24 0%, #f9a825 100%);
  color: #1a1a1a;
  padding: 5px 16px;
  font-size: 13px;
  border-radius: 6px;
  font-weight: 600;
  white-space: nowrap;
  transition: all 0.3s ease;
  flex-shrink: 0;
  box-shadow: 0 2px 4px rgba(251, 191, 36, 0.3);
}

.banner-link:hover .action-btn {
  background: linear-gradient(135deg, #f9a825 0%, #fbbf24 100%);
  box-shadow: 0 3px 6px rgba(251, 191, 36, 0.4);
  transform: translateY(-1px);
}

.close {
  font-size: 24px;
  line-height: 24px;
  height: 24px;
  color: #999;
  transition: color 0.3s ease;
}

.banner .close-btn {
  top: 50%;
  transform: translateY(-50%);
  height: 24px;
  right: 16px;
  z-index: 99;
  position: absolute;
  cursor: pointer;
}

.banner .close-btn:hover .close {
  color: #fff;
}

@media (max-width: 960px) {
  .banner-link {
    gap: 16px;
    padding: 0 16px;
  }

  .banner-content {
    gap: 12px;
  }

  .logo-img {
    height: 22px;
  }

  .logo-text {
    font-size: 14px;
    gap: 6px;
  }

  .promo-text {
    font-size: 12px;
  }

  .action-btn {
    padding: 4px 12px;
    font-size: 12px;
  }
}

@media (max-width: 768px) {
  .banner {
    height: auto;
    min-height: 56px;
  }

  html.has-facebetter-banner {
    --vp-layout-top-height: auto;
  }

  .banner-link {
    flex-wrap: wrap;
    padding: 8px 40px 8px 40px;
    gap: 8px;
  }

  .banner-content {
    flex-direction: column;
    align-items: flex-start;
    gap: 4px;
    flex: 1;
    min-width: 0;
  }

  .logo-img {
    height: 18px;
  }

  .logo-text {
    font-size: 13px;
    flex-wrap: wrap;
  }

  .promo-text {
    font-size: 11px;
  }

  .action-btn {
    padding: 3px 10px;
    font-size: 11px;
    width: 100%;
    justify-content: center;
  }

  .banner .close-btn {
    right: 12px;
    top: 12px;
    transform: none;
  }
}
</style>

