import { defineConfig } from 'vitepress'
import { search as zhSearch } from './zh'
import { devDependencies } from '../../package.json'
import markdownItTaskCheckbox from 'markdown-it-task-checkbox'
import { groupIconMdPlugin, groupIconVitePlugin, localIconLoader } from 'vitepress-plugin-group-icons'
import { MermaidMarkdown, MermaidPlugin } from 'vitepress-plugin-mermaid';
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
    ['link', { rel: 'icon', type: 'image/png', href: '/logo.png' }],
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
    logo: { src: '/logo.png', width: 24, height: 24 },

    socialLinks: [
      { icon: 'discord', link: 'https://discord.gg/q2MjmqK4'},
      { icon: 'github', link: 'https://github.com/pixpark/gpupixel' }
    ],

    search: {
      provider: 'local',
      options: {
        detailedView: true,
        locales: { ...zhSearch }
      }
    },

    // carbonAds: { code: 'CEBDT27Y', placement: 'vuejsorg' }
  }
})
