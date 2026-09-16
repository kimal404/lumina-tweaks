<template>
  <div class="page language-selection-page h-full flex flex-col overflow-hidden bg-surface select-none">
    <!-- Header Page Sticky (Backdrop Blur) -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto px-5 pt-10 pb-3">
        <div class="flex items-center gap-4 text-on-surface">
          <button 
            @click="goBack" 
            class="p-1.5 -ml-1.5 rounded-full hover:bg-on-surface/10 transition-colors cursor-pointer"
            :title="$t('common.back', 'Kembali')"
          >
            <ArrowLeftIcon class="w-6 h-6 text-on-surface rtl:rotate-180" />
          </button>
          <h1 class="text-xl font-semibold">
            {{ $t('settings_page.language.title', 'Bahasa') }}
          </h1>
        </div>
      </div>
    </div>

    <!-- Scrollable Content List -->
    <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll">
      <div class="max-w-3xl mx-auto p-5 py-3 space-y-3">

        <!-- KARTU UTAMA BAHASA (Gaya Card Rounded-28px Tema Baru) -->
        <div class="bg-surface-container-low p-2 rounded-[28px] space-y-1 shadow-sm">
          <div
            v-for="language in filteredAndSortedLanguages"
            :key="language.code"
            @click="selectLanguage(language.code)"
            class="px-4 py-3.5 rounded-[20px] cursor-pointer transition-all hover:bg-surface-container active:scale-[0.99] flex items-center justify-between"
          >
            <UiRadio
              :model-value="selectedLanguage"
              :value="language.code"
              :name="radioGroupName"
              :label="language.name"
              @update:model-value="selectLanguage"
              class="w-full cursor-pointer"
            />
          </div>
        </div>

      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useLanguageStore } from '@/stores/Language'
import { checkLanguageFile } from '@/helpers/Locales'

import ArrowLeftIcon from '@/components/icons/IconArrowLeft.vue'
import UiRadio from '@/components/ui/UiRadio.vue'

const router = useRouter()
const languageStore = useLanguageStore()

const radioGroupName = 'language-selection-group'
const languagesWithMissingFiles = ref([])

// Bind langsung ke bahasa aktif di Store (Default 'id' jika belum ada)
const selectedLanguage = computed(() => languageStore.currentLanguage || 'id')

const filteredAndSortedLanguages = computed(() => {
  const langArray = languageStore.getAvailableLanguages()
  const filtered = langArray.filter((lang) => !languagesWithMissingFiles.value.includes(lang.code))

  const english = filtered.find((lang) => lang.code === 'en')
  const others = filtered
    .filter((lang) => lang.code !== 'en')
    .sort((a, b) => a.code.localeCompare(b.code))

  return english ? [english, ...others] : others
})

onMounted(async () => {
  const allLanguages = languageStore.getAvailableLanguages()

  const checkPromises = allLanguages.map(async (lang) => {
    const hasFile = await checkLanguageFile(lang.code)
    if (!hasFile) {
      languagesWithMissingFiles.value.push(lang.code)
    }
    return { code: lang.code, hasFile }
  })

  await Promise.all(checkPromises)
})

async function selectLanguage(languageCode) {
  if (languagesWithMissingFiles.value.includes(languageCode)) {
    console.warn(`Cannot set language ${languageCode}: translation file missing`)
    return
  }

  const success = await languageStore.setLanguage(languageCode)
  if (success) {
    console.log(`Language successfully updated to: ${languageCode}`)
  }
}

function goBack() {
  router.back()
}
</script>
