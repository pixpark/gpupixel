<template>
  <!-- 仅用于演示流体边框，请勿使用本组件 -->
  <div id="fluid-border">
    <div class="info custom-block"><p class="custom-block-title">INFO</p><p>INFO流体边框</p></div>
    <div class="tip custom-block"><p class="custom-block-title">TIP</p><p>TIP流体边框</p></div>
    <div class="warning custom-block"><p class="custom-block-title">WARNING</p><p>WARNING流体边框</p></div>
    <div class="danger custom-block"><p class="custom-block-title">DANGER</p><p>DANGER流体边框</p></div>
    <div class="note custom-block github-alert"><p class="custom-block-title">NOTE</p><p>NOTE流体边框</p></div>
    <div class="important custom-block github-alert"><p class="custom-block-title">IMPORTANT</p><p>IMPORTANT流体边框</p></div>
    <div class="caution custom-block github-alert"><p class="custom-block-title">CAUTION</p><p>CAUTION流体边框</p></div>
  </div>
</template>

<style>
#fluid-border {

/* info容器:背景色、流体边框 */
.custom-block.info {
  border-left: none;
  position: relative;
  transition: all .3s;
}



/* info容器:流体边框 */
.custom-block.info::before,
.custom-block.info::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-info-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* info容器:流体边框动画 */
.custom-block.info::after {
  animation: clippath 6s infinite -3s linear;
}




/* TIP容器::背景色、流体边框 */
.custom-block.tip {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* TIP容器:流体边框 */
.custom-block.tip::before,
.custom-block.tip::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-tip-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* TIP容器:流体边框动画 */
.custom-block.tip::after {
  animation: clippath 6s infinite -3s linear;
}




/* WARNING:背景色、流体边框 */
.custom-block.warning {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* WARNING容器:流体边框 */
.custom-block.warning::before,
.custom-block.warning::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-warning-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* WARNING容器:流体边框动画 */
.custom-block.warning::after {
  animation: clippath 6s infinite -3s linear;
}




/* DANGER容器:背景色、流体边框 */
.custom-block.danger {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* DANGER容器:流体边框 */
.custom-block.danger::before,
.custom-block.danger::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-danger-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* DANGER容器:流体边框动画 */
.custom-block.danger::after {
  animation: clippath 6s infinite -3s linear;
}








/* GitHub风格警告 */



/* NOTE容器:背景色、流体边框 */
.custom-block.note {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* NOTE容器:流体边框 */
.custom-block.note.github-alert::before,
.custom-block.note.github-alert::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-note-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* NOTE容器:流体边框动画 */
.custom-block.note.github-alert::after {
  animation: clippath 6s infinite -3s linear;
}




/* IMPORTANT容器:背景色、流体边框 */
.custom-block.important {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* IMPORTANT容器:流体边框 */
.custom-block.important.github-alert::before,
.custom-block.important.github-alert::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-important-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* IMPORTANT容器:流体边框动画 */
.custom-block.important.github-alert::after {
  animation: clippath 6s infinite -3s linear;
}




/* CAUTION容器:背景色、流体边框 */
.custom-block.caution {
  border-left: none;
  position: relative;
  transition: all .3s;
}


/* CAUTION容器:流体边框 */
.custom-block.caution.github-alert::before,
.custom-block.caution.github-alert::after {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  border: 2px solid var(--custom-block-caution-left);
  transition: all .5s;
  animation: clippath 6s infinite linear;
  border-radius: 10px;
}

/* CAUTION容器:流体边框动画 */
.custom-block.caution.github-alert::after {
  animation: clippath 6s infinite -3s linear;
}

}

/* 流光边框 - 跑马灯 */

@keyframes clippath {

0%,
100% {
  clip-path: inset(0 0 90% 0);
}

25% {
  clip-path: inset(0 90% 0 0);
}

50% {
  clip-path: inset(90% 0 0 0);
}

75% {
  clip-path: inset(0 0 0 90%);
}
}
</style>