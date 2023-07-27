import type { DefaultTheme } from 'vitepress'

export const sidebar: DefaultTheme.Config['sidebar'] = {
  '/docs/gpupixel': [
    {
      text: 'Introduction',
      collapsed: false,
      items: [
        { text: 'Quick Start', link: '/docs/gpupixel/start' },
        { text: 'Build', link: '/docs/gpupixel/run' },
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
  '/docs/sphere-camera': [
    {
      text: '简介',
      collapsed: false,
      items: [
        { text: 'eee', link: '/docs/sphere-camera/start' },
        { text: 'Build', link: '/docs/sphere-camera//run' },
      ]
    },
    {
      text: '编译',
      collapsed: false,
      items: [
        { text: 'jjj', link: '/fe/html/' },
        { text: 'mmmm', link: '/fe/css/' }
      ]
    },
    {
      text: 'iiii成',
      link: '/fe/coding/'
    }
  ]
}
