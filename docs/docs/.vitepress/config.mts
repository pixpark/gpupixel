import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base: '/gpupixel/',
  title: "GPUPixel",
  titleTemplate: ':title - GPUPixel',
  description: "Cross-Platform AI Beauty Effects Library, Achieving Commercial-Grade Beauty Effects. Written in C++11, Based on OpenGL/ES and VNN.",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config,
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Docs', link: '/markdown-examples' }
    ],
    logo: '/image/logo.png',
    sidebar: [
      {
        text: 'Docs',
        items: [
          { text: 'Markdown Examples', link: '/markdown-examples' },
          { text: 'Runtime API Examples', link: '/api-examples' }
        ]
      }
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/vuejs/vitepress' }
    ]
  }
})