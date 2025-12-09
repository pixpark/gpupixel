<script setup lang="ts">
import { computed } from 'vue'
import { VPDocAsideSponsors } from 'vitepress/theme'
import { useData } from 'vitepress'
import sponsors from './sponsors.json'

// to avoid the never[] type in json
interface Sponsor {
  alt: string
  href: string
  imgSrcDark: string
  imgSrcLight: string
  description?: string
}

const { isDark } = useData()

const asideSponsors = computed(() => {
  return [
    ...(sponsors.platinum.length
      ? [
          {
            items: sponsors.platinum.map((sponsor: Sponsor) => ({
              name: sponsor.alt,
              url: sponsor.href,
              img: isDark.value ? sponsor.imgSrcDark : sponsor.imgSrcLight,
            })),
          },
        ]
      : []),
    {
      size: 'mini',
      items: sponsors.gold.map((sponsor: Sponsor) => ({
        name: sponsor.alt,
        url: sponsor.href,
        img: isDark.value ? sponsor.imgSrcDark : sponsor.imgSrcLight,
      })),
    },
    {
      size: 'xmini',
      items: sponsors.silver.map((sponsor: Sponsor) => ({
        name: sponsor.alt,
        url: sponsor.href,
        img: isDark.value ? sponsor.imgSrcDark : sponsor.imgSrcLight,
      })),
    },
  ]
})

const goldSponsor = computed(() => {
  return sponsors.gold.length > 0 ? sponsors.gold[0] : null
})
</script>

<template>
  <div class="aside-sponsors-wrapper">
    <div class="sponsor-container">
      <VPDocAsideSponsors :data="asideSponsors" />
      <p v-if="goldSponsor && goldSponsor.description" class="sponsor-description">
        {{ goldSponsor.description }}
      </p>
    </div>
  </div>
</template>

<style scoped>
.aside-sponsors-wrapper {
  margin-top: 0 !important;
  margin-bottom: 30px !important;
}

.sponsor-container {
  background-color: var(--vp-c-bg-soft);
  border-radius: 8px;
  padding: 12px;
  margin-top: 0 !important;
}

.VPDocAsideSponsors {
  margin-top: 0 !important;
  margin-bottom: 0 !important;
}

:deep(.vp-sponsor-grid) {
  margin-bottom: 0 !important;
}

:deep(.vp-sponsor-grid.mini) {
  margin-bottom: 0 !important;
}

:deep(.vp-sponsor-grid.mini .vp-sponsor-grid-image) {
  max-width: 158px;
  max-height: 48px;
  filter: none !important;
  opacity: 1 !important;
}

:deep(.vp-sponsor-grid.mini .vp-sponsor-grid-item:hover .vp-sponsor-grid-image) {
  filter: none !important;
  opacity: 1 !important;
}

:deep(.vp-sponsor-grid.xmini .vp-sponsor-grid-image) {
  max-width: 80px;
  max-height: 32px;
  filter: none !important;
  opacity: 1 !important;
}

:deep(.vp-sponsor-grid.xmini .vp-sponsor-grid-item:hover .vp-sponsor-grid-image) {
  filter: none !important;
  opacity: 1 !important;
}

.sponsor-description {
  margin-top: 6px;
  margin-bottom: 0;
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
  font-style: italic;
  text-align: center;
  line-height: 1.4;
  word-break: break-word;
  padding: 0 4px;
}
</style>

