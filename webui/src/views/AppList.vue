<template>
  <div class="page applist-page h-full flex flex-col overflow-hidden bg-surface select-none">
    <div class="max-w-3xl mx-auto h-full flex flex-col w-full min-h-0">
      
      <!-- 1. HEADER, SEARCH BAR & BADGE SEMUA -->
      <div class="flex-none px-5 pt-10 pb-2 space-y-4">
        <!-- Title & Option Button -->
        <div class="flex justify-between items-center text-on-surface">
          <h1 class="text-2xl font-bold tracking-tight text-on-surface">{{ t('games_page.title', 'AppList') }}</h1>
          
          <button 
            @click="openGeneralSettings" 
            class="p-2 -mr-2 rounded-full text-on-surface-variant hover:text-on-surface hover:bg-surface-container-high transition-all active:scale-90 cursor-pointer"
            :title="t('common.settings', 'Pengaturan')"
          >
            <IconDots class="w-6 h-6 text-on-surface" />
          </button>
        </div>

        <!-- Search Bar & Indikator Jumlah (Mirip Foto 5039.jpg) -->
        <div class="space-y-3.5">
          <!-- Search Bar -->
          <div class="bg-surface-container-highest px-4 py-3.5 rounded-full flex items-center gap-3 transition-all">
            <SearchIcon class="text-on-surface-variant shrink-0 w-5 h-5" />
            <input 
              v-model="searchQuery" 
              type="text" 
              :placeholder="t('games_page.search_placeholder', 'Search apps...')"
              class="bg-transparent border-none outline-none text-on-surface placeholder-on-surface-variant w-full text-base font-medium" 
            />
            <button 
              v-if="searchQuery" 
              @click="searchQuery = ''"
              class="text-on-surface-variant hover:text-on-surface transition-colors cursor-pointer"
            >
              <CloseIcon class="w-5 h-5" />
            </button>
          </div>

          <!-- Indikator Total Aplikasi (Pill Biru Solid) -->
          <div class="flex items-center">
            <span class="px-4 py-1.5 rounded-full text-sm font-semibold bg-primary text-on-primary inline-block">
              {{ t('games_page.all', 'Semua') }} ({{ isLoading ? '...' : processedApps.length }})
            </span>
          </div>
        </div>
      </div>

      <!-- 2. MAIN SHEET CONTAINER -->
      <div class="flex-1 min-h-0 bg-surface-container-low rounded-t-[28px] overflow-hidden shadow-2xl flex flex-col mx-2 mt-2">
        
        <!-- Scrollable App List -->
        <div class="scrollbar-hidden pb-safe-nav flex-1 min-h-0 overflow-y-scroll p-4" ref="scrollContainer">
          
          <!-- STATE 1: LOADING SKELETON ANIMASI -->
          <div v-if="isLoading" class="space-y-2.5 pb-28 animate-pulse">
            <div 
              v-for="n in 8" 
              :key="n" 
              class="bg-surface-container-high/60 p-3.5 rounded-[24px] flex items-center gap-4 h-18"
            >
              <div class="w-12 h-12 bg-surface-container-highest rounded-[16px] shrink-0"></div>
              <div class="flex-1 space-y-2">
                <div class="w-1/2 h-3.5 bg-surface-container-highest rounded"></div>
                <div class="w-1/3 h-2.5 bg-surface-container-highest rounded"></div>
              </div>
              <div class="w-8 h-8 bg-surface-container-highest rounded-full shrink-0"></div>
            </div>
          </div>

          <!-- STATE 2: DATA APP LIST -->
          <div v-else class="pb-28 space-y-2.5">
            <div 
              v-for="app in displayedApps" 
              :key="app.packageName"
              @click="onAppClick(app)"
              class="group relative bg-surface-container-high/50 hover:bg-surface-container-high p-3.5 rounded-[24px] flex items-center gap-3.5 transition-all duration-200 cursor-pointer active:scale-[0.99] shadow-xs"
            >
              <!-- Kiri: Ikon Aplikasi -->
              <div class="w-12 h-12 rounded-[16px] bg-surface-container-highest flex items-center justify-center overflow-hidden shadow-sm shrink-0">
                <img 
                  :src="app.icon || `ksu://packages/${app.packageName}/icon`" 
                  loading="lazy" 
                  @error="handleImageError"
                  class="w-full h-full object-cover group-hover:scale-105 transition-transform" 
                  :alt="app.appName" 
                />
              </div>

              <!-- Tengah: Nama App & Package Name -->
              <div class="min-w-0 flex-1">
                <h3 class="text-sm font-semibold text-on-surface truncate group-hover:text-primary transition-colors leading-tight">
                  {{ app.appName || app.packageName }}
                </h3>
                <p 
                  v-if="app.appName && app.appName !== app.packageName"
                  class="text-xs text-on-surface-variant truncate mt-1 opacity-80"
                >
                  {{ app.packageName }}
                </p>
              </div>

              <!-- Kanan: Badge Status & Tombol Titik Tiga -->
              <div class="flex items-center gap-2 shrink-0">
                <span 
                  v-if="checkIsActive(app)"
                  class="px-2.5 py-1 rounded-full text-[10px] font-bold tracking-wide bg-primary/20 text-primary uppercase"
                >
                  {{ t('common.active', 'AKTIF') }}
                </span>

                <div class="w-8 h-8 rounded-full bg-surface-container-highest/60 flex items-center justify-center text-on-surface-variant group-hover:text-on-surface group-hover:bg-surface-container-highest transition-colors">
                  <svg xmlns="http://www.w3.org/2000/svg" class="w-4 h-4" viewBox="0 0 24 24" fill="currentColor">
                    <circle cx="12" cy="5" r="2" />
                    <circle cx="12" cy="12" r="2" />
                    <circle cx="12" cy="19" r="2" />
                  </svg>
                </div>
              </div>
            </div>

            <!-- Pesan Tidak Ada Aplikasi -->
            <div 
              v-if="displayedApps.length === 0"
              class="text-center py-12 text-on-surface-variant"
            >
              <p class="text-sm font-medium">{{ t('games_page.no_apps_found', 'Tidak ada aplikasi ditemukan') }}</p>
            </div>
          </div>

        </div>
      </div>

    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onActivated } from 'vue'
import { useRouter } from 'vue-router'
import { useI18n } from 'vue-i18n'
import { useAppListStore } from '@/stores/AppList'

import IconDots from '@/components/icons/IconDots.vue'
import SearchIcon from '@/components/icons/IconSearch.vue'
import CloseIcon from '@/components/icons/IconClose.vue'

const router = useRouter()
const { t } = useI18n()
const gamesStore = useAppListStore()

const isLoading = ref(true)
const searchQuery = ref('')

const checkIsActive = (app) => {
  if (!app) return false
  return app.isEnabled === true || app.isEnabled === 'true' || app.isEnabled === 1 || app.isEnabled === 'on' || app.isTweakEnabled === true
}

const checkIsGame = (app) => {
  if (!app) return false

  const pkg = (app.packageName || '').toLowerCase()
  const name = (app.appName || '').toLowerCase()

  if ((pkg.startsWith('com.google.') || pkg.startsWith('com.android.')) && !pkg.includes('games')) {
    return false
  }

  if (app.isGame === true || app.isGame === 'true' || app.isGame === 1 || 
      app.is_game === true || app.is_game === 'true' ||
      app.category === 'game' || app.category === 0 || app.appType === 'game') {
    return true
  }

  const gameKeywords = [
    'mobile.legends', 'pubg', 'freefire', 'genshin', 'honkai', 'starrail', 
    'clash', 'brawl', 'roblox', 'minecraft', 'codm', 'callofduty', 'garena', 
    'tencent', 'moonton', 'mihoyo', 'hoyoverse', 'supercell', 'riotgames', 
    'activision', 'ea.gp', 'netease', 'asphalt', 'subway', 'candycrush', 
    'efootball', 'fifa', 'pes', 'ragnarok', 'stumble', 'warthunder', 'aov', 
    'arenaofvalor', 'arknights', 'bluearchive'
  ]

  for (const kw of gameKeywords) {
    if (pkg.includes(kw) || name.includes(kw)) {
      return true
    }
  }

  return false
}

const refreshData = async (force = false) => {
  if (!gamesStore.userApps || gamesStore.userApps.length < 20 || force) {
    isLoading.value = true
  }

  try {
    if (gamesStore.initializeData) {
      await gamesStore.initializeData()
    } else if (gamesStore.loadGamelist) {
      await gamesStore.loadGamelist()
    }
  } catch (e) {
    console.warn('Gagal memuat gamelist:', e)
  } finally {
    isLoading.value = false
  }
}

onMounted(() => {
  refreshData(true)
})

onActivated(() => {
  refreshData()
})

// HIRARKI PRIORITAS URUTAN APLIKASI:
// 1. Aktif & Game (Score 4)
// 2. Aktif & Non-Game (Score 3)
// 3. Non-aktif & Game (Score 2)
// 4. Non-aktif & Non-Game (Score 1)
const processedApps = computed(() => {
  const list = [...(gamesStore.userApps || [])]

  return list.sort((a, b) => {
    const aGame = checkIsGame(a)
    const bGame = checkIsGame(b)
    const aActive = checkIsActive(a)
    const bActive = checkIsActive(b)

    const getScore = (isG, isAct) => {
      if (isAct && isG) return 4   // Prioritas 1: Aktif + Game
      if (isAct && !isG) return 3  // Prioritas 2: Aktif + App Biasa
      if (!isAct && isG) return 2  // Prioritas 3: Non-aktif + Game
      return 1                     // Prioritas 4: Non-aktif + App Biasa
    }

    const scoreA = getScore(aGame, aActive)
    const scoreB = getScore(bGame, bActive)

    if (scoreA !== scoreB) {
      return scoreB - scoreA
    }

    const nameA = (a.appName || a.packageName || '').toLowerCase()
    const nameB = (b.appName || b.packageName || '').toLowerCase()
    return nameA.localeCompare(nameB)
  })
})

const displayedApps = computed(() => {
  let apps = processedApps.value

  if (searchQuery.value.trim() !== '') {
    const q = searchQuery.value.toLowerCase()
    apps = apps.filter(app => 
      (app.appName && app.appName.toLowerCase().includes(q)) ||
      (app.packageName && app.packageName.toLowerCase().includes(q))
    )
  }

  return apps
})

const openGeneralSettings = () => {
  router.push('/tweaks/general')
}

const onAppClick = (app) => {
  router.push(`/applist/${app.packageName}`)
}

const handleImageError = (e) => {
  e.target.style.display = 'none'
}
</script>
