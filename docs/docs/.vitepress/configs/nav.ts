import type { DefaultTheme } from 'vitepress'

export const nav: DefaultTheme.Config['nav'] = [
  {
    text: '项目',
    items: [
      {
        text: '美颜特效库',
        items: [
          { text: 'GPUPixel', link: 'projects/gpupixel' }
        ]
      },
      {
        text: '全景相机',
        items: [
          { text: 'SphereCam', link: 'projects/sphere-camera' }
        ]
      },
    ],
    activeMatch: '^/projects'
  },
  {
    text: '文档',
    items: [
      {
        text: 'SDK',
        items: [
          { text: 'GPUPixel', link: '/docs/gpupixel/start' },
          { text: 'SphereCam', link: '/docs/sphere-camera/start' },
        ]
      },
    ],
    activeMatch: '^/docs'
  },
  { text: '博客', link: '/blog/', activeMatch: '^/blog' }
]