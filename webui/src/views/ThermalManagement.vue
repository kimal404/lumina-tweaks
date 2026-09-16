<template>
  <div 
    class="page thermal-page w-full min-h-screen h-screen overflow-y-scroll bg-surface pb-44 select-none"
    style="touch-action: pan-y !important; -webkit-overflow-scrolling: touch !important;"
  >
    <!-- Header Sticky -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto p-5 pb-3">
        <div class="flex items-center gap-3 text-on-surface">
          <button 
            @click="router.back()" 
            class="p-2 -ml-2 rounded-full text-on-surface hover:bg-on-surface/10 transition-colors cursor-pointer"
          >
            <svg class="w-6 h-6" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M19 12H5M12 19l-7-7 7-7"/>
            </svg>
          </button>
          <h1 class="text-xl font-semibold">Thermal Management</h1>
        </div>
      </div>
    </div>

    <!-- Content Area -->
    <div class="scrollbar-hidden max-w-3xl mx-auto p-5 py-3 space-y-3">
      
      <!-- Card Deskripsi / Informasi -->
      <div class="bg-surface-container-low rounded-[24px] p-5 flex flex-col items-start text-left">
        <div class="w-8 h-8 rounded-lg bg-surface-container-high text-primary flex items-center justify-center shrink-0 mb-3">
          <svg class="w-4 h-4 text-primary" viewBox="0 0 24 24" fill="currentColor">
            <path d="M12,0A12,12,0,1,0,24,12,12.013,12.013,0,0,0,12,0Zm0,22A10,10,0,1,1,22,12,10.011,10.011,0,0,1,12,22Z"/>
            <path d="M12,5a1,1,0,0,0-1,1v8a1,1,0,0,0,2,0V6A1,1,0,0,0,12,5Z"/>
            <rect x="11" y="17" width="2" height="2" rx="1"/>
          </svg>
        </div>
        
        <p class="text-xs text-on-surface-variant leading-relaxed">
          Matikan pembatasan suhu (thermal throttling) khusus saat bermain game untuk mempertahankan performa maksimal perangkat. Fitur ini otomatis tidak aktif saat mode seimbang atau santai.
        </p>
      </div>

      <!-- Card Toggle Disable Thermal -->
      <div class="bg-surface-container-low rounded-[24px] p-5 flex items-center justify-between gap-4">
        <!-- Icon & Text Sejajar -->
        <div class="flex items-center gap-3">
          <div class="w-8 h-8 rounded-lg bg-surface-container-high text-primary flex items-center justify-center shrink-0">
            <svg class="w-5 h-5 text-primary" viewBox="0 0 56 56" fill="currentColor" xmlns="http://www.w3.org/2000/svg">
              <path d="M 27.1918 8.3044 C 28.1918 8.3044 29.0179 7.4784 29.0179 6.5001 L 29.0179 1.8045 C 29.0179 .8045 28.1918 -7.1054e-15 27.1918 -7.105427357601002e-15 C 26.2136 -7.1054e-15 25.3875 .8045 25.3875 1.804462500447196 L 25.3875 6.5001 C 25.3875 7.4784 26.2136 8.3044 27.1918 8.3044 Z M 37.2353 12.5001 C 37.9092 13.1958 39.1048 13.2175 39.8222 12.5001 L 43.1266 9.1523 C 43.8222 8.4566 43.8222 7.3044 43.1266 6.6088 C 42.4309 5.9131 41.3005 5.9131 40.6048 6.6088 L 37.2353 9.9784 C 36.5396 10.6740 36.5614 11.8045 37.2353 12.5001 Z M 16.8440 56 C 23.1049 56 28.1918 50.9131 28.1918 44.6522 C 28.1918 41.3479 26.8223 38.4566 24.2353 36.1088 C 23.7571 35.6740 23.6483 35.4348 23.6483 34.7827 L 23.6701 32.6957 C 24.7788 33.1087 25.9745 33.3044 27.1918 33.3044 C 33.1701 33.3044 37.9527 28.5218 37.9527 22.5435 C 37.9527 16.5653 33.1701 11.7610 27.1918 11.7610 C 25.8440 11.7610 24.5397 12.0218 23.3005 12.5218 C 22.4092 9.5436 20.0397 7.6957 16.8440 7.6957 C 12.7353 7.6957 9.9745 10.7175 9.9745 15.2610 L 9.9962 34.7827 C 9.9962 35.4348 9.8875 35.6740 9.4310 36.1088 C 6.8223 38.4566 5.4745 41.3479 5.4745 44.6522 C 5.4745 50.9131 10.5397 56 16.8440 56 Z M 16.8440 52.8479 C 12.3223 52.8479 8.6484 49.1522 8.6484 44.6522 C 8.6484 41.9348 9.9310 39.4783 12.2353 37.9349 C 12.9093 37.4783 13.1701 37.0653 13.1701 36.1740 L 13.1701 15.3914 C 13.1701 12.6523 14.6701 10.8914 16.8440 10.8914 C 18.9962 10.8914 20.4744 12.6523 20.4744 15.3914 L 20.4744 36.1740 C 20.4744 37.0653 20.7353 37.4783 21.4092 37.9349 C 23.7136 39.4783 24.9962 41.9348 24.9962 44.6522 C 24.9962 49.1522 21.3440 52.8479 16.8440 52.8479 Z M 27.1918 14.9349 C 31.4527 14.9349 34.7570 18.2610 34.7570 22.5435 C 34.7570 26.8044 31.4527 30.1305 27.1918 30.1305 C 25.9527 30.1305 24.7353 29.8044 23.6701 29.2392 L 23.6918 15.8914 C 24.7571 15.2827 25.9527 14.9349 27.1918 14.9349 Z M 16.8223 49.9131 C 19.7353 49.9131 22.0831 47.5435 22.0831 44.6305 C 22.0831 42.5870 20.9309 40.9131 19.2571 40.0000 C 18.5614 39.6305 18.3223 39.3696 18.3223 38.3044 L 18.3223 17.2175 C 18.3223 16.0871 17.6701 15.3914 16.8223 15.3914 C 15.9962 15.3914 15.3223 16.0871 15.3223 17.2175 L 15.3223 38.3044 C 15.3223 39.3696 15.0832 39.6305 14.3875 40.0000 C 12.7136 40.9131 11.5614 42.5870 11.5614 44.6305 C 11.5614 47.5652 13.9092 49.9131 16.8223 49.9131 Z M 43.1700 24.3479 L 47.8875 24.3479 C 48.8656 24.3479 49.6701 23.5218 49.6701 22.5435 C 49.6701 21.5436 48.8656 20.7392 47.8875 20.7392 L 43.1700 20.7392 C 42.2135 20.7392 41.3874 21.5436 41.3874 22.5435 C 41.3874 23.5218 42.2135 24.3479 43.1700 24.3479 Z M 40.6048 38.5001 C 41.3005 39.1957 42.4309 39.1740 43.1266 38.4783 C 43.8222 37.7827 43.8222 36.6305 43.1266 35.9566 L 39.7787 32.6088 C 39.0831 31.9348 37.9309 31.9131 37.2353 32.6088 C 36.5396 33.3044 36.5396 34.4566 37.2353 35.1522 Z"></path>
            </svg>
          </div>
          <h3 class="text-sm font-semibold text-on-surface">Disable Thermal</h3>
        </div>

        <!-- M3 Switch Toggle -->
        <button 
          type="button"
          role="switch"
          :aria-checked="disableThermal"
          @click="toggleDisableThermal"
          class="w-[52px] h-8 rounded-full transition-colors duration-200 cursor-pointer relative shrink-0 border-2 flex items-center px-0.5 select-none"
          :class="disableThermal ? 'bg-primary border-primary' : 'bg-surface-container-highest border-outline/50'"
        >
          <div 
            class="w-6 h-6 rounded-full flex items-center justify-center transition-transform duration-200 ease-in-out shadow-sm"
            :class="disableThermal ? 'translate-x-5 bg-on-primary text-primary' : 'translate-x-0 bg-outline text-surface'"
          >
            <!-- X Icon (Off) -->
            <svg v-if="!disableThermal" class="w-3.5 h-3.5 stroke-current" viewBox="0 0 24 24" fill="none" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
              <line x1="18" y1="6" x2="6" y2="18"></line>
              <line x1="6" y1="6" x2="18" y2="18"></line>
            </svg>

            <!-- Check Icon (On) -->
            <svg v-else class="w-3.5 h-3.5 stroke-current" viewBox="0 0 24 24" fill="none" stroke-width="3.5" stroke-linecap="round" stroke-linejoin="round">
              <polyline points="20 6 9 17 4 12"></polyline>
            </svg>
          </div>
        </button>
      </div>

    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { useRouter } from 'vue-router'
