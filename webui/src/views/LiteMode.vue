<template>
  <div 
    class="page lite-mode-page w-full min-h-screen h-screen overflow-y-scroll bg-surface pb-44 select-none"
    style="touch-action: pan-y !important; -webkit-overflow-scrolling: touch !important;"
  >
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
          <h1 class="text-xl font-semibold">Mode Lite</h1>
        </div>
      </div>
    </div>

    <div class="scrollbar-hidden max-w-3xl mx-auto p-5 py-3 space-y-3">
      <div class="bg-surface-container-low rounded-[24px] p-5 flex flex-col items-start text-left">
        <div class="w-8 h-8 rounded-lg bg-surface-container-high text-primary flex items-center justify-center shrink-0 mb-3">
          <svg class="w-4 h-4 text-primary" viewBox="0 0 24 24" fill="currentColor">
            <path d="M12,0A12,12,0,1,0,24,12,12.013,12.013,0,0,0,12,0Zm0,22A10,10,0,1,1,22,12,10.011,10.011,0,0,1,12,22Z"/>
            <path d="M12,5a1,1,0,0,0-1,1v8a1,1,0,0,0,2,0V6A1,1,0,0,0,12,5Z"/>
            <rect x="11" y="17" width="2" height="2" rx="1"/>
          </svg>
        </div>
        
        <p class="text-xs text-on-surface-variant leading-relaxed">
          Mode Lite membantu mengoptimalkan penggunaan memori dan baterai dengan menyeimbangkan clockspeed serta beban GPU saat beroperasi.
        </p>
      </div>

      <!-- Card Mode Lite dengan Switch MD3 Icon -->
      <div class="bg-surface-container-low rounded-[24px] p-5 flex items-center justify-between gap-4">
        <div class="flex items-center gap-3">
          <div class="w-8 h-8 rounded-lg bg-surface-container-high text-primary flex items-center justify-center shrink-0">
            <svg class="w-5 h-5 text-primary" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path d="M10.5026 5.01692L9.96661 3.65785C9.62068 2.78072 8.37933 2.78072 8.03339 3.65784L6.96137 6.37599C6.85576 6.64378 6.64378 6.85575 6.37599 6.96137L3.65785 8.03339C2.78072 8.37932 2.78072 9.62067 3.65784 9.96661L6.37599 11.0386C6.64378 11.1442 6.85575 11.3562 6.96137 11.624L8.03339 14.3422C8.37932 15.2193 9.62067 15.2193 9.96661 14.3422L11.0386 11.624C11.1442 11.3562 11.3562 11.1442 11.624 11.0386L14.3422 9.96661C15.2193 9.62068 15.2193 8.37933 14.3422 8.03339L12.9831 7.49738" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"></path>
              <path d="M16.4885 13.3481C16.6715 12.884 17.3285 12.884 17.5115 13.3481L18.3121 15.3781C18.368 15.5198 18.4802 15.632 18.6219 15.6879L20.6519 16.4885C21.116 16.6715 21.116 17.3285 20.6519 17.5115L18.6219 18.3121C18.4802 18.368 18.368 18.4802 18.3121 18.6219L17.5115 20.6519C17.3285 21.116 16.6715 21.116 16.4885 20.6519L15.6879 18.6219C15.632 18.4802 15.5198 18.368 15.3781 18.3121L13.3481 17.5115C12.884 17.3285 12.884 16.6715 13.3481 16.4885L15.3781 15.6879C15.5198 15.632 15.632 15.5198 15.6879 15.3781L16.4885 13.3481Z" stroke="currentColor" stroke-width="1.5"></path>
            </svg>
          </div>
          <h3 class="text-sm font-semibold text-on-surface">Mode Lite</h3>
        </div>

        <!-- M3 Switch Toggle -->
        <button 
          type="button"
          role="switch"
          :aria-checked="isLiteMode"
          @click="toggleLiteMode"
          class="w-[52px] h-8 rounded-full transition-colors duration-200 cursor-pointer relative shrink-0 border-2 flex items-center px-0.5 select-none"
          :class="isLiteMode ? 'bg-primary border-primary' : 'bg-surface-container-highest border-outline/50'"
        >
          <div 
            class="w-6 h-6 rounded-full flex items-center justify-center transition-transform duration-200 ease-in-out shadow-sm"
            :class="isLiteMode ? 'translate-x-5 bg-on-primary text-primary' : 'translate-x-0 bg-outline text-surface'"
          >
            <svg v-if="!isLiteMode" class="w-3.5 h-3.5 stroke-current" viewBox="0 0 24 24" fill="none" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
              <line x1="18" y1="6" x2="6" y2="18"></line>
              <line x1="6" y1="6" x2="18" y2="18"></line>
            </svg>
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
const isLiteMode = ref(false)

const syncStateFromStorage = async () => {
  if (!luminaConfigStore.isLoaded) {
    await luminaConfigStore.loadConfig()
  }
  isLiteMode.value = Boolean(
    luminaConfigStore.isLiteModeEnabled ?? luminaConfigStore.config?.enforce_lite_mode ?? luminaConfigStore.config?.lite_mode
  )
}

onMounted(async () => {
  await syncStateFromStorage()
})

watch(
  () => luminaConfigStore.isLiteModeEnabled,
  (newVal) => {
    isLiteMode.value = Boolean(newVal)
  }
)

const toggleLiteMode = async () => {
  const nextVal = !isLiteMode.value
  isLiteMode.value = nextVal
  
  // 1. Simpan ke Store (menulis ke config.json)
  if (luminaConfigStore.setLiteMode) {
    await luminaConfigStore.setLiteMode(nextVal)
  } else if (luminaConfigStore.updateConfig) {
    await luminaConfigStore.updateConfig({ enforce_lite_mode: nextVal })
  }

  // 2. Tembak backend langsung via CLI agar daemon merespon seketika
  try {
    await exec(`/data/adb/modules/lumina/lumina set_lite_mode ${nextVal ? 1 : 0} 2>/dev/null`)
  } catch (e) {
    console.warn('Gagal memicu set_lite_mode CLI:', e)
  }
}
</script>

<style scoped>
.lite-mode-page {
  touch-action: pan-y !important;
  -webkit-overflow-scrolling: touch !important;
  overscroll-behavior-y: contain;
  overflow-y: scroll !important;
}
</style>
