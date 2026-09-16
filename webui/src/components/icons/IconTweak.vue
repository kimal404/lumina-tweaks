<template>
  <div class="page general-settings-page h-full flex flex-col overflow-hidden bg-surface">
    <!-- Header dengan Tombol Kembali -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto p-5 pb-3">
        <div class="flex items-center gap-4 text-on-surface">
          <button @click="$router.back()" class="hover:text-primary transition-colors">
            <ArrowLeftIcon class="w-6 h-6 cursor-pointer rtl:rotate-180" />
          </button>
          <h1 class="text-xl font-semibold">Pengaturan</h1>
        </div>
      </div>
    </div>

    <!-- Content: Card Pengaturan -->
    <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll">
      <div class="max-w-3xl mx-auto p-5 py-1 space-y-4">
        
        <!-- Bahasa -->
        <div class="md3-list">
          <RippleComponent @click="openLanguageView" class="md3-list-item" tabindex="0">
            <div class="flex items-center justify-between px-5 py-4">
              <div class="flex items-center gap-4 min-w-0 flex-1">
                <div class="w-10 h-10 rounded-full bg-primary-container flex items-center justify-center shrink-0">
                  <LanguageIcon class="w-5 h-5 text-on-primary-container" />
                </div>
                <div class="flex-1 min-w-0">
                  <h3 class="text-sm font-medium text-on-surface">{{ $t('settings_page.language.title') }}</h3>
                  <p class="text-xs text-on-surface-variant mt-1">{{ currentLanguage }}</p>
                </div>
              </div>
              <ChevronRightIcon class="text-on-surface-variant shrink-0 rtl:rotate-180" :size="22" />
            </div>
          </RippleComponent>
        </div>

        <!-- Mitigasi Perangkat -->
        <div class="md3-list">
          <div class="md3-list-item flex items-center justify-between px-5 py-4">
            <div class="flex items-center gap-4 min-w-0 flex-1">
              <div class="w-10 h-10 rounded-full bg-primary-container flex items-center justify-center shrink-0">
                <BugIcon class="w-5 h-5 text-on-primary-container" />
              </div>
              <div class="flex-1 min-w-0 pr-2">
                <h3 class="text-sm font-medium text-on-surface">{{ $t('settings_page.device_mitigation.title') }}</h3>
                <p class="text-xs text-on-surface-variant mt-1">{{ $t('settings_page.device_mitigation.description') }}</p>
              </div>
            </div>
            <UiToggle :model-value="isDeviceMitigationEnabled" @update:model-value="toggleDeviceMitigation" />
          </div>
        </div>

        <!-- Simpan Log -->
        <div class="md3-list">
          <RippleComponent @click="handleSaveLog" class="md3-list-item" tabindex="0">
            <div class="flex items-center justify-between px-5 py-4">
              <div class="flex items-center gap-4 min-w-0 flex-1">
                <div class="w-10 h-10 rounded-full bg-primary-container flex items-center justify-center shrink-0">
                  <IconSaveIcon class="w-5 h-5 text-on-primary-container" />
                </div>
                <div class="flex-1 min-w-0">
                  <h3 class="text-sm font-medium text-on-surface">{{ $t('settings_page.save_log.title') }}</h3>
                  <p class="text-xs text-on-surface-variant mt-1">{{ $t('settings_page.save_log.description') }}</p>
                </div>
              </div>
            </div>
          </RippleComponent>
        </div>

        <!-- Buat Pintasan -->
        <div class="md3-list">
          <RippleComponent @click="handleCreateShortcut" class="md3-list-item" tabindex="0">
            <div class="flex items-center justify-between px-5 py-4">
              <div class="flex items-center gap-4 min-w-0 flex-1">
                <div class="w-10 h-10 rounded-full bg-primary-container flex items-center justify-center shrink-0">
                  <HomePlusIcon class="w-5 h-5 text-on-primary-container" />
                </div>
                <div class="flex-1 min-w-0">
                  <h3 class="text-sm font-medium text-on-surface">{{ $t('settings_page.create_shortcut.title') }}</h3>
                  <p class="text-xs text-on-surface-variant mt-1">{{ $t('settings_page.create_shortcut.description') }}</p>
                </div>
              </div>
            </div>
          </RippleComponent>
        </div>

      </div>
    </div>
  </div>
</template>

<script setup>
import { computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useI18n } from 'vue-i18n'
import { useLanguageStore } from '@/stores/Language'
import { useLuminaConfigStore } from '@/stores/LuminaConfig'

import RippleComponent from '@/components/ui/UiRipple.vue'
import UiToggle from '@/components/ui/UiToggle.vue'
import ArrowLeftIcon from '@/components/icons/IconArrowLeft.vue'
import ChevronRightIcon from '@/components/icons/IconChevronRight.vue'
import LanguageIcon from '@/components/icons/IconLanguage.vue'
import BugIcon from '@/components/icons/IconError.vue'
import HomePlusIcon from '@/components/icons/IconHomePlus.vue'
import IconSaveIcon from '@/components/icons/IconSave.vue'

import * as KernelSU from '@/helpers/KernelSU'

const router = useRouter()
const { t } = useI18n()
const languageStore = useLanguageStore()
const luminaConfigStore = useLuminaConfigStore()

onMounted(async () => {
  if (!luminaConfigStore.isLoaded) {
    await luminaConfigStore.loadConfig()
  }
})

const isDeviceMitigationEnabled = computed(() => luminaConfigStore.isDeviceMitigationEnabled)
const toggleDeviceMitigation = async (val) => {
  await luminaConfigStore.setDeviceMitigation(val)
}

const currentLanguage = computed(() => {
  if (languageStore.userPreference === null) {
    return t('language_selection.follow_system')
  } else {
    const langCode = languageStore.userPreference
    const langData = languageStore.availableLanguages[langCode]
    return langData?.name || langCode
  }
})

const openLanguageView = () => router.push('/tweaks/language')

// --- FIX HANDLER CREATESHORTCUT (Bypass helper yang pemicu pop-up) ---
const handleCreateShortcut = async () => {
  try {
    if (typeof window !== 'undefined' && window.ksu && window.ksu.createShortcut) {
      await window.ksu.createShortcut()
    } else if (typeof KernelSU !== 'undefined' && KernelSU.createShortcut) {
      await KernelSU.createShortcut()
    } else {
      alert('Pintasan hanya didukung saat dijalankan via KernelSU Manager.')
    }
  } catch (err) {
    console.warn('Shortcut error:', err)
  }
}

// --- FIX HANDLER SAVE LOG (Langsung tembak API shell tanpa lewat trigger modal) ---
const handleSaveLog = async () => {
  const cmd = `LOG="/sdcard/Lumina_Debug.log"; echo "=== LUMINA TWEAKS DIAGNOSTIC LOG ===" > $LOG; echo "Date: $(date)" >> $LOG; echo "Device: $(getprop ro.product.model) ($(getprop ro.hardware))" >> $LOG; echo "Kernel: $(uname -r)" >> $LOG; echo "" >> $LOG; echo "=== DAEMON LOG ===" >> $LOG; if [ -f /data/adb/.config/lumina/daemon.log ]; then cat /data/adb/.config/lumina/daemon.log >> $LOG; else echo "daemon.log not found" >> $LOG; fi; chmod 666 $LOG`

  try {
    let executed = false

    // 1. Prioritas eksekusi via window.ksu
    if (typeof window !== 'undefined' && window.ksu && window.ksu.exec) {
      await window.ksu.exec(cmd)
      executed = true
    } 
    // 2. Fallback via window.exec (WebUI X / WebView umum)
    else if (typeof window !== 'undefined' && window.exec) {
      await window.exec(cmd)
      executed = true
    } 
    // 3. Fallback ke helper KernelSU
    else if (typeof KernelSU !== 'undefined' && KernelSU.exec) {
      await KernelSU.exec(cmd)
      executed = true
    }

    if (executed) {
      alert('Log berhasil disimpan ke:\n/sdcard/Lumina_Debug.log')
    } else {
      alert('Gagal mengeksekusi perintah shell. Bridge root tidak ditemukan.')
    }
  } catch (err) {
    console.error('Save log error:', err)
    alert('Terjadi kesalahan saat menyimpan log:\n' + (err?.message || err))
  }
}
</script>
