<template>
  <div class="page general-settings-page h-full flex flex-col overflow-hidden bg-surface font-sans select-none">
    <!-- Header Page Clean -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto px-5 pt-10 pb-3">
        <div class="flex items-center gap-4 text-on-surface">
          <button 
            @click="$router.back()" 
            class="p-1.5 -ml-1.5 rounded-full hover:bg-on-surface/10 transition-colors cursor-pointer"
          >
            <ArrowLeftIcon class="w-6 h-6 text-on-surface rtl:rotate-180" />
          </button>
          <h1 class="text-xl font-semibold">
            {{ isIndonesian ? 'Pengaturan' : 'Settings' }}
          </h1>
        </div>
      </div>
    </div>

    <!-- Content List Menu -->
    <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll">
      <div class="max-w-3xl mx-auto p-5 py-3">

        <div class="space-y-[2px]">
          <!-- 1. Bahasa (First) -->
          <SettingsCard
            :title="isIndonesian ? 'Bahasa' : 'Language'"
            position="first"
            :on-click="openLanguageView"
          >
            <template #icon>
              <LanguageIcon />
            </template>
          </SettingsCard>

          <!-- 2. Tampilan (Middle) -->
          <SettingsCard
            :title="isIndonesian ? 'Tampilan' : 'Appearance'"
            position="middle"
            :on-click="openAdvancedControlsView"
          >
            <template #icon>
              <PaletteIcon />
            </template>
          </SettingsCard>

          <!-- 3. Simpan Log (Middle) -->
          <SettingsCard
            :title="isIndonesian ? 'Simpan Log' : 'Save Log'"
            position="middle"
            :on-click="handleSaveLog"
          >
            <template #icon>
              <IconSaveIcon />
            </template>
          </SettingsCard>

          <!-- 4. Buat Pintasan (Last) -->
          <SettingsCard
            :title="isIndonesian ? 'Buat Pintasan' : 'Create Shortcut'"
            position="last"
            :on-click="handleCreateShortcut"
          >
            <template #icon>
              <HomeIcon />
            </template>
          </SettingsCard>
        </div>

      </div>
    </div>

    <!-- MODAL 1: MODAL SAVING LOG -->
    <UiModal
      :show="showLogModal"
      :title="modalTitle"
      :description="modalMessage"
      @close="showLogModal = false"
    >
      <template #actions>
        <button
          @click="showLogModal = false"
          class="px-6 py-2.5 rounded-full bg-primary text-on-primary text-sm font-semibold hover:opacity-90 active:scale-95 transition-all cursor-pointer"
        >
          OK
        </button>
      </template>
    </UiModal>

    <!-- MODAL 2: PANDUAN BUAT PINTASAN SHORTCUT (VISUAL STEP-BY-STEP) -->
    <div 
      v-if="showShortcutModal" 
      class="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/60 backdrop-blur-sm animate-fade-in"
    >
      <div class="bg-surface-container-low rounded-3xl p-5 w-full max-w-sm space-y-4 shadow-2xl">
        <div class="flex items-center justify-between">
          <h3 class="text-base font-bold text-on-surface">
            {{ isIndonesian ? 'Cara Tambah Pintasan' : 'How to Add Shortcut' }}
          </h3>
          <button 
            @click="showShortcutModal = false" 
            class="text-on-surface-variant hover:text-on-surface p-1 cursor-pointer"
          >
            <svg class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <line x1="18" y1="6" x2="6" y2="18"/>
              <line x1="6" y1="6" x2="18" y2="18"/>
            </svg>
          </button>
        </div>

        <!-- Kartu Tahapan -->
        <div class="space-y-2.5 text-xs text-on-surface-variant">
          <div class="flex items-start gap-3 bg-surface-container-high/60 p-3 rounded-2xl">
            <span class="w-5 h-5 rounded-full bg-primary text-on-primary font-bold flex items-center justify-center shrink-0 text-[11px]">1</span>
            <p class="text-on-surface leading-relaxed" v-if="isIndonesian">
              Buka aplikasi <b class="text-primary">KernelSU / APatch / MMRL</b> atau WebUI Standalone.
            </p>
            <p class="text-on-surface leading-relaxed" v-else>
              Open your <b class="text-primary">KernelSU / APatch / MMRL</b> or Standalone app.
            </p>
          </div>

          <div class="flex items-start gap-3 bg-surface-container-high/60 p-3 rounded-2xl">
            <span class="w-5 h-5 rounded-full bg-primary text-on-primary font-bold flex items-center justify-center shrink-0 text-[11px]">2</span>
            <p class="text-on-surface leading-relaxed" v-if="isIndonesian">
              Masuk ke tab <b class="text-primary">Modul</b> lalu tekan & tahan (Long-press) pada modul <b class="text-primary">LUMina Tweaks</b>.
            </p>
            <p class="text-on-surface leading-relaxed" v-else>
              Go to <b class="text-primary">Modules</b> tab then long-press on <b class="text-primary">LUMina Tweaks</b> card.
            </p>
          </div>

          <div class="flex items-start gap-3 bg-surface-container-high/60 p-3 rounded-2xl">
            <span class="w-5 h-5 rounded-full bg-primary text-on-primary font-bold flex items-center justify-center shrink-0 text-[11px]">3</span>
            <p class="text-on-surface leading-relaxed" v-if="isIndonesian">
              Pilih menu <b class="text-primary">"Buat Pintasan"</b> / <b class="text-primary">"Pin to Home Screen"</b>.
            </p>
            <p class="text-on-surface leading-relaxed" v-else>
              Select <b class="text-primary">"Create Shortcut"</b> or <b class="text-primary">"Pin to Home Screen"</b>.
            </p>
          </div>
        </div>

        <button 
          @click="showShortcutModal = false" 
          class="w-full py-2.5 rounded-2xl bg-primary text-on-primary font-semibold text-xs transition-all active:scale-95 cursor-pointer shadow-sm"
        >
          {{ isIndonesian ? 'Saya Paham' : 'Got It' }}
        </button>
      </div>
    </div>

  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useI18n } from 'vue-i18n'
import { useLanguageStore } from '@/stores/Language'
import { useLuminaConfigStore } from '@/stores/LuminaConfig'

import SettingsCard from '@/components/ui/SettingsCard.vue'
import UiModal from '@/components/ui/UiModal.vue'
import ArrowLeftIcon from '@/components/icons/IconArrowLeft.vue'
import LanguageIcon from '@/components/icons/IconLanguage.vue'
import PaletteIcon from '@/components/icons/IconPalette.vue'
import HomeIcon from '@/components/icons/IconHome.vue'
import IconSaveIcon from '@/components/icons/IconSave.vue'

import { exec } from '@/helpers/WebUIX'

const router = useRouter()
const { locale } = useI18n()
const languageStore = useLanguageStore()
const luminaConfigStore = useLuminaConfigStore()

const showLogModal = ref(false)
const showShortcutModal = ref(false)

const modalTitle = ref('LUMina Tweaks')
const modalMessage = ref('')

const isIndonesian = computed(() => {
  const currentLang = languageStore.currentLanguage || locale.value || localStorage.getItem('preferred-language') || 'id'
  return currentLang === 'id' || currentLang.startsWith('id')
})

onMounted(async () => {
  if (!luminaConfigStore.isLoaded && luminaConfigStore.loadConfig) {
    await luminaConfigStore.loadConfig()
  }
})

const openLanguageView = () => router.push('/tweaks/language')
const openAdvancedControlsView = () => router.push('/tweaks/advanced-controls')

const handleCreateShortcut = () => {
  showShortcutModal.value = true
}

const handleSaveLog = async () => {
  const now = new Date()
  const dateStr = now.toISOString().split('T')[0]
  const timeStr = now.toTimeString().split(' ')[0].replace(/:/g, '-')
  const fileName = `Lumina_Log_${dateStr}_${timeStr}.txt`
  const logPath = `/sdcard/${fileName}`

  // DIBERSIHKAN: Dibuat rapi tanpa dmesg & logcat
  const cmd = `
LOG="${logPath}"
{
  echo "=========================================="
  echo "       LUMINA TWEAKS DIAGNOSTICS          "
  echo "=========================================="
  echo "Waktu         : $(date)"
  echo "Brand / Model : $(getprop ro.product.brand) $(getprop ro.product.model) ($(getprop ro.product.device))"
  echo "Android Ver   : $(getprop ro.build.version.release) (SDK $(getprop ro.build.version.sdk))"
  echo "Kernel Ver    : $(uname -r)"
  echo "SELinux       : $(getenforce)"
  echo ""
  echo "=========================================="
  echo "            STATUS DAEMON & CORE          "
  echo "=========================================="
  PID=$(pgrep -f "luminad" || echo "")
  if [ -n "$PID" ]; then
    echo "Status Daemon   : AKTIF ✅ (PID: $PID)"
  else
    echo "Status Daemon   : MATI / CRASH ❌"
  fi
  echo ""
  echo "=========================================="
  echo "              CONFIG JSON                 "
  echo "=========================================="
  if [ -f /data/adb/.config/lumina/config.json ]; then
    cat /data/adb/.config/lumina/config.json
  else
    echo "File config.json tidak ditemukan."
  fi
  echo ""
  echo "=========================================="
  echo "          LUMINA SERVICE LOGS             "
  echo "=========================================="
  if [ -f /data/adb/modules/lumina/lumina.log ]; then
    cat /data/adb/modules/lumina/lumina.log
  else
    echo "Belum ada catatan log dari daemon (lumina.log)."
  fi
  echo "=========================================="
} > "$LOG"
chmod 666 "$LOG"
`.trim()

  try {
    await exec(cmd)
    modalTitle.value = 'LUMina Tweaks'
    modalMessage.value = isIndonesian.value
      ? `Log operasional berhasil disimpan di:\n${logPath}`
      : `Operational log successfully saved to:\n${logPath}`
  } catch (err) {
    modalTitle.value = isIndonesian.value ? 'Gagal' : 'Failed'
    modalMessage.value = (isIndonesian.value ? 'Terjadi kesalahan saat menyimpan log:\n' : 'An error occurred while saving log:\n') + (err?.message || err)
  } finally {
    showLogModal.value = true
  }
}
</script>
