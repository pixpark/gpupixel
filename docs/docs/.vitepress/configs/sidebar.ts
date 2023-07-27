import type { DefaultTheme } from 'vitepress'

export const sidebar: DefaultTheme.Config['sidebar'] = {
  '/guide/': [
    {
      text: 'Getting Started',
      collapsed: false,
      items: [
        { text: 'Introduction', link: '/guide/introduction' },
        { text: 'Quick Start', link: '/guide/quick-start' },
      ]
    },
    {
      text: '编译',
      collapsed: false,
      items: [
        { text: 'iOS编译', link: '/fe/html/' },
        { text: 'Android编译', link: '/fe/css/' }
      ]
    },
    {
      text: '集成',
      link: '/fe/coding/'
    }
  ],
  '/filter': [
    {
      text: '简介',
      collapsed: false,
      items: [
        { text: 'eee', link: '/filter/sphere-camera/start' },
        { text: 'Build', link: '/filter/sphere-camera//run' },
      ]
    },
    {
      text: '编译',
      collapsed: false,
      items: [
        { text: 'jjj', link: '/filter/html/' },
        { text: 'mmmm', link: '/filter/css/' }
      ]
    }
  ],
  '/demo': [
    {
      text: '简介',
      collapsed: false,
      items: [
        { text: 'eee', link: '/demo/sphere-camera/start' },
        { text: 'Build', link: '/demo/sphere-camera//run' },
      ]
    },
    {
      text: '编译',
      collapsed: false,
      items: [
        { text: 'jjj', link: '/demo/html/' },
        { text: 'mmmm', link: '/demo/css/' }
      ]
    }
  ]
}
