<template>
  <div 
    class="page settings-page w-full min-h-screen h-screen overflow-y-scroll bg-surface pb-44 select-none"
    style="touch-action: pan-y !important; -webkit-overflow-scrolling: touch !important;"
  >
    <!-- Header Sticky -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto p-5 pb-3">
        <div class="flex items-center gap-4 text-on-surface">
          <button 
            @click="$router.back()" 
            class="p-2 -ml-2 rounded-full text-on-surface hover:bg-on-surface/10 transition-colors cursor-pointer"
            :title="t('common.back', 'Kembali')"
          >
            <ArrowLeftIcon class="w-6 h-6 text-on-surface rtl:rotate-180" />
          </button>
          <h1 class="text-xl font-semibold">
            {{ t('appearance_page.title', 'Tampilan') }}
          </h1>
        </div>
      </div>
    </div>

    <!-- Content Area -->
    <div class="scrollbar-hidden max-w-3xl mx-auto p-5 py-3 space-y-4">
      
      <!-- KATEGORI HEADER -->
      <div class="px-1">
        <h2 class="text-xs font-bold text-primary tracking-wider">
          {{ t('appearance_page.banner_settings', 'Pengaturan banner') }}
        </h2>
      </div>

      <!-- KARTU UTAMA BANNER STUDIO -->
      <div class="bg-surface-container-low p-4 rounded-[28px] space-y-4 shadow-sm">
        
        <!-- 1. Live Banner Preview Box (Rasio Aspek Presisi 2.6:1 Identik Home) -->
        <div class="relative overflow-hidden rounded-2xl w-full aspect-[2.6/1] shadow-inner flex flex-col justify-end p-4">
          <img 
            v-if="bannerBgImage" 
            :src="bannerBgImage" 
            class="absolute inset-0 w-full h-full object-cover transition-none pointer-events-none"
            :style="{ objectPosition: `center ${bannerPosY}%` }"
            alt="Preview Banner"
          />
          <div 
            v-else 
            class="absolute inset-0 bg-gradient-to-br from-primary/35 via-surface-container-low to-surface-container-low flex items-center justify-center"
          >
            <span class="text-xs text-on-surface-variant font-medium">Default Banner</span>
          </div>

          <!-- Overlay Masking Gelap -->
          <div 
            class="absolute inset-0 bg-black pointer-events-none transition-none" 
            :style="{ opacity: bannerBgImage ? `${bannerOverlay / 100}` : '0.2' }"
          ></div>

          <!-- Mockup Teks Beranda -->
          <div class="relative z-10 flex items-end justify-between pointer-events-none">
            <div class="min-w-0 pr-2">
              <span class="text-xs text-white/90 font-medium tracking-wide drop-shadow block truncate">
                {{ bannerSubtitle || t('appearance_page.preview_sub', 'Pratinjau Beranda') }}
              </span>
            </div>
            
            <div class="px-2.5 py-1 rounded-lg bg-black/50 backdrop-blur-md text-[10px] font-semibold text-white/90 shrink-0">
              {{ t('appearance_page.preview', 'Pratinjau') }}
            </div>
          </div>
        </div>

        <!-- 2. Segmented Action Buttons -->
        <div class="grid grid-cols-2 gap-2.5">
          <button 
            @click="$refs.fileInputRef.click()"
            class="flex items-center justify-center gap-2 py-3 px-3 rounded-2xl bg-surface-container-high hover:bg-surface-container-highest text-on-surface text-xs font-semibold transition-all active:scale-[0.98] cursor-pointer"
          >
            <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 text-primary" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <rect width="18" height="18" x="3" y="3" rx="2" ry="2"/>
              <circle cx="9" cy="9" r="2"/>
              <path d="m21 15-3.086-3.086a2 2 0 0 0-2.828 0L6 21"/>
            </svg>
            <span>{{ t('appearance_page.choose_file', 'Pilih file ...') }}</span>
          </button>

          <button 
            @click="resetBannerToDefault"
            class="flex items-center justify-center gap-2 py-3 px-3 rounded-2xl bg-surface-container-high/50 hover:bg-surface-container-high text-on-surface-variant hover:text-on-surface text-xs font-semibold transition-all active:scale-[0.98] cursor-pointer"
          >
            <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/>
              <path d="M3 3v5h5"/>
            </svg>
            <span>Default</span>
          </button>

          <input 
            type="file" 
            ref="fileInputRef" 
            accept="image/*" 
            class="hidden" 
            @change="handleFileSelected" 
          />
        </div>

        <!-- 3. SLIDER 1: POSISI VERTIKAL GAMBAR -->
        <div v-if="bannerBgImage" class="px-1 pt-2 border-t border-on-surface/5 space-y-2">
          <div class="flex justify-between items-center text-xs font-medium text-on-surface">
            <span class="text-on-surface-variant font-medium">
              {{ t('appearance_page.image_pos', 'Posisi gambar (Vertikal)') }}
            </span>
            <div class="flex items-center gap-2 font-mono">
              <span class="text-on-surface font-bold">{{ bannerPosY }}%</span>
              <button 
                @click="bannerPosY = 50; saveSettings()" 
                class="p-1 text-on-surface-variant hover:text-primary transition-colors cursor-pointer"
                :title="t('common.reset', 'Reset Posisi')"
              >
                <svg xmlns="http://www.w3.org/2000/svg" class="w-3.5 h-3.5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                  <path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/>
                  <path d="M3 3v5h5"/>
                </svg>
              </button>
            </div>
          </div>

          <!-- Slider Dual Track -->
          <div class="relative w-full h-8 flex items-center touch-none select-none">
            <div 
              class="absolute left-0 h-4 bg-primary rounded-full pointer-events-none transition-none"
              :style="{ width: `max(0px, calc(${bannerPosY}% - 8px))` }"
            ></div>

            <div 
              class="absolute right-0 h-4 bg-surface-container-highest rounded-full pointer-events-none transition-none"
              :style="{ width: `max(0px, calc((100% - ${bannerPosY}%) - 8px))` }"
            ></div>

            <div 
              class="absolute top-1/2 -translate-y-1/2 w-[5px] h-8 bg-primary rounded-full pointer-events-none z-10 -translate-x-1/2 transition-none shadow-sm"
              :style="{ left: `${bannerPosY}%` }"
            ></div>

            <input
              type="range"
              min="0"
              max="100"
              step="1"
              v-model.number="bannerPosY"
              @change="saveSettings"
              class="m3-expressive-input absolute inset-0 w-full h-full opacity-0 cursor-pointer z-20"
            />
          </div>
        </div>

        <!-- 4. SLIDER 2: OPACITY GRADIEN -->
        <div class="px-1 pt-2 border-t border-on-surface/5 space-y-2">
          <div class="flex justify-between items-center text-xs font-medium text-on-surface">
            <span class="text-on-surface-variant font-medium">
              {{ t('appearance_page.gradient_opacity', 'Opacity gradien') }}
            </span>
            <div class="flex items-center gap-2 font-mono">
              <span class="text-on-surface font-bold">{{ bannerOverlay }}%</span>
              <button 
                @click="bannerOverlay = 25; saveSettings()" 
                class="p-1 text-on-surface-variant hover:text-primary transition-colors cursor-pointer"
                :title="t('common.reset', 'Reset Opacity')"
              >
                <svg xmlns="http://www.w3.org/2000/svg" class="w-3.5 h-3.5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                  <path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/>
                  <path d="M3 3v5h5"/>
                </svg>
              </button>
            </div>
          </div>

          <!-- Slider Dual Track -->
          <div class="relative w-full h-8 flex items-center touch-none select-none">
            <div 
              class="absolute left-0 h-4 bg-primary rounded-full pointer-events-none transition-none"
              :style="{ width: `max(0px, calc(${bannerOverlay}% - 8px))` }"
            ></div>

            <div 
              class="absolute right-0 h-4 bg-surface-container-highest rounded-full pointer-events-none transition-none"
              :style="{ width: `max(0px, calc((100% - ${bannerOverlay}%) - 8px))` }"
            ></div>

            <div 
              class="absolute top-1/2 -translate-y-1/2 w-[5px] h-8 bg-primary rounded-full pointer-events-none z-10 -translate-x-1/2 transition-none shadow-sm"
              :style="{ left: `${bannerOverlay}%` }"
            ></div>

            <input
              type="range"
              min="0"
              max="100"
              step="1"
              v-model.number="bannerOverlay"
              @change="saveSettings"
              class="m3-expressive-input absolute inset-0 w-full h-full opacity-0 cursor-pointer z-20"
            />
          </div>
        </div>

      </div>

    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useI18n } from 'vue-i18n'

