<script setup lang="ts">
import HomeSponsorsGroup from './HomeSponsorsGroup.vue'
import sponsors from './sponsors.json'
</script>

<template>
  <div class="sponsors_outer">
    <div>
      <HomeSponsorsGroup
        v-if="sponsors.platinum.length"
        name="Platinum"
        size="96"
      />

      <HomeSponsorsGroup v-if="sponsors.gold.length" name="Gold" size="38" />

      <HomeSponsorsGroup
        v-if="sponsors.silver.length"
        name="Silver"
        size="24"
      />
    </div>
  </div>
</template>

<style scoped>
.sponsors_outer {
  text-align: center;
  padding: 35px 40px 45px;
  background-color: var(--vp-c-bg-accent);
  /* transition when toggling dark mode */
  transition: background-color 300ms ease-in-out, color 300ms ease-in-out;
}
</style>

