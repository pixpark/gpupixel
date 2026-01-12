import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)

export const en = defineConfig({
  lang: 'en-US',
  description: 'Realtime AI Beauty Filter Library.',

  themeConfig: {
    nav: nav(),

    sidebar: {
      '/': { base: '/', items: sidebarGuide() },
    },

    editLink: {
      pattern: 'https://github.com/pixpark/gpupixel/edit/main/docs/docs/:path',
      text: 'Edit this page on GitHub'
    },

    footer: {
      message: 'Released under the Apache-2.0 License.',
      copyright: 'Copyright © 2019-present PixPark'
    }
  }
})

function nav(): DefaultTheme.NavItem[] {
  return [
    {
      text: 'Guide',
      link: '/guide/intro',
      activeMatch: '/guide/'
    },
    {
      text: '1.3.0-beta',
      items: [
        {
          text: '1.3.0-beta',
          link: 'https://github.com/pixpark/gpupixel/releases/tag/v1.3.0-beta'
        }
      ]
    },
    {
      text: 'About',
      items: [
        {
          text: 'Contact',
          link: 'about/contact'
        },
        {
          items: [
            {
              text: 'Contributing',
              link: 'https://github.com/pixpark/gpupixel/issues/new/choose'
            }
          ],
        }
      ]
    }
  ]
}

function sidebarGuide(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Quick Start',
      collapsed: false,
      items: [
        { text: 'Intro', link: 'guide/intro' },
        { text: 'Build', link: 'guide/build' },
        { text: 'Demo', link: 'guide/demo' },
        { text: 'Integration', link: 'guide/integrated' },
      ]
    },
    {
      text: 'API Calls',
      collapsed: false,
      items: [
        { text: 'Basic Filters', link: 'call/basic_filters' },
        { text: 'Beauty Effects', link: 'call/beauty_effects' },
        { text: 'Input and Output', link: 'call/input_output' },
      ]
    },
    {
      text: 'Custom',
      collapsed: false,
      items: [
        { text: 'Custom Filters', link: 'custom/custom_filter' },
        { text: 'Custom Input', link: 'custom/custom_input' },
        { text: 'Custom Output', link: 'custom/custom_target' },
      ]
    },   
    {
      text: 'Reference',
      collapsed: false,
      items: [
        { text: 'Filter List', link: 'reference/filter-list' },
      ]
    }
  ]
}