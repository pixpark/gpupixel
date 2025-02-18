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

# Team

The GPUPixel project was originally created by [Jaaron Kot](https://github.com/jaaronkot)

<VPTeamMembers size="small" :members="members" />