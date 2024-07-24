import { defineConfig } from 'vitepress'
import { search as zhSearch } from './zh'

export const shared = defineConfig({
  title: 'GPUPixel',

  rewrites: {
    'en/:rest*': ':rest*'
  },
  
  lastUpdated: true,
  cleanUrls: true,
  metaChunk: true,

  markdown: {
    math: true,
    codeTransformers: [
      // We use `[!!code` in demo to prevent transformation, here we revert it back.
      {
        postprocess(code) {
          return code.replace(/\[\!\!code/g, '[!code')
        }
      }
    ]
  },

  sitemap: {
    hostname: 'https://open.pixpark.net/gpupixel',
    transformItems(items) {
      return items.filter((item) => !item.url.includes('migration'))
    }
  },

  /* prettier-ignore */
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/gpupixel-logo-mini.svg' }],
    ['link', { rel: 'icon', type: 'image/png', href: '/gpupixel-logo-mini.svg' }],
    ['meta', { name: 'theme-color', content: '#5f67ee' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'GPUPixel | Realtime Image filter Lib' }],
    ['meta', { property: 'og:site_name', content: 'GPUPixel' }],
    ['meta', { property: 'og:image', content: 'https://vitepress.dev/vitepress-og.jpg' }],
    ['meta', { property: 'og:url', content: 'https://gpupixel.pixpark.net/' }],
    ['script', { src: 'https://cdn.usefathom.com/script.js', 'data-site': 'AZBRSFGG', 'data-spa': 'auto', defer: '' }]
  ],

  themeConfig: {
    logo: { src: '/gpupixel-logo-mini.svg', width: 24, height: 24 },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/pixpark/gpupixel' }
    ],

    search: {
      provider: 'algolia',
      options: {
        appId: 'EJA99Y55JA',
        apiKey: '1df90cf42bd532964a5c08e31dbbc9cb',
        indexName: 'open-pixpark"',
        locales: { ...zhSearch }
      }
    },

    // carbonAds: { code: 'CEBDT27Y', placement: 'vuejsorg' }
  }
})
