import { defineConfig } from 'vitepress'
import { head, nav, sidebar } from './configs'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base:'gpupixel',
  title: "GPUPixel",
  description: "GPUPixel Docs",
  lang: 'en-US',
  head,
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    i18nRouting: true,
    siteTitle: 'PixPark',
    logo: '/logo.png',
    // nav and sidebar
    nav,
    sidebar,
    socialLinks: [
      { icon: 'twitter', link: 'https://twitter.com/PixParkOSS' },
      { icon: 'youtube', link: 'https://www.youtube.com/@pixpark/' },
      { icon: 'github', link: 'https://github.com/pixpark/gpupixel' }
    ],

    footer: {
      message: 'Released under the MIT License.',
      copyright: 'Copyright © 2023-present PixPark'
    },
    // 
    editLink: {
      pattern: 'https://github.com/pixpark/pixpark.github.io/edit/main/docs/:path',
      text: 'Edit this page on GitHub'
    },

    lastUpdated: {
      text: 'Updated at',
      formatOptions: {
        dateStyle: 'full',
        timeStyle: 'medium'
      }
    },
    search: {
      provider: 'local'
    },
  }
})
