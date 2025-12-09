<template>
  <h3>{{ name }} Sponsors</h3>

  <p>
    <a
      v-for="sponsor in list"
      :key="sponsor.href"
      :href="sponsor.href"
      :title="sponsor.alt"
      target="_blank"
      rel="sponsored noopener"
      class="sponsor_wrapper"
      :class="
        isDark && sponsor.imgSrcLight === sponsor.imgSrcDark && 'apply-bg'
      "
    >
      <img
        :src="sponsor.imgSrc"
        :class="
          isDark &&
          sponsor.imgSrcLight === sponsor.imgSrcDark &&
          'invert-colors'
        "
        :alt="sponsor.alt"
        :style="{ height: size + 'px' }"
      />
    </a>
  </p>
  
  <p v-if="list.length > 0 && list[0].description" class="sponsor-description">
    {{ list[0].description }}
  </p>
</template>

<script setup lang="ts">
import sponsors from './sponsors.json'
import { computed } from 'vue'
import { useData } from 'vitepress'

const props = withDefaults(
  defineProps<{
    name: 'Gold' | 'Platinum' | 'Silver' | 'Bronze'
    size?: number | string
  }>(),
  {
    size: 140,
  }
)

const { isDark } = useData()

const list = computed(() =>
  sponsors[props.name.toLowerCase()].map((sponsor) => ({
    ...sponsor,
    imgSrc: isDark.value ? sponsor.imgSrcDark : sponsor.imgSrcLight,
  }))
)
</script>

<style scoped>
.sponsor_wrapper {
  padding: 5px;
  margin: 0 3px;

  border-radius: 5px;

  display: inline-block;
  text-decoration: none;
  vertical-align: middle;

  transition: background-color 300ms ease-in-out;
}

p:not(.sponsor-description) {
  margin: 0;
}

h3 {
  font-size: 1.35rem;
  font-weight: 600;
  margin: 0.75em 0;
}

img {
  transition: all 0.3s ease;
  opacity: 1;
}

html:not(.light) img.invert-colors {
  filter: invert(1);
  background-color: transparent;
}

.sponsor_wrapper.apply-bg:hover {
  background-color: var(--c-text);
}

.sponsor-description {
  margin-top: 0.5rem;
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
  font-style: italic;
}
</style>

