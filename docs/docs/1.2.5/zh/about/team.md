<script setup>
import { VPTeamMembers } from 'vitepress/theme'

const members = [
  {
    avatar: 'https://www.github.com/jaaronkot.png',
    name: 'Jaaron Kot',
    links: [
      { icon: 'github', link: 'https://github.com/jaaronkot' },
      { icon: 'twitter', link: 'https://twitter.com/jaaronkot' }
    ]
  }
]
</script>

# 团队

GPUPixel 项目最开始由 [Jaaron Kot](https://github.com/jaaronkot) 创建

<VPTeamMembers size="small" :members="members" />