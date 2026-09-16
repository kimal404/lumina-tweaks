import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import * as KernelSU from '@/helpers/KernelSU'

const MOCK_APPS = [
  {
    packageName: 'com.mobile.legends',
    appName: 'Mobile Legends: Bang Bang',
    icon: 'https://raw.githubusercontent.com/google/material-design-icons/master/png/action/android/2x/baseline_android_black_48dp.png',
  },
  {
    packageName: 'com.miHoYo.GenshinImpact',
    appName: 'Genshin Impact',
    icon: 'https://raw.githubusercontent.com/google/material-design-icons/master/png/action/android/2x/baseline_android_black_48dp.png',
  },
  {
    packageName: 'com.whatsapp',
    appName: 'WhatsApp',
    icon: 'https://raw.githubusercontent.com/google/material-design-icons/master/png/action/android/2x/baseline_android_black_48dp.png',
  },
  {
    packageName: 'com.android.chrome',
    appName: 'Google Chrome',
    icon: 'https://raw.githubusercontent.com/google/material-design-icons/master/png/action/android/2x/baseline_android_black_48dp.png',
  },
]

export const useAppListStore = defineStore('games', () => {
  const userApps = ref([])
  const searchQuery = ref('')
  const isLoading = ref(false)
  const gamelistConfig = ref({})

  const isAppEnabled = (packageName) => packageName in gamelistConfig.value

  // Computed property untuk aplikasi yang difilter
  const filteredApps = computed(() => {
    let apps = userApps.value

    if (searchQuery.value.trim()) {
      const q = searchQuery.value.toLowerCase()
      apps = apps.filter(
        (a) => a.appName?.toLowerCase().includes(q) || a.packageName?.toLowerCase().includes(q),
      )
    }

    const [enabled, disabled] = [
      apps.filter((a) => isAppEnabled(a.packageName)),
      apps.filter((a) => !isAppEnabled(a.packageName)),
    ]

    const sortByName = (a, b) =>
      (a.appName || a.packageName).localeCompare(b.appName || b.packageName)

    return [...enabled.sort(sortByName), ...disabled.sort(sortByName)]
  })

  // Mengubah path config ke lokasi folder LUMina
  const configPath = '/data/adb/.config/lumina/gamelist.json'

  async function loadGamelistConfig() {
    try {
      if (await KernelSU.fileExists(configPath)) {
        const content = await KernelSU.readFile(configPath)
        gamelistConfig.value = JSON.parse(content)
        console.log('Gamelist config loaded successfully')
        return gamelistConfig.value
      } else {
        gamelistConfig.value = {}
        return gamelistConfig.value
      }
    } catch (e) {
      console.warn('[loadGamelistConfig] Gagal/Localhost mode, memakai mock config:', e)
      // Mock config untuk localhost (lite_mode: 'default' | 'on' | 'off')
      gamelistConfig.value = {
        'com.mobile.legends': { lite_mode: 'on', enable_dnd: false },
        'com.android.chrome': { lite_mode: 'off', enable_dnd: true },
      }
      return gamelistConfig.value
    }
  }

  async function saveGamelistConfig() {
    try {
      const configString = JSON.stringify(gamelistConfig.value, null, 2)
      await KernelSU.writeFile(configPath, configString)
      console.log('Gamelist config saved successfully')
      return true
    } catch (e) {
      console.warn('[saveGamelistConfig] KSU write tidak tersedia (Localhost mode):', e)
      return true
    }
  }

  async function updateAppConfig(packageName, config) {
    if (!packageName) {
      throw new Error('Package name is required')
    }

    const currentConfig = { ...gamelistConfig.value }

    if (config) {
      currentConfig[packageName] = {
        lite_mode: config.lite_mode || 'default',
        enable_dnd: !!config.enable_dnd,
      }
    } else {
      delete currentConfig[packageName]
    }

    gamelistConfig.value = currentConfig

    const appIndex = userApps.value.findIndex((a) => a.packageName === packageName)
    if (appIndex !== -1) {
      userApps.value[appIndex].isEnabled = !!config
      userApps.value[appIndex].liteMode = config ? (config.lite_mode || 'default') : 'default'
      userApps.value[appIndex].isDndEnabled = config ? !!config.enable_dnd : false
    }

    await saveGamelistConfig()

    return currentConfig[packageName] || null
  }

  async function toggleAppEnabled(packageName, enabled) {
    const currentConfig = gamelistConfig.value[packageName] || {}

    if (enabled) {
      return await updateAppConfig(packageName, {
        lite_mode: currentConfig.lite_mode || 'default',
        enable_dnd: currentConfig.enable_dnd || false,
      })
    } else {
      return await updateAppConfig(packageName, null)
    }
  }

  // Alias untuk kompatibilitas UI
  async function toggleAppTweak(packageName, enabled) {
    return await toggleAppEnabled(packageName, enabled)
  }

  // Handler khusus untuk mengubah Lite Mode per App ('default' | 'on' | 'off')
  async function setAppLiteMode(packageName, value) {
    const currentConfig = gamelistConfig.value[packageName] || {}
    return await updateAppConfig(packageName, {
      ...currentConfig,
      lite_mode: value,
    })
  }

  // Handler khusus DND per App
  async function toggleAppDnd(packageName, enabled) {
    const currentConfig = gamelistConfig.value[packageName] || {}
    return await updateAppConfig(packageName, {
      ...currentConfig,
      enable_dnd: enabled,
    })
  }

  async function updateAppSetting(packageName, setting, value) {
    const currentConfig = gamelistConfig.value[packageName] || {}

    return await updateAppConfig(packageName, {
      ...currentConfig,
      [setting]: value,
    })
  }

  async function loadUserApps() {
    if (userApps.value.length > 0) {
      return
    }

    isLoading.value = true

    try {
      const pkgs = await KernelSU.listApps()
      if (!pkgs || pkgs.length === 0) {
        throw new Error('KernelSU listApps mengembalikan data kosong')
      }

      const chunk = 15
      const loaded = []

      for (let i = 0; i < pkgs.length; i += chunk) {
        const slice = pkgs.slice(i, i + chunk)

        try {
          const [infos, icons] = await Promise.allSettled([
            KernelSU.getBatchAppLabel(slice),
            KernelSU.getBatchAppIcons(slice, 100),
          ])

          const appInfos =
            infos.status === 'fulfilled'
              ? infos.value
              : slice.map((pkg) => ({ packageName: pkg, appName: pkg }))

          const appIcons =
            icons.status === 'fulfilled'
              ? icons.value
              : slice.map((pkg) => ({ packageName: pkg, icon: '' }))

          loaded.push(
            ...appInfos.map((info) => {
              const matchingIcon = appIcons.find((icon) => icon.packageName === info.packageName)
              const iconUrl = matchingIcon?.icon || '/fallback_app_icon.avif'
              const cfg = gamelistConfig.value[info.packageName]

              return {
                packageName: info.packageName,
                appName: info.appName,
                icon: iconUrl,
                isEnabled: isAppEnabled(info.packageName),
                liteMode: cfg?.lite_mode || 'default',
                isDndEnabled: cfg?.enable_dnd || false,
              }
            }),
          )
          userApps.value = loaded
          await new Promise((r) => setTimeout(r, 10))
        } catch (batchError) {
          console.error(
            '[loadUserApps] Batch processing failed, loading apps with minimal info:',
            batchError,
          )

          const fallbackApps = slice.map((pkg) => ({
            packageName: pkg,
            appName: pkg,
            icon: '/fallback_app_icon.avif',
            isEnabled: isAppEnabled(pkg),
            liteMode: gamelistConfig.value[pkg]?.lite_mode || 'default',
            isDndEnabled: gamelistConfig.value[pkg]?.enable_dnd || false,
          }))

          loaded.push(...fallbackApps)
          userApps.value = loaded
        }
      }
    } catch (e) {
      console.warn('[loadUserApps] KernelSU API tidak terdeteksi (Localhost Mode). Menampilkan MOCK_APPS.')
      userApps.value = MOCK_APPS.map((app) => ({
        ...app,
        isEnabled: isAppEnabled(app.packageName),
        liteMode: gamelistConfig.value[app.packageName]?.lite_mode || 'default',
        isDndEnabled: gamelistConfig.value[app.packageName]?.enable_dnd || false,
      }))
    } finally {
      isLoading.value = false
    }
  }

  async function refreshFromSettings() {
    await loadGamelistConfig()
    const updatedApps = [...userApps.value]
    updatedApps.forEach((app) => {
      const cfg = gamelistConfig.value[app.packageName]
      app.isEnabled = isAppEnabled(app.packageName)
      app.liteMode = cfg?.lite_mode || 'default'
      app.isDndEnabled = cfg?.enable_dnd || false
    })
    userApps.value = updatedApps
  }

  async function initializeData() {
    try {
      await loadGamelistConfig()
    } catch (e) {
      console.error('Failed to load gamelist:', e)
    }
    await loadUserApps()
  }

  return {
    userApps,
    filteredApps,
    searchQuery,
    isLoading,
    gamelistConfig,
    isAppEnabled,

    // Config management methods
    loadGamelistConfig,
    saveGamelistConfig,
    updateAppConfig,
    toggleAppEnabled,
    toggleAppTweak,
    setAppLiteMode,
    toggleAppDnd,
    updateAppSetting,

    // App management methods
    loadUserApps,
    initializeData,
    refreshFromSettings,
  }
})