import { useLuminaConfigStore } from '@/stores/LuminaConfig'
import { exec } from '@/utils/exec'

const router = useRouter()
const luminaConfigStore = useLuminaConfigStore()
const disableThermal = ref(false)

const syncStateFromStorage = async () => {
  if (!luminaConfigStore.isLoaded) {
    await luminaConfigStore.loadConfig()
  }
  disableThermal.value = Boolean(
    luminaConfigStore.isDisableThermalEnabled ?? luminaConfigStore.config?.disable_thermal
  )
}

onMounted(async () => {
  await syncStateFromStorage()
})

watch(
  () => luminaConfigStore.isDisableThermalEnabled,
  (newVal) => {
    disableThermal.value = Boolean(newVal)
  }
)

const toggleDisableThermal = async () => {
  const nextVal = !disableThermal.value
  disableThermal.value = nextVal
  
  // 1. Simpan ke Store (menulis ke config.json)
  if (luminaConfigStore.setDisableThermal) {
    await luminaConfigStore.setDisableThermal(nextVal)
  } else if (luminaConfigStore.updateConfig) {
    await luminaConfigStore.updateConfig({ disable_thermal: nextVal })
  }

  // 2. Tembak CLI daemon secara instan
  try {
    await exec(`/data/adb/modules/lumina/lumina set_disable_thermal ${nextVal ? 1 : 0} 2>/dev/null`)
  } catch (e) {
    console.warn('Gagal memicu set_disable_thermal CLI:', e)
  }
}
</script>

<style scoped>
.thermal-page {
  touch-action: pan-y !important;
  -webkit-overflow-scrolling: touch !important;
  overscroll-behavior-y: contain;
  overflow-y: scroll !important;
}
</style>
