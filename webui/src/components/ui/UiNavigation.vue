<template>
  <nav ref="navEl"
    class="footer fixed bottom-0 left-0 right-0 w-full flex items-end bg-surface-container shadow-lg z-50 md:left-0 md:top-0 md:bottom-0 md:w-23 md:h-full md:flex-col backdrop-blur-md"
    :style="{
      paddingBottom: 'var(--window-inset-bottom, 0px)',
      paddingRight: 'var(--window-inset-right, 0px)',
      paddingLeft: 'var(--window-inset-left, 0px)'
    }">
    <div class="w-full h-20 flex items-center justify-center md:h-full md:flex-col md:justify-center">
      <router-link v-for="item in navItems" :key="item.name" :to="item.path"
        class="footer-btn gap-1 w-full max-w-50 text-on-secondary-container border-none bg-transparent text-sm flex justify-center items-center flex-col user-select-none p-0 no-underline transition-all duration-200 md:max-h-min md:py-3"
        :class="{
          'text-on-background': isActive(item),
          'text-on-surface-variant': !isActive(item),
        }">
        <div
          class="footer-btn-icon h-8 flex justify-center items-center rounded-full transition-all duration-200 ease-in-out"
          :class="{
            'bg-secondary-container px-5': isActive(item),
            'px-0': !isActive(item),
          }">
          <component :is="item.icon" :active="isActive(item)" />
        </div>
        <div class="footer-btn-text text-xs">
          <span class="font-medium">{{ item.label }}</span>
        </div>
      </router-link>
    </div>
  </nav>
</template>

<script setup>
import { computed, ref, onMounted, onBeforeUnmount, h } from 'vue'
import { useRoute } from 'vue-router'
import { useI18n } from 'vue-i18n'

import HomeIcon from '@/components/icons/IconHome.vue'

const { t } = useI18n()
const route = useRoute()

// 1. Ikon SVG AppList (Grid Apps)
const AppListIcon = () => h('svg', {
  xmlns: 'http://www.w3.org/2000/svg',
  class: 'w-5 h-5',
  viewBox: '0 0 24 24',
  fill: 'none',
  stroke: 'currentColor',
  'stroke-width': '2',
  'stroke-linecap': 'round',
  'stroke-linejoin': 'round'
}, [
  h('rect', { x: '3', y: '3', width: '7', height: '7', rx: '1.5' }),
  h('rect', { x: '14', y: '3', width: '7', height: '7', rx: '1.5' }),
  h('rect', { x: '14', y: '14', width: '7', height: '7', rx: '1.5' }),
  h('rect', { x: '3', y: '14', width: '7', height: '7', rx: '1.5' })
])

// 2. Ikon SVG Tweak (Sliders Control)
const TweakIcon = () => h('svg', {
  xmlns: 'http://www.w3.org/2000/svg',
  class: 'w-5 h-5',
  viewBox: '0 0 24 24',
  fill: 'none',
  stroke: 'currentColor',
  'stroke-width': '2',
  'stroke-linecap': 'round',
  'stroke-linejoin': 'round'
}, [
  h('line', { x1: '4', y1: '21', x2: '4', y2: '14' }),
  h('line', { x1: '4', y1: '10', x2: '4', y2: '3' }),
  h('line', { x1: '12', y1: '21', x2: '12', y2: '12' }),
  h('line', { x1: '12', y1: '8', x2: '12', y2: '3' }),
  h('line', { x1: '20', y1: '21', x2: '20', y2: '16' }),
  h('line', { x1: '20', y1: '12', x2: '20', y2: '3' }),
  h('line', { x1: '1', y1: '14', x2: '7', y2: '14' }),
  h('line', { x1: '9', y1: '8', x2: '15', y2: '8' }),
  h('line', { x1: '17', y1: '16', x2: '23', y2: '16' })
])

// NavItems dengan Path & Label Baru
const navItems = computed(() => [
  {
    name: 'Home',
    path: '/',
    label: t('navigation.home') !== 'navigation.home' ? t('navigation.home') : 'Beranda',
    icon: HomeIcon,
  },
  {
    name: 'AppList',
    path: '/applist',
    label: 'AppList',
    icon: AppListIcon,
  },
  {
    name: 'Tweaks',
    path: '/tweaks',
    label: 'Tweak',
    icon: TweakIcon,
  },
])

const isActive = (item) => {
  const currentPath = route.path
  if (item.path === '/') return currentPath === '/'
  return currentPath.startsWith(item.path)
}

const navEl = ref(null)
let ro = null

onMounted(() => {
  ro = new ResizeObserver(([entry]) => {
    const h = entry.borderBoxSize?.[0]?.blockSize ?? entry.target.offsetHeight
    document.documentElement.style.setProperty('--nav-height', `${h}px`)
  })
  ro.observe(navEl.value)
})

onBeforeUnmount(() => ro?.disconnect())
</script>

<style scoped>
.footer-btn {
  flex: 1;
}

.footer-btn-icon {
  transition:
    background-color 0.2s ease,
    padding 0.25s cubic-bezier(0.4, 0, 0.2, 1);
}
</style>
