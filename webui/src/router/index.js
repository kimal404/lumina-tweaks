import { createRouter, createWebHashHistory } from 'vue-router'

import Home from '../views/Home.vue'
import AppList from '../views/AppList.vue'
import Tweak from '../views/Tweak.vue'
import AppSettings from '../views/AppSettings.vue'
import GeneralSettings from '../views/GeneralSettings.vue'
import LanguageSelection from '../views/LanguageSelection.vue'

const routes = [
  // 1. Halaman Tab Utama
  { 
    path: '/', 
    name: 'Home', 
    component: Home, 
    alias: ['/home'] 
  },
  { 
    path: '/applist', 
    name: 'AppList', 
    component: AppList, 
    alias: ['/games', '/app-list', '/apps'] 
  },
  { 
    path: '/tweaks', 
    name: 'Tweak', 
    component: Tweak, 
    alias: ['/settings', '/tweak'] 
  },

  // 2. Sub-Halaman Tweaks (Hanya Lite Mode & Thermal)
  {
    path: '/tweaks/lite-mode',
    name: 'LiteMode',
    component: () => import('@/views/LiteMode.vue'),
    alias: ['/lite-mode', '/tweak/lite-mode', '/settings/lite-mode']
  },
  {
    path: '/tweaks/thermal',
    name: 'ThermalManagement',
    component: () => import('@/views/ThermalManagement.vue'),
    alias: ['/thermal', '/tweak/thermal', '/settings/thermal']
  },

  // 3. Detail Pengaturan Aplikasi
  { 
    path: '/app-settings/:package*', 
    name: 'AppSettings', 
    component: AppSettings, 
    alias: [
      '/applist/:package+', 
      '/games/:package+', 
      '/game-settings/:package+', 
      '/game/:package+'
    ] 
  },

  // 4. Pengaturan Umum, Tampilan / Banner & Bahasa
  { 
    path: '/general-settings', 
    name: 'GeneralSettings', 
    component: GeneralSettings, 
    alias: ['/tweaks/general', '/tweak/general', '/settings/general', '/general'] 
  },
  { 
    path: '/settings/appearance', 
    name: 'Appearance', 
    component: () => import('@/views/AdvancedControls.vue'),
    alias: ['/tweaks/appearance', '/tweak/appearance', '/appearance', '/tweaks/advanced-controls'] 
  },
  { 
    path: '/language', 
    name: 'LanguageSelection', 
    component: LanguageSelection, 
    alias: ['/tweaks/language', '/tweak/language', '/settings/language', '/language-selection'] 
  },

  // 5. Fallback Wildcard
  {
    path: '/:pathMatch(.*)*',
    redirect: '/'
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes,
})

export default router
