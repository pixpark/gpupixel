<template>
    <Transition name="fade">
      <div v-show="showBackTop" class="vitepress-backTop-main" title="返回顶部" @click="scrollToTop()">
        <svg t="1720595052079" class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg" p-id="4279" width="200" height="200">
          <path d="M752.736 431.063C757.159 140.575 520.41 8.97 504.518 0.41V0l-0.45 0.205-0.41-0.205v0.41c-15.934 8.56-252.723 140.165-248.259 430.653-48.21 31.457-98.713 87.368-90.685 184.074 8.028 96.666 101.007 160.768 136.601 157.287 35.595-3.482 25.232-30.31 25.232-30.31l12.206-50.095s52.47 80.569 69.304 80.528c15.114-1.23 87-0.123 95.6 0h0.82c8.602-0.123 80.486-1.23 95.6 0 16.794 0 69.305-80.528 69.305-80.528l12.165 50.094s-10.322 26.83 25.272 30.31c35.595 3.482 128.574-60.62 136.602-157.286 8.028-96.665-42.475-152.617-90.685-184.074z m-248.669-4.26c-6.758-0.123-94.781-3.359-102.891-107.192 2.95-98.714 95.97-107.438 102.891-107.93 6.964 0.492 99.943 9.216 102.892 107.93-8.11 103.833-96.174 107.07-102.892 107.192z m-52.019 500.531c0 11.838-9.42 21.382-21.012 21.382a21.217 21.217 0 0 1-21.054-21.34V821.74c0-11.797 9.421-21.382 21.054-21.382 11.591 0 21.012 9.585 21.012 21.382v105.635z m77.333 57.222a21.504 21.504 0 0 1-21.34 21.626 21.504 21.504 0 0 1-21.34-21.626V827.474c0-11.96 9.543-21.668 21.299-21.668 11.796 0 21.38 9.708 21.38 21.668v157.082z m71.147-82.043c0 11.796-9.42 21.34-21.053 21.34a21.217 21.217 0 0 1-21.013-21.34v-75.367c0-11.755 9.421-21.299 21.013-21.299 11.632 0 21.053 9.544 21.053 21.3v75.366z" fill="#FFF" p-id="4280"></path>
        </svg>
      </div>
    </Transition>
  </template>
  
  <script setup>
  import { onBeforeUnmount, onMounted, ref } from "vue";
  
  // 是否显示返回顶部
  const showBackTop = ref(true);
  
  function scrollToTop() {
    window.scrollTo({
      top: 0,
      behavior: "smooth",
    });
  }
  
  // 节流
  function throttle(fn, delay = 100) {
    let lastTime = 0;
    return function () {
      let nowTime = +new Date();
      if (nowTime - lastTime > delay) {
        fn.apply(this, arguments);
        lastTime = nowTime;
      }
    };
  }
  const onScroll = throttle(
    () => (showBackTop.value = Boolean(window.scrollY > 100))
  );
  
  // 监听滚动事件
  onMounted(() => window.addEventListener("scroll", onScroll));
  
  // 移除监听事件
  onBeforeUnmount(() => window.removeEventListener("scroll", onScroll));
  </script>
  
  <style lang="css" scoped>
  .vitepress-backTop-main {
    z-index: 999;
    position: fixed;
    bottom: 20px;
    right: 20px;
    cursor: pointer;
    width: 50px;
    height: 50px;
    border-radius: 50%;
    background-color: #3eaf7c;
    ;
    padding: 10px;
    box-shadow: 2px 2px 10px 4px rgba(0, 0, 0, 0.15);
  }
  
  .vitepress-backTop-main:hover {
    background-color: #71cda3;
  }
  
  svg {
    width: 100%;
    height: 100%;
  }
  
  /* 旋转动画 */
  @keyframes bounce {
    0% {
      transform: translateY(0) rotateY(0);
    }
  
    50% {
      transform: translateY(-10px) rotateY(180deg);
    }
  
    100% {
      transform: translateY(0) rotateY(360deg);
    }
  }
  
  /* 进入 退出动画 */
  .fade-enter-active,
  .fade-leave-active {
    transition: opacity 0.5s ease;
  }
  
  .fade-enter-from,
  .fade-leave-to {
    opacity: 0;
  }
  </style>