import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)

export const en = defineConfig({
  lang: 'en-US',
  description: 'Realtime AI Beauty Filter Library.',

  themeConfig: {
    nav: nav(),

    sidebar: {
      '/guide/': { base: '/guide/', items: sidebarGuide() },
      '/1.2.5/en/guide/': { base: '/1.2.5/en/guide/', items: sidebarGuide() },
      '/reference/': { base: '/reference/', items: sidebarReference() }
    },

    editLink: {
      pattern: 'https://github.com/pixpark/gpupixel/edit/main/docs/docs/:path',
      text: 'Edit this page on GitHub'
    },

    footer: {
      message: 'Released under the MIT License.',
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
      text: 'Reference',
      link: '/reference/site-config',
      activeMatch: '/reference/'
    },
    {
      text: '1.3.0-beta',
      items: [
        {
          text: '1.3.0-beta',
          link: '/guide/intro'
        },
        {
          text: '1.2.5',
          link: '/1.2.5/en/guide/intro'
        }
      ]
    },
    {
      text: 'About',
      items: [
        {
          text: 'Team',
          link: '/about/team'
        },
        {
          text: 'Contact',
          link: 'about/contact'
        },
        {
          items: [
            {
              text: 'Blog',
              link: 'https://pixpark.net' 
            },
            {
              text: 'Contributing',
              link: 'https://github.com/pixpark/gpupixel/issues/new/choose'
            }
          ],
        }
      ]
    },
    {
      text: 'Sponsor',
      link: 'sponsor'
    }
  ]
}

function sidebarGuide(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Quick Start',
      collapsed: false,
      items: [
        { text: 'Introduction', link: 'intro' },
        { text: 'Build', link: 'build' },
        { text: 'Examples', link: 'demo' },
        { text: 'Integration', link: 'integrated' },
      ]
    },
    {
      text: 'API Calls',
      collapsed: false,
      items: [
        { text: 'Basic Filters', link: '#' },
        { text: 'Beauty Effects', link: '#' },
        { text: 'Input and Output', link: '#' },
      ]
    },
    {
      text: 'Customization',
      collapsed: false,
      items: [
        { text: 'Custom Filters', link: '#' },
        { text: 'Custom Input', link: '#' },
        { text: 'Custom Output', link: '#' },
      ]
    },
    {
      text: 'Other',
      collapsed: false,
      items: [
        { text: 'Architecture Analysis', link: '#' },
        { text: 'Code Debugging', link: '#' },
      ]
    },
    { text: 'Configuration and API Reference', base: 'reference/', link: 'filter-list' }
  ]
}

function sidebarReference(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Reference',
      items: [
        { text: 'Filter List', link: 'filter-list' },
      ]
    }
  ]
}
