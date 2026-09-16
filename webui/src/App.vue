<template>
  <div 
    class="h-dvh w-full flex flex-col overflow-hidden bg-surface text-on-surface select-none"
    @touchstart="handleTouchStart"
    @touchend="handleTouchEnd"
  >
    
    <main class="flex-1 min-h-0 relative overflow-hidden">
      <router-view v-slot="{ Component, route }">
        <transition :name="transitionName" mode="out-in">
          <component :is="Component" :key="route.path" />
        </transition>
      </router-view>
    </main>

    <UiNavigation />

  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import UiNavigation from '@/components/ui/UiNavigation.vue'
import { useLanguageStore } from '@/stores/Language'

const router = useRouter()
const route = useRoute()
const languageStore = useLanguageStore()

// Daftar Urutan Tab Utama (Beranda -> AppList -> Tweaks)
const mainTabs = ['/', '/applist', '/tweaks']

const transitionName = ref('slide-left')

// Variabel Koordinat Sentuhan Tangan
let touchStartX = 0
let touchStartY = 0

const handleTouchStart = (e) => {
  touchStartX = e.touches[0].clientX
  touchStartY = e.touches[0].clientY
}

const handleTouchEnd = (e) => {
  // Hanya aktifkan swipe gesture jika berada di halaman tab utama
  const currentPath = route.path
  
  // Jika sedang membuka detail aplikasi (/applist/com.xxx) atau sub-page lain, matikan gesture swipe tab
  const isMainTab = mainTabs.some(tab => tab === currentPath || (tab === '/' && (currentPath === '/' || currentPath === '/home')))
  if (!isMainTab && currentPath.split('/').length > 2) return

  const touchEndX = e.changedTouches[0].clientX
  const touchEndY = e.changedTouches[0].clientY

  const deltaX = touchEndX - touchStartX
  const deltaY = touchEndY - touchStartY

  // Pastikan usapan mendatar (Horizontal) dan bukan scroll ke atas/bawah
  if (Math.abs(deltaX) > 50 && Math.abs(deltaX) > Math.abs(deltaY) * 1.5) {
    let currentIndex = mainTabs.findIndex(t => t === currentPath || (t === '/' && currentPath === '/home'))
    if (currentIndex === -1) currentIndex = 0

    if (deltaX < 0 && currentIndex < mainTabs.length - 1) {
      // USAP KE KIRI -> Pindah ke Tab Kanan
      transitionName.value = 'slide-left'
      router.push(mainTabs[currentIndex + 1])
    } else if (deltaX > 0 && currentIndex > 0) {
      // USAP KE KANAN -> Pindah ke Tab Kiri
      transitionName.value = 'slide-right'
      router.push(mainTabs[currentIndex - 1])
    }
  }
}

onMounted(() => {
  if (languageStore.initializeLanguage) {
    languageStore.initializeLanguage()
  }
})
</script>

<style>
/* --- ANIMASI SLIDE DINAMIS SESUAI ARAH USAPAN --- */

/* 1. Slide Left (Pindah ke Kanan / Tab Selanjutnya) */
.slide-left-enter-active,
.slide-left-leave-active,
.slide-right-enter-active,
.slide-right-leave-active {
  transition: transform 0.22s cubic-bezier(0.2, 0.8, 0.2, 1),
              opacity 0.22s cubic-bezier(0.2, 0.8, 0.2, 1);
  will-change: transform, opacity;
}

.slide-left-enter-from {
  opacity: 0;
  transform: translate3d(36px, 0, 0);
}
.slide-left-leave-to {
  opacity: 0;
  transform: translate3d(-36px, 0, 0);
}

/* 2. Slide Right (Pindah ke Kiri / Tab Sebelumnya) */
.slide-right-enter-from {
  opacity: 0;
  transform: translate3d(-36px, 0, 0);
}
.slide-right-leave-to {
  opacity: 0;
  transform: translate3d(36px, 0, 0);
}
</style>