import ArrowLeftIcon from '@/components/icons/IconArrowLeft.vue'

const router = useRouter()
const { t } = useI18n()

const bannerBgImage = ref('')
const bannerOverlay = ref(25)
const bannerPosY = ref(50)
const bannerSubtitle = ref('')
const fileInputRef = ref(null)

onMounted(() => {
  loadSettings()
})

function loadSettings() {
  bannerBgImage.value = localStorage.getItem('lumina_banner_bg') || ''
  bannerOverlay.value = parseInt(localStorage.getItem('lumina_banner_overlay') || '25', 10)
  bannerPosY.value = parseInt(localStorage.getItem('lumina_banner_pos_y') || '50', 10)
  bannerSubtitle.value = localStorage.getItem('lumina_banner_sub') || ''
}

function saveSettings() {
  localStorage.setItem('lumina_banner_bg', bannerBgImage.value)
  localStorage.setItem('lumina_banner_overlay', bannerOverlay.value.toString())
  localStorage.setItem('lumina_banner_pos_y', bannerPosY.value.toString())
}

function resetBannerToDefault() {
  bannerBgImage.value = ''
  bannerOverlay.value = 25
  bannerPosY.value = 50
  saveSettings()
}

function handleFileSelected(e) {
  const file = e.target.files && e.target.files[0]
  if (!file) return

  const reader = new FileReader()
  reader.onload = (event) => {
    const img = new Image()
    img.onload = () => {
      const canvas = document.createElement('canvas')
      const maxW = 1080
      let width = img.width
      let height = img.height

      if (width > maxW) {
        height = Math.round((height * maxW) / width)
        width = maxW
      }

      canvas.width = width
      canvas.height = height
      const ctx = canvas.getContext('2d')
      ctx.drawImage(img, 0, 0, width, height)

      bannerBgImage.value = canvas.toDataURL('image/jpeg', 0.82)
      saveSettings()
    }
    img.src = event.target.result
  }
  reader.readAsDataURL(file)
}
</script>

<style scoped>
.settings-page {
  touch-action: pan-y !important;
  -webkit-overflow-scrolling: touch !important;
  overscroll-behavior-y: contain;
  overflow-y: scroll !important;
}

.m3-expressive-input::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 16px;
  height: 32px;
  background: transparent;
  cursor: pointer;
}

.m3-expressive-input::-moz-range-thumb {
  width: 16px;
  height: 32px;
  background: transparent;
  border: none;
  cursor: pointer;
}
</style>
