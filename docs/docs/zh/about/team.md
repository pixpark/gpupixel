<script setup>
import { VPTeamMembers } from 'vitepress/theme'

const members = [
  {
    avatar: 'https://www.github.com/jaaronkot.png',
    name: 'Jaaron Kot',
    title: '项目创始人 & 核心开发者',
    desc: '专注于图像处理和计算机视觉领域，拥有多年 GPU 优化经验',
    links: [
      { icon: 'github', link: 'https://github.com/jaaronkot' },
      { icon: 'twitter', link: 'https://twitter.com/jaaronkot' }
    ]
  }
]

const contributors = [
  {
    avatar: 'https://github.com/leavenotrace.png',
    name: 'Leave No Trace',
    title: '社区贡献者',
    desc: '专注于 Android 平台开发'
  },
  {
    avatar: 'https://github.com/weiyu666.png',
    name: 'Wei Yu',
    title: '社区贡献者',
    desc: '专注于 iOS 平台开发'
  }
]
</script>

# 团队介绍

## 关于我们

GPUPixel 是一个充满活力的开源项目，由一群热爱图像处理技术的开发者共同维护。我们致力于为开发者提供高性能、易用的图像处理解决方案。

## 核心团队

GPUPixel 项目由 [Jaaron Kot](https://github.com/jaaronkot) 创建并主导开发。核心团队成员均具备丰富的图像处理经验和跨平台开发能力。

<VPTeamMembers size="medium" :members="members" />

## 技术方向

- 🎨 图像处理算法优化
- 🚀 GPU 渲染性能提升
- 📱 跨平台兼容性支持
- 🛠️ 开发者工具改进
- 📖 文档和示例完善
 
## 加入我们

我们欢迎各类形式的贡献，包括但不限于：

- 💡 提出新的想法和建议
- 🐛 报告和修复 Bug
- 📝 改进文档和示例
- 🌍 提供多语言支持
- 🔧 优化工具链

### 如何参与

1. 查看我们的 [贡献指南](../guide/contributing)
2. 关注项目 [GitHub](https://github.com/pixpark/gpupixel) 动态

## 联系我们

- 📧 邮件：[hello@pixpark.net](mailto:hello@pixpark.net)
 
---

💝 感谢所有为 GPUPixel 项目做出贡献的开发者！