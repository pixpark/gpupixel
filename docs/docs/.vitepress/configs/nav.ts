import type { DefaultTheme } from 'vitepress'

export const nav: DefaultTheme.Config['nav'] = [
  {
    text: 'Guide',
    link: '/guide/introduction',
    activeMatch: '^/guide'
  },
  {
    text: 'Filter',
    link: '/filter/',
    activeMatch: '^/filter'
  },
  { text: 'Demo', link: '/demo/', activeMatch: '^/demo' }
]