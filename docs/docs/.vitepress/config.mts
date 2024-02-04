import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base: '/gpupixel/',
  title: "GPUPixel",
  titleTemplate: ':title - GPUPixel',
  description: "Cross-Platform AI Beauty Effects Library, Achieving Commercial-Grade Beauty Effects. Written in C++11, Based on OpenGL/ES and VNN.",
  head: [['link', { rel: 'icon', href: '/gpupixel/favicon/favicon.ico' }]],

  locales: {
    root: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/' 
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      themeConfig: {
        nav: nav_en(),
        docFooter: {
          prev: 'Previous Page',
          next: 'Next Page'
        },
        outlineTitle: 'On this page',
        editLink: {
          pattern: 'https://github.com/pixpark/gpupixel/docs/docs/en/:path',
          text: 'Edit this page on GitHub'
        },
        darkModeSwitchLabel: 'Appearance',
        sidebarMenuLabel: 'Menu',
        returnToTopLabel: 'Return to top',
        langMenuLabel: 'Change Language',
        lastUpdatedText: 'Last Updated'
      }
    }
  }, 

  appearance: true,
  lastUpdated: true,
  ignoreDeadLinks: true,  
  markdown: {
    // theme: 'material-theme-palenight',
    lineNumbers: false
  },

  themeConfig: {
    logo: '/logo.png',
    i18nRouting: true,
    lastUpdatedText: '最近更新时间',
    outline: 'deep',
    outlineTitle: '本页目录',
    editLink: {
      pattern: 'https://github.com/pixpark/gpupixel/docs/docs/:path',
      text: '在 GitHub 上编辑此页'
    },
    search: {
      provider: 'local',
      options: {
        locales: {
          root: {
            translations: {
              button: {
                buttonText: '搜索文档',
                buttonAriaLabel: '搜索文档'
              },
              modal: {
                noResultsText: '无法找到相关结果',
                resetButtonTitle: '清除查询条件',
                footer: {
                  selectText: '选择',
                  navigateText: '切换',
                  closeText: '关闭'
                }
              }
            }
          }
        }
      }
    },
    // https://vitepress.dev/reference/default-theme-config,

  
    nav: nav(),
            
    sidebar: {
      '/': sidebarDocs()
    },

    footer: {
      message: '基于 MIT License 发布',
      copyright: 'Copyright © 2023-present @PixPark'
    },
    docFooter: {
      prev: '上一页',
      next: '下一页'
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/pixpark/gpupixel' },
      { icon: 'youtube', link: 'https://www.youtube.com/@pixpark' },
      { icon: 'x', link: 'https://pixpark.net' }
    ]
  }
})

function nav() {
  return [ 
      { text: '主页', link: '/' },
      { text: '文档', link: '/intro' },
      // { text: '1.1.x',
      //    items: [
      //      { text: '变更日志', link: 'https://github.com/vuejs/vitepress/blob/main/CHANGELOG.md' },
      //      { text: '贡献', link: 'https://github.com/vuejs/vitepress/blob/main/.github/contributing.md' }
      //    ]
      //  }        
  ]
}

function nav_en() {
  return [ 
      { text: 'Guide', link: '/en/guide/what-is-vitepress' },
      { text: 'Reference', link: '/en/reference/site-config' },
      { text: '1.0.0-beta.1',
         items: [
           { text: 'Changelog', link: 'https://github.com/vuejs/vitepress/blob/main/CHANGELOG.md' },
           { text: 'Contributing', link: 'https://github.com/vuejs/vitepress/blob/main/.github/contributing.md' }
         ]
       }        
  ]
}

function sidebarDocs() {
  return [
      {
        text: '快速开始',
        collapsed: false,
        items: [
          { text: '简介', link: '/intro' },
          { text: '源码编译', link: '/how-to-build' },
          { text: 'SDK集成', link: '/how-to-use' }
        ]
      },
      {
        text: '滤镜',
        collapsed: true,
        items: [
          { text: '内建滤镜', link: '/in-buiding' },
          { text: '自定义滤镜', link: '/in-buiding' },
        ]
      },
      {
        text: '源码分析',
        collapsed: true,
        items: [
          { text: '架构', link: '/in-buiding' },
          { text: '流程', link: '/in-buiding' },
        ]
      }
    ]
}