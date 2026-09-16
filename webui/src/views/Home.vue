<template>
  <div class="page home-page h-full flex flex-col bg-surface select-none">
    <!-- Header Sticky -->
    <div class="sticky top-0 z-20 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto px-5 pt-10 pb-3">
        <div class="flex justify-between items-center text-on-surface">
          <h1 class="text-xl font-bold tracking-tight text-on-surface">LUMina Tweaks</h1>
          
          <button 
            @click="$router.push('/tweaks/general')" 
            class="p-2 -mr-2 rounded-full text-on-surface-variant hover:text-on-surface hover:bg-surface-container-high transition-all active:scale-90 cursor-pointer"
            :title="t('common.settings', 'Pengaturan')"
          >
            <IconDots class="w-5 h-5" />
          </button>
        </div>
      </div>
    </div>

    <!-- Scrollable Content -->
    <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll">
      <div class="max-w-3xl mx-auto p-5 space-y-3.5">

        <!-- BANNER CUSTOM (Rasio Aspek Presisi 2.6:1) -->
        <div 
          @click="openBannerModal"
          class="group relative overflow-hidden rounded-3xl shadow-sm border-none w-full aspect-[2.6/1] flex flex-col justify-end p-4 cursor-pointer transition-all active:scale-[0.99]"
          title="Klik untuk atur gambar & teks banner"
        >
          <img 
            v-if="bannerBgImage" 
            :src="bannerBgImage" 
            class="absolute inset-0 w-full h-full object-cover pointer-events-none transition-all duration-300"
            :style="{ objectPosition: `center ${bannerPosY}%` }"
            alt="Banner Custom Background"
          />
          
          <div v-else class="absolute inset-0 bg-gradient-to-br from-primary/35 via-surface-container-low to-surface-container-low">
            <div class="absolute -right-10 -bottom-10 w-48 h-48 bg-primary/25 rounded-full blur-3xl pointer-events-none"></div>
          </div>

          <div 
            class="absolute inset-0 bg-black pointer-events-none transition-opacity"
            :style="{ opacity: bannerBgImage ? `${bannerOverlayOpacity / 100}` : '0.2' }"
          ></div>

          <div class="relative z-10 flex justify-between items-end">
            <div class="min-w-0 pr-3">
              <h2 
                v-if="bannerTitle" 
                class="text-2xl font-black tracking-wider text-white font-mono uppercase truncate leading-tight drop-shadow-md"
              >
                {{ bannerTitle }}
              </h2>
              <p class="text-xs text-white/90 font-medium tracking-wide drop-shadow leading-relaxed truncate">
                {{ displaySub }}
              </p>
            </div>
          </div>
        </div>

        <!-- GRID 2 KARTU STATUS -->
        <div class="grid grid-cols-2 gap-3">
          
          <div class="bg-surface-container-low p-3.5 rounded-3xl flex flex-col justify-between transition-all">
            <div class="w-full h-16 rounded-2xl bg-primary/20 text-primary flex items-center justify-center mb-3">
              <svg xmlns="http://www.w3.org/2000/svg" class="w-6 h-6" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/>
                <path d="m9 12 2 2 4-4"/>
              </svg>
            </div>
            <div class="px-1 pb-0.5">
              <h3 class="text-sm font-bold text-on-surface">Akses Root</h3>
              <p class="text-xs text-on-surface-variant font-medium mt-0.5 truncate">
                {{ isDaemonOnline ? 'Aktif' : 'Tidak Aktif' }}
              </p>
            </div>
          </div>

          <div class="bg-surface-container-low p-3.5 rounded-3xl flex flex-col justify-between transition-all">
            <div class="w-full h-16 rounded-2xl bg-primary/20 text-primary flex items-center justify-center mb-3">
              <svg xmlns="http://www.w3.org/2000/svg" class="w-6 h-6" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <line x1="4" y1="21" x2="4" y2="14"/>
                <line x1="4" y1="10" x2="4" y2="3"/>
                <line x1="12" y1="21" x2="12" y2="12"/>
                <line x1="12" y1="8" x2="12" y2="3"/>
                <line x1="20" y1="21" x2="20" y2="16"/>
                <line x1="20" y1="12" x2="20" y2="3"/>
                <line x1="1" y1="14" x2="7" y2="14"/>
                <line x1="9" y1="8" x2="15" y2="8"/>
                <line x1="17" y1="16" x2="23" y2="16"/>
              </svg>
            </div>
            <div class="px-1 pb-0.5">
              <h3 class="text-sm font-bold text-on-surface">Profil Aktif</h3>
              <p class="text-xs text-on-surface-variant font-medium capitalize mt-0.5 truncate">
                {{ currentProfileText }}
              </p>
            </div>
          </div>

        </div>

        <!-- INFORMASI MODUL & DAEMON -->
        <div class="bg-surface-container-low p-5 rounded-3xl space-y-4 shadow-sm">
          
          <div class="flex items-center gap-4">
            <div class="w-9 h-9 rounded-xl bg-surface-container-high text-primary flex items-center justify-center shrink-0">
              <svg xmlns="http://www.w3.org/2000/svg" class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"/>
                <polyline points="3.27 6.96 12 12.01 20.73 6.96"/>
                <line x1="12" y1="22.08" x2="12" y2="12"/>
              </svg>
            </div>
            <div class="min-w-0 flex-1">
              <p class="text-xs text-on-surface-variant font-medium">Versi Modul</p>
              <p class="text-sm font-bold text-on-surface truncate mt-0.5">
                {{ displayValue(homeStore.moduleVersion) }}
              </p>
            </div>
          </div>

          <div class="h-[1px] bg-on-surface/5 w-full"></div>

          <div class="flex items-center gap-4">
            <div class="w-9 h-9 rounded-xl bg-surface-container-high text-primary flex items-center justify-center shrink-0">
              <svg xmlns="http://www.w3.org/2000/svg" class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="10"/>
                <polyline points="12 6 12 12 16 14"/>
              </svg>
            </div>
            <div class="min-w-0 flex-1">
              <p class="text-xs text-on-surface-variant font-medium">Status Daemon</p>
              <p class="text-sm font-bold text-on-surface truncate mt-0.5">
                {{ isDaemonOnline ? 'Berjalan Normal' : 'Berhenti' }}
              </p>
            </div>
          </div>

        </div>

        <!-- SPESIFIKASI PERANGKAT -->
        <div class="bg-surface-container-low p-5 rounded-3xl shadow-sm">
          <div class="grid grid-cols-2 gap-y-4 gap-x-3">
            
            <div class="space-y-1">
              <div class="flex items-center gap-1.5 text-on-surface-variant">
                <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 shrink-0" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <rect x="5" y="2" width="14" height="20" rx="3"/>
                  <line x1="12" y1="18" x2="12.01" y2="18" stroke-width="3"/>
                </svg>
                <span class="text-xs font-medium">Model</span>
              </div>
              <p class="allow-copy text-sm font-bold text-on-surface truncate">
                {{ deviceNameFormatted }}
              </p>
            </div>

            <div class="space-y-1">
              <div class="flex items-center gap-1.5 text-on-surface-variant">
                <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 shrink-0" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <rect x="4" y="4" width="16" height="16" rx="2"/>
                  <rect x="9" y="9" width="6" height="6"/>
                  <line x1="9" y1="1" x2="9" y2="4"/>
                  <line x1="15" y1="1" x2="15" y2="4"/>
                  <line x1="9" y1="20" x2="9" y2="23"/>
                  <line x1="15" y1="20" x2="15" y2="23"/>
                  <line x1="20" y1="9" x2="23" y2="9"/>
                  <line x1="20" y1="15" x2="23" y2="15"/>
                  <line x1="1" y1="9" x2="4" y2="9"/>
                  <line x1="1" y1="15" x2="4" y2="15"/>
                </svg>
                <span class="text-xs font-medium">Chipset</span>
              </div>
              <p class="allow-copy text-sm font-bold text-on-surface truncate">
                {{ displayValue(homeStore.chipsetName) }}
              </p>
            </div>

            <div class="space-y-1">
              <div class="flex items-center gap-1.5 text-on-surface-variant">
                <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 shrink-0" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <polyline points="4 17 10 11 4 5"/>
                  <line x1="12" y1="19" x2="20" y2="19"/>
                </svg>
                <span class="text-xs font-medium">Versi Kernel</span>
              </div>
              <p class="allow-copy text-xs font-bold text-on-surface truncate">
                {{ displayValue(homeStore.kernelVersion) }}
              </p>
            </div>

            <div class="space-y-1">
              <div class="flex items-center gap-1.5 text-on-surface-variant">
                <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 shrink-0" viewBox="0 0 24 24" fill="currentColor">
                  <path d="M17.523 15.3414C17.0601 15.3414 16.6853 14.9666 16.6853 14.5037C16.6853 14.0408 17.0601 13.666 17.523 13.666C17.9859 13.666 18.3607 14.0408 18.3607 14.5037C18.3607 14.9666 17.9859 15.3414 17.523 15.3414ZM6.47702 15.3414C6.01413 15.3414 5.63934 14.9666 5.63934 14.5037C5.63934 14.0408 6.01413 13.666 6.47702 13.666C6.93991 13.666 7.3147 14.0408 7.3147 14.5037C7.3147 14.9666 6.93991 15.3414 6.47702 15.3414ZM17.973 10.3666L19.5393 7.65373C19.6893 7.39393 19.6001 7.06173 19.3403 6.91173C19.0805 6.76173 18.7483 6.85093 18.5983 7.11073L17.0016 9.87633C15.5393 9.20813 13.8441 8.83333 12 8.83333C10.1559 8.83333 8.4607 9.20813 6.9984 9.87633L5.4017 7.11073C5.2517 6.85093 4.9195 6.76173 4.6597 6.91173C4.3999 7.06173 4.3107 7.39393 4.4607 7.65373L6.027 10.3666C2.6105 12.2323 0.25 15.6881 0 19.75H24C23.75 15.6881 21.3895 12.2323 17.973 10.3666Z"/>
                </svg>
                <span class="text-xs font-medium">Versi SDK</span>
              </div>
              <p class="allow-copy text-sm font-bold text-on-surface font-mono truncate">
                {{ androidSDKText }}
              </p>
            </div>

          </div>
        </div>

        <!-- TOMBOL DONASI -->
        <RippleComponent
          @click="handleDonateClick"
          tabindex="0"
          class="cursor-pointer text-on-surface bg-surface-container-low p-5 rounded-3xl shadow-sm block transition-all active:scale-[0.98]"
        >
          <h2 class="text-sm font-bold mb-1">Dukung Pengembang</h2>
          <p class="text-xs text-on-surface-variant leading-relaxed">
            LUMina Tweaks bersifat gratis dan open-source untuk semua pengguna.
          </p>
        </RippleComponent>

      </div>
    </div>

    <!-- MODAL KUSTOMISASI BANNER -->
    <div 
      v-if="showModal" 
      class="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/60 backdrop-blur-sm animate-fade-in"
    >
      <div class="bg-surface-container-low rounded-3xl p-5 w-full max-w-md space-y-4 shadow-2xl max-h-[90vh] overflow-y-auto">
        <div class="flex justify-between items-center">
          <h3 class="text-base font-bold text-on-surface">Kustomisasi Banner</h3>
          <button @click="showModal = false" class="text-on-surface-variant hover:text-on-surface p-1 cursor-pointer">
            <svg class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <line x1="18" y1="6" x2="6" y2="18"/>
              <line x1="6" y1="6" x2="18" y2="18"/>
            </svg>
          </button>
        </div>

        <div class="relative overflow-hidden rounded-2xl w-full aspect-[2.6/1] flex flex-col justify-end p-4">
          <img 
            v-if="tempBgImage" 
            :src="tempBgImage" 
            class="absolute inset-0 w-full h-full object-cover"
            :style="{ objectPosition: `center ${tempPosY}%` }"
          />
          <div v-else class="absolute inset-0 bg-gradient-to-br from-primary/35 via-surface-container-low to-surface-container-low"></div>
          
          <div 
            class="absolute inset-0 bg-black pointer-events-none" 
            :style="{ opacity: tempBgImage ? `${tempOverlay / 100}` : '0.2' }"
          ></div>

          <div class="relative z-10">
            <p v-if="tempTitle" class="text-xl font-black text-white font-mono uppercase truncate leading-tight drop-shadow">
              {{ tempTitle }}
            </p>
            <p class="text-xs text-white/90 font-semibold truncate mt-0.5 drop-shadow">
              {{ tempSubtitle || deviceNameFormatted }}
            </p>
          </div>
        </div>

        <div class="space-y-3 text-xs">
          <div>
            <label class="block font-semibold text-on-surface mb-1">Judul Banner (Opsional / Boleh Kosong)</label>
            <input 
              v-model="tempTitle" 
              type="text" 
              placeholder="Kosongkan jika hanya ingin tulisan kecil" 
              class="w-full bg-surface-container-high rounded-xl px-3 py-2 text-on-surface outline-none focus:border-primary"
            />
          </div>

          <div>
            <label class="block font-semibold text-on-surface mb-1">Tulisan Kecil / Sub-judul</label>
            <input 
              v-model="tempSubtitle" 
              type="text" 
              :placeholder="deviceNameFormatted" 
              class="w-full bg-surface-container-high rounded-xl px-3 py-2 text-on-surface outline-none focus:border-primary"
            />
          </div>

          <div class="pt-1">
            <label class="block font-semibold text-on-surface mb-1.5">Gambar Background</label>
            <div class="flex items-center gap-2">
              <input 
                type="file" 
                ref="fileInputRef" 
                accept="image/*" 
                class="hidden" 
                @change="handleFileSelected" 
              />
              <button 
                @click="$refs.fileInputRef.click()" 
                class="flex-1 bg-primary text-on-primary font-medium py-2.5 px-3 rounded-xl hover:opacity-90 transition-all text-xs cursor-pointer"
              >
                🖼️ {{ tempBgImage ? 'Ganti Gambar' : 'Pilih Gambar dari Galeri' }}
              </button>
              <button 
                v-if="tempBgImage" 
                @click="removeBgImage" 
                class="bg-red-500/20 text-red-400 font-medium py-2.5 px-3 rounded-xl hover:bg-red-500/30 transition-all text-xs cursor-pointer"
              >
                Hapus
              </button>
            </div>
          </div>

          <div v-if="tempBgImage" class="space-y-3 pt-2 border-t border-on-surface/5">
            <div>
              <div class="flex justify-between font-semibold text-on-surface mb-1.5">
                <span>Posisi Gambar (Atas - Bawah)</span>
                <span>{{ tempPosY }}%</span>
              </div>
              <input 
                v-model="tempPosY" 
                type="range" 
                min="0" 
                max="100" 
                @touchstart.stop
                @touchmove.stop
                @touchend.stop
                @pointerdown.stop
                class="custom-slider w-full cursor-pointer h-2 bg-surface-container-highest rounded-lg appearance-none" 
              />
              <p class="text-[10px] text-on-surface-variant mt-1">Geser slider untuk mengatur posisi fokus gambar.</p>
            </div>

            <div>
              <div class="flex justify-between font-semibold text-on-surface mb-1.5">
                <span>Kegelapan Overlay Teks</span>
                <span>{{ tempOverlay }}%</span>
              </div>
              <input 
                v-model="tempOverlay" 
                type="range" 
                min="0" 
                max="90" 
                @touchstart.stop
                @touchmove.stop
                @touchend.stop
                @pointerdown.stop
                class="custom-slider w-full cursor-pointer h-2 bg-surface-container-highest rounded-lg appearance-none" 
              />
            </div>
          </div>
        </div>

        <div class="flex items-center justify-end gap-2 pt-2 border-t border-on-surface/5">
          <button 
            @click="showModal = false" 
            class="px-4 py-2 rounded-xl text-on-surface-variant hover:bg-surface-container-high transition-all text-xs font-semibold cursor-pointer"
          >
            Batal
          </button>
          <button 
            @click="saveBannerSettings" 
            class="px-5 py-2 rounded-xl bg-primary text-on-primary transition-all text-xs font-semibold cursor-pointer shadow-sm hover:opacity-90"
          >
            Simpan
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed } from 'vue'
import { useHomeStore } from '@/stores/Home'
import { useI18n } from 'vue-i18n'
import * as KernelSU from '@/helpers/KernelSU'

import IconDots from '@/components/icons/IconDots.vue'
import RippleComponent from '@/components/ui/UiRipple.vue'

const { t } = useI18n()
const homeStore = useHomeStore()

const bannerTitle = ref(localStorage.getItem('lumina_banner_title') ?? '')
const bannerSubtitle = ref(localStorage.getItem('lumina_banner_sub') || '')
const bannerBgImage = ref(localStorage.getItem('lumina_banner_bg') || '')
const bannerPosY = ref(localStorage.getItem('lumina_banner_pos_y') || '50')
const bannerOverlayOpacity = ref(localStorage.getItem('lumina_banner_overlay') || '40')

const showModal = ref(false)
const fileInputRef = ref(null)
const tempTitle = ref('')
const tempSubtitle = ref('')
const tempBgImage = ref('')
const tempPosY = ref('50')
const tempOverlay = ref('40')

function displayValue(value) {
  if (value === 'unknown' || !value || value === 'Tidak diketahui') return t('common.unknown')
  return value
}

const deviceNameFormatted = computed(() => {
  const val = homeStore.deviceName || homeStore.deviceModel || homeStore.deviceBrand
  if (!val || val === 'unknown' || val === 'Tidak diketahui') {
    return 'Android Device'
  }
  return val
})

const displaySub = computed(() => {
  return bannerSubtitle.value || deviceNameFormatted.value
})

function openBannerModal() {
  tempTitle.value = bannerTitle.value
  tempSubtitle.value = bannerSubtitle.value
  tempBgImage.value = bannerBgImage.value
  tempPosY.value = bannerPosY.value
  tempOverlay.value = bannerOverlayOpacity.value
  showModal.value = true
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

      tempBgImage.value = canvas.toDataURL('image/jpeg', 0.82)
    }
    img.src = event.target.result
  }
  reader.readAsDataURL(file)
}

function removeBgImage() {
  tempBgImage.value = ''
}

function saveBannerSettings() {
  bannerTitle.value = tempTitle.value.trim()
  bannerSubtitle.value = tempSubtitle.value.trim()
  bannerBgImage.value = tempBgImage.value
  bannerPosY.value = tempPosY.value
  bannerOverlayOpacity.value = tempOverlay.value

  localStorage.setItem('lumina_banner_title', bannerTitle.value)
  localStorage.setItem('lumina_banner_sub', bannerSubtitle.value)
  localStorage.setItem('lumina_banner_bg', bannerBgImage.value)
  localStorage.setItem('lumina_banner_pos_y', bannerPosY.value)
  localStorage.setItem('lumina_banner_overlay', bannerOverlayOpacity.value)

  showModal.value = false
}

const androidSDKText = computed(() => {
  const sdk = homeStore.androidSDK
  if (!sdk || sdk === 'unknown') return 'Android'
  return `SDK ${sdk}`
})

const isDaemonOnline = computed(() => {
  return homeStore.daemonStatusRaw === 'running'
})

const currentProfileText = computed(() => {
  const profileKey = homeStore.currentProfileRaw
  if (profileKey === 'unknown' || !profileKey) return t('common.unknown')

  const translation = t(`profiles.${profileKey}`)
  return translation !== `profiles.${profileKey}` ? translation : profileKey
})

onMounted(async () => {
  await homeStore.initializeData()
})

onUnmounted(() => {
  if (homeStore.stopProfileMonitoring) homeStore.stopProfileMonitoring()
  if (homeStore.stopDaemonMonitoring) homeStore.stopDaemonMonitoring()
})

function handleDonateClick() {
  const telegramUrl = 'https://t.me/mal6876'
  if (KernelSU.openWebsite) {
    KernelSU.openWebsite(telegramUrl)
  } else {
    window.open(telegramUrl, '_blank')
  }
}
</script>

<style scoped>
.custom-slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: var(--color-primary, #38bdf8);
  cursor: pointer;
  box-shadow: 0 0 8px rgba(0, 0, 0, 0.4);
}

.custom-slider::-moz-range-thumb {
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: var(--color-primary, #38bdf8);
  cursor: pointer;
  border: none;
  box-shadow: 0 0 8px rgba(0, 0, 0, 0.4);
}
</style>
