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

### Useful Links

- [GitHub](https://github.com/pixpark/gpupixel)
- [Contribution Guide](/contrib-guide/)
- [Discord Chat](https://discord.gg/q2MjmqK4)