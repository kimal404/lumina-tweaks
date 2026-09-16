<template>
  <div class="page game-settings-page h-full flex flex-col overflow-hidden bg-surface select-none">
    <!-- Header Page -->
    <div class="sticky top-0 z-10 bg-surface/80 backdrop-blur-md">
      <div class="max-w-3xl mx-auto px-5 pt-10 pb-3">
        <div class="flex items-center justify-between text-on-surface">
          <div class="flex items-center gap-4">
            <button 
              @click="$router.back()" 
              class="p-1.5 -ml-1.5 rounded-full hover:bg-on-surface/10 transition-colors cursor-pointer"
            >
              <ArrowLeftIcon class="w-6 h-6 text-on-surface rtl:rotate-180" />
            </button>
            <h1 class="text-xl font-semibold">
              {{ isIndonesian ? 'Pengaturan aplikasi' : 'App settings' }}
            </h1>
          </div>
          
          <button 
            @click="$router.push('/tweaks/general')" 
            class="p-2 -mr-2 rounded-full text-on-surface hover:bg-on-surface/10 transition-colors cursor-pointer"
            :title="isIndonesian ? 'Pengaturan' : 'Settings'"
          >
            <DotsVertical class="w-6 h-6 text-on-surface" />
          </button>
        </div>
      </div>
    </div>

    <!-- Scrollable Content -->
    <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll">
      <div class="max-w-3xl mx-auto p-5 py-2 space-y-4">

        <!-- 1. HERO APP HEADER CARD -->
        <div class="bg-surface-container-low p-5 rounded-[28px] flex items-center justify-between shadow-sm">
          <div class="flex items-center gap-4 min-w-0 flex-1">
            <div class="w-16 h-16 rounded-[20px] shrink-0 overflow-hidden shadow-sm bg-surface-container-highest flex items-center justify-center">
              <img 
                v-if="appIconSrc && !iconError"
                :src="appIconSrc" 
                @error="handleImageError"
                class="w-full h-full object-cover" 
                :alt="appName" 
              />
              <span v-else class="text-xl font-bold text-on-surface-variant select-none">
                {{ appName.charAt(0).toUpperCase() }}
              </span>
            </div>

            <div class="min-w-0 flex-1 pr-2">
              <h2 class="text-base font-bold text-on-surface truncate leading-tight">
                {{ appName }}
              </h2>
              <p class="text-xs text-on-surface-variant truncate mt-1">
                {{ packageName || (isIndonesian ? 'Package name tidak ditemukan' : 'Package name not found') }}
              </p>
              
              <div class="mt-2.5 flex items-center gap-2">
                <span 
                  :class="[
                    'px-3 py-1 rounded-full text-xs font-semibold transition-all',
                    isTweakEnabled 
                      ? 'bg-primary/20 text-primary' 
                      : 'bg-surface-container-highest text-on-surface-variant'
                  ]"
                >
                  {{ isTweakEnabled 
                    ? (isIndonesian ? 'Tweak Aktif' : 'Tweak Active') 
                    : (isIndonesian ? 'Tweak Nonaktif' : 'Tweak Inactive') }}
                </span>
              </div>
            </div>
          </div>
        </div>

        <!-- 2. MASTER SWITCH CARD -->
        <div 
          :class="[
            'p-5 rounded-[28px] transition-all duration-300 flex items-center justify-between shadow-sm',
            isTweakEnabled 
              ? 'bg-primary-container/30' 
              : 'bg-surface-container-low'
          ]"
        >
          <div class="flex items-center gap-4 min-w-0 flex-1">
            <div 
              :class="[
                'w-12 h-12 rounded-2xl flex items-center justify-center shrink-0 transition-colors shadow-sm',
                isTweakEnabled ? 'bg-primary text-on-primary' : 'bg-surface-container-high text-on-surface-variant'
              ]"
            >
              <TuneIcon class="w-6 h-6" />
            </div>

            <div class="flex-1 min-w-0 pr-2">
              <h3 class="text-sm font-bold text-on-surface">
                {{ isIndonesian ? 'Aktifkan tweaks' : 'Enable tweaks' }}
              </h3>
              <p class="text-xs text-on-surface-variant mt-0.5 leading-relaxed">
                {{ isIndonesian ? 'Terapkan optimasi profil performa khusus' : 'Apply custom performance profile optimization' }}
              </p>
            </div>
          </div>

          <!-- M3 Switch: Master Tweak -->
          <button 
            type="button"
            role="switch"
            :aria-checked="isTweakEnabled"
            @click="toggleAppTweak(!isTweakEnabled)"
            class="w-[52px] h-8 rounded-full transition-colors duration-200 cursor-pointer relative shrink-0 border-2 flex items-center px-0.5 select-none ms-3"
            :class="isTweakEnabled ? 'bg-primary border-primary' : 'bg-surface-container-highest border-outline/50'"
          >
            <div 
              class="w-6 h-6 rounded-full flex items-center justify-center transition-transform duration-200 ease-in-out shadow-sm"
              :class="isTweakEnabled ? 'translate-x-5 bg-on-primary text-primary' : 'translate-x-0 bg-outline text-surface'"
            >
              <!-- X Icon (Off) -->
              <svg v-if="!isTweakEnabled" class="w-3.5 h-3.5 stroke-current" viewBox="0 0 24 24" fill="none" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
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

        <!-- 3. PREFERENSI OPSIONAL -->
        <div class="space-y-3 pt-2">
          <h3 class="text-xs font-bold text-primary tracking-wider px-2">
            {{ isIndonesian ? 'Preferensi' : 'Preferences' }}
          </h3>

          <div 
            :class="[
              'bg-surface-container-low p-2 rounded-[28px] space-y-1 shadow-sm transition-all duration-300',
              !isTweakEnabled ? 'opacity-40 pointer-events-none grayscale-[30%]' : 'opacity-100'
            ]"
          >
            <!-- Mode Lite Item -->
            <div 
              @click="openLiteModeModal"
              class="p-3.5 rounded-[20px] flex items-center justify-between hover:bg-surface-container transition-colors cursor-pointer active:scale-[0.99]"
            >
              <div class="flex items-center gap-4 min-w-0 flex-1">
                <div class="w-10 h-10 rounded-xl bg-surface-container-high flex items-center justify-center shrink-0 text-primary shadow-xs">
                  <FeatherIcon class="w-5 h-5" />
                </div>

                <div class="flex-1 min-w-0 pr-2">
                  <h4 class="text-sm font-bold text-on-surface">
                    {{ isIndonesian ? 'Mode lite' : 'Lite mode' }}
                  </h4>
                  <p class="text-xs text-on-surface-variant mt-0.5 leading-relaxed">
                    {{ isIndonesian ? 'Prioritaskan baterai daripada performa' : 'Prioritize battery over peak performance' }}
                  </p>
                </div>
              </div>

              <!-- Selector Status -->
              <div class="bg-surface-container-high text-on-surface text-xs font-semibold px-3.5 py-2 rounded-xl flex items-center gap-1.5 shrink-0 ms-3 shadow-xs">
                <span>{{ currentLiteModeLabel }}</span>
                <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4 text-on-surface-variant" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <polyline points="6 9 12 15 18 9"/>
                </svg>
              </div>
            </div>

            <!-- Jangan Ganggu (DND) Item -->
            <div class="p-3.5 rounded-[20px] flex items-center justify-between hover:bg-surface-container transition-colors active:scale-[0.99]">
              <div class="flex items-center gap-4 min-w-0 flex-1">
                <div class="w-10 h-10 rounded-xl bg-surface-container-high flex items-center justify-center shrink-0 text-primary shadow-xs">
                  <svg xmlns="http://www.w3.org/2000/svg" class="w-5 h-5" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <circle cx="12" cy="12" r="10"></circle>
                    <line x1="8" y1="12" x2="16" y2="12"></line>
                  </svg>
                </div>

                <div class="flex-1 min-w-0 pr-2">
                  <h4 class="text-sm font-bold text-on-surface">
                    {{ isIndonesian ? 'Jangan ganggu' : 'Do not disturb' }}
                  </h4>
                  <p class="text-xs text-on-surface-variant mt-0.5 leading-relaxed">
                    {{ isIndonesian ? 'Blokir notifikasi & panggilan saat dibuka' : 'Block notifications & calls while using' }}
                  </p>
                </div>
              </div>

              <!-- M3 Switch: DND -->
              <button 
                type="button"
                role="switch"
                :aria-checked="isDndEnabled"
                @click="toggleDnd(!isDndEnabled)"
                class="w-[52px] h-8 rounded-full transition-colors duration-200 cursor-pointer relative shrink-0 border-2 flex items-center px-0.5 select-none ms-3"
                :class="isDndEnabled ? 'bg-primary border-primary' : 'bg-surface-container-highest border-outline/50'"
              >
                <div 
                  class="w-6 h-6 rounded-full flex items-center justify-center transition-transform duration-200 ease-in-out shadow-sm"
                  :class="isDndEnabled ? 'translate-x-5 bg-on-primary text-primary' : 'translate-x-0 bg-outline text-surface'"
                >
                  <!-- X Icon (Off) -->
                  <svg v-if="!isDndEnabled" class="w-3.5 h-3.5 stroke-current" viewBox="0 0 24 24" fill="none" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
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

      </div>
    </div>

    <!-- 4. CUSTOM POP-UP MODAL -->
    <Teleport to="body">
      <Transition name="modal-fade">
        <div 
          v-if="isModalOpen" 
          class="fixed inset-0 z-50 flex items-center justify-center p-5 bg-black/60 backdrop-blur-sm"
          @click.self="isModalOpen = false"
        >
          <div class="bg-surface-container-low rounded-[28px] p-6 w-full max-w-xs shadow-2xl space-y-4 animate-scale-up">
            <h3 class="text-base font-bold text-on-surface px-1">
              {{ isIndonesian ? 'Pilih mode lite' : 'Select lite mode' }}
            </h3>

            <div class="space-y-1.5">
              <button
                v-for="opt in liteModeOptions"
                :key="opt.value"
                @click="selectOption(opt.value)"
                :class="[
                  'w-full text-left px-4 py-3.5 rounded-2xl flex items-center justify-between transition-all text-sm font-medium cursor-pointer',
                  liteModeValue === opt.value
                    ? 'bg-primary text-on-primary font-bold shadow-md'
                    : 'text-on-surface hover:bg-surface-container-highest/60'
                ]"
              >
                <span>{{ opt.label }}</span>
                
                <svg 
                  v-if="liteModeValue === opt.value" 
                  xmlns="http://www.w3.org/2000/svg" 
                  class="w-5 h-5 text-on-primary shrink-0 ms-2" 
                  viewBox="0 0 24 24" 
                  fill="none" 
                  stroke="currentColor" 
                  stroke-width="2.5" 
                  stroke-linecap="round" 
                  stroke-linejoin="round"
                >
                  <polyline points="20 6 9 17 4 12"/>
                </svg>
              </button>
            </div>
          </div>
        </div>
      </Transition>
    </Teleport>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useAppListStore } from '@/stores/AppList'
import { useLanguageStore } from '@/stores/Language'
import { useI18n } from 'vue-i18n'

import ArrowLeftIcon from '@/components/icons/IconArrowLeft.vue'
import DotsVertical from '@/components/icons/IconDots.vue'
import FeatherIcon from '@/components/icons/IconTune.vue'
import TuneIcon from '@/components/icons/IconTune.vue'

const route = useRoute()
const router = useRouter()
const gamesStore = useAppListStore()
const languageStore = useLanguageStore()
const { locale } = useI18n()

const iconError = ref(false)

const isIndonesian = computed(() => {
  const currentLang = languageStore.currentLanguage || locale.value || localStorage.getItem('preferred-language') || 'id'
  return currentLang === 'id' || currentLang.startsWith('id')
})

const packageName = computed(() => {
  const param = route.params.packageName || route.params.id || route.params.pkg || route.params.name
  if (param) return decodeURIComponent(param)

  const parts = route.path.split('/').filter(Boolean)
  if (parts.length > 0 && parts[parts.length - 1] !== 'applist') {
    return decodeURIComponent(parts[parts.length - 1])
  }
  return ''
})

const currentApp = computed(() => {
  if (!packageName.value || !gamesStore.userApps) return null
  return gamesStore.userApps.find(a => 
    a.packageName === packageName.value || 
    a.id === packageName.value
  ) || null
})

const appName = computed(() => {
  return currentApp.value?.appName || currentApp.value?.label || packageName.value || (isIndonesian.value ? 'Aplikasi' : 'Application')
})

const appIconSrc = computed(() => {
  if (currentApp.value?.icon) return currentApp.value.icon
  if (packageName.value) return `ksu://packages/${packageName.value}/icon`
  return ''
})

onMounted(async () => {
  if (!gamesStore.userApps || gamesStore.userApps.length === 0) {
    if (gamesStore.initializeData) {
      await gamesStore.initializeData()
    } else if (gamesStore.loadGamelist) {
      await gamesStore.loadGamelist()
    }
  }
})

const localTweakEnabled = ref(null)
const localLiteMode = ref(null)
const localDndEnabled = ref(null)

const isTweakEnabled = computed(() => {
  if (localTweakEnabled.value !== null) return localTweakEnabled.value
  if (!currentApp.value) return false
  return Boolean(currentApp.value.isEnabled ?? currentApp.value.isTweakEnabled ?? false)
})

const liteModeValue = computed(() => {
  if (localLiteMode.value !== null) return localLiteMode.value
  if (!currentApp.value) return 'default'
  return currentApp.value.liteMode ?? 'default'
})

const isDndEnabled = computed(() => {
  if (localDndEnabled.value !== null) return localDndEnabled.value
  if (!currentApp.value) return false
  return Boolean(currentApp.value.isDndEnabled ?? currentApp.value.dnd ?? false)
})

const isModalOpen = ref(false)

const liteModeOptions = computed(() => [
  { value: 'default', label: isIndonesian.value ? 'Bawaan' : 'Default' },
  { value: 'on', label: isIndonesian.value ? 'Aktif' : 'Enabled' },
  { value: 'off', label: isIndonesian.value ? 'Nonaktif' : 'Disabled' }
])

const currentLiteModeLabel = computed(() => {
  const selected = liteModeOptions.value.find(o => o.value === liteModeValue.value)
  return selected ? selected.label : (isIndonesian.value ? 'Bawaan' : 'Default')
})

const openLiteModeModal = () => {
  if (isTweakEnabled.value) {
    isModalOpen.value = true
  }
}

const selectOption = async (val) => {
  await changeLiteMode(val)
  isModalOpen.value = false
}

const toggleAppTweak = async (val) => {
  if (!packageName.value) return
  localTweakEnabled.value = val
  if (currentApp.value) currentApp.value.isEnabled = val

  try {
    if (gamesStore.toggleAppTweak) {
      await gamesStore.toggleAppTweak(packageName.value, val)
    }
  } catch (err) {
    console.warn('Gagal menyimpan state tweak:', err)
  }
}

const changeLiteMode = async (val) => {
  if (!packageName.value) return
  localLiteMode.value = val
  if (currentApp.value) currentApp.value.liteMode = val

  try {
    if (gamesStore.setAppLiteMode) {
      await gamesStore.setAppLiteMode(packageName.value, val)
    }
  } catch (err) {
    console.warn('Gagal menyimpan lite mode:', err)
  }
}

const toggleDnd = async (val) => {
  if (!packageName.value) return
  localDndEnabled.value = val
  if (currentApp.value) currentApp.value.isDndEnabled = val

  try {
    if (gamesStore.toggleAppDnd) {
      await gamesStore.toggleAppDnd(packageName.value, val)
    }
  } catch (err) {
    console.warn('Gagal menyimpan DND:', err)
  }
}

const handleImageError = () => {
  iconError.value = true
}
</script>

<style scoped>
.modal-fade-enter-active,
.modal-fade-leave-active {
  transition: opacity 0.2s ease;
}

.modal-fade-enter-from,
.modal-fade-leave-to {
  opacity: 0;
}

@keyframes scaleUp {
  from {
    transform: scale(0.92);
    opacity: 0;
  }
  to {
    transform: scale(1);
    opacity: 1;
  }
}

.animate-scale-up {
  animation: scaleUp 0.2s cubic-bezier(0.16, 1, 0.3, 1) forwards;
}
</style>
