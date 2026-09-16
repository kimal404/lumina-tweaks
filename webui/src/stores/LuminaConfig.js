import { defineStore } from 'pinia'
import { ref, computed } from 'vue'

import { exec } from 'kernelsu'
import * as KernelSU from '@/helpers/KernelSU'

import { useHomeStore } from '@/stores/Home'

const DEFAULT_MOCK_CONFIG = {
  preferences: {
    enforce_lite_mode: false,
    use_device_mitigation: false,
    disable_tweaks: false,
    log_level: 3,
  },
  cpu_governor: {
    balance: 'schedutil',
    powersave: 'powersave',
  },
  cpu_limit: 100,
  io_scheduler: 'none',
  auto_mode: true,
  ram_tweaks: false,
  mtk_tweaks: false,
  tcp_bbr: false,
  language: 'id',
  disable_thermal: false,
  refresh_rate: '60 Hz',
  render_engine: 'Vulkan',
}

export const useLuminaConfigStore = defineStore('luminaConfig', () => {
  const config = ref(null)

  const homeStore = useHomeStore()
  const currentProfile = computed(() => homeStore.currentProfileRaw)

  const preferences = computed(() => config.value?.preferences)
  const cpuGovernor = computed(() => config.value?.cpu_governor)

  const isLiteModeEnabled = computed(() => config.value?.preferences?.enforce_lite_mode ?? false)
  const logLevel = computed(() => config.value?.preferences?.log_level ?? 3)
  const isDeviceMitigationEnabled = computed(
    () => config.value?.preferences?.use_device_mitigation ?? false,
  )
  const isDisableTweaksEnabled = computed(() => config.value?.preferences?.disable_tweaks ?? false)
  const balanceGovernor = computed(() => config.value?.cpu_governor?.balance ?? 'schedutil')
  const powersaveGovernor = computed(() => config.value?.cpu_governor?.powersave ?? 'schedutil')

  const cpuLimit = computed(() => config.value?.cpu_limit ?? 100)
  const ioScheduler = computed(() => config.value?.io_scheduler ?? 'none')
  const isAutoModeEnabled = computed(() => config.value?.auto_mode ?? true)
  const isRamTweaksEnabled = computed(() => config.value?.ram_tweaks ?? false)
  const isMtkTweaksEnabled = computed(() => config.value?.mtk_tweaks ?? false)
  const isTcpBbrEnabled = computed(() => config.value?.tcp_bbr ?? false)
  const language = computed(() => config.value?.language ?? 'id')

  const isDisableThermalEnabled = computed(() => config.value?.disable_thermal ?? false)
  const refreshRate = computed(() => config.value?.refresh_rate ?? '60 Hz')
  const renderEngine = computed(() => config.value?.render_engine ?? 'Vulkan')

  const isLoaded = computed(() => config.value !== null)

  const configPath = '/data/adb/.config/lumina/config.json'
  const autoFlagPath = '/data/adb/.config/lumina/auto_mode'

  async function loadConfig() {
    try {
      if (await KernelSU.fileExists(configPath)) {
        const content = await KernelSU.readFile(configPath)
        config.value = JSON.parse(content)
        ensureConfigStructure()
        return config.value
      } else {
        config.value = JSON.parse(JSON.stringify(DEFAULT_MOCK_CONFIG))
        return config.value
      }
    } catch (error) {
      config.value = JSON.parse(JSON.stringify(DEFAULT_MOCK_CONFIG))
      return config.value
    }
  }

  async function saveConfig() {
    if (!config.value) {
      throw new Error('Config not loaded')
    }

    try {
      const configString = JSON.stringify(config.value, null, 2)
      await KernelSU.writeFile(configPath, configString)
      return true
    } catch (error) {
      return true
    }
  }

  function ensureConfigStructure() {
    if (!config.value) {
      config.value = JSON.parse(JSON.stringify(DEFAULT_MOCK_CONFIG))
    }

    if (!config.value.preferences) {
      config.value.preferences = {}
    }
    if (!config.value.cpu_governor) {
      config.value.cpu_governor = {}
    }

    if (config.value.preferences.use_device_mitigation === undefined) {
      config.value.preferences.use_device_mitigation = false
    }
    if (config.value.preferences.disable_tweaks === undefined) {
      config.value.preferences.disable_tweaks = false
    }
    if (config.value.preferences.enforce_lite_mode === undefined) {
      config.value.preferences.enforce_lite_mode = false
    }
    if (config.value.preferences.log_level === undefined) {
      config.value.preferences.log_level = 3
    }

    if (config.value.cpu_limit === undefined) {
      config.value.cpu_limit = 100
    }
    if (config.value.io_scheduler === undefined) {
      config.value.io_scheduler = 'none'
    }
    if (config.value.auto_mode === undefined) {
      config.value.auto_mode = true
    }
    if (config.value.ram_tweaks === undefined) {
      config.value.ram_tweaks = false
    }
    if (config.value.mtk_tweaks === undefined) {
      config.value.mtk_tweaks = false
    }
    if (config.value.tcp_bbr === undefined) {
      config.value.tcp_bbr = false
    }
    if (config.value.language === undefined) {
      config.value.language = 'id'
    }
    if (config.value.disable_thermal === undefined) {
      config.value.disable_thermal = false
    }
    if (config.value.refresh_rate === undefined) {
      config.value.refresh_rate = '60 Hz'
    }
    if (config.value.render_engine === undefined) {
      config.value.render_engine = 'Vulkan'
    }
  }

  async function setLanguage(lang) {
    ensureConfigStructure()
    config.value.language = lang
    await saveConfig()
  }

  async function setLiteMode(enabled) {
    ensureConfigStructure()
    config.value.preferences.enforce_lite_mode = enabled
    await saveConfig()
  }

  async function setLogLevel(level) {
    if (level < 0 || level > 5) {
      throw new Error('Log level must be between 0 and 5')
    }

    ensureConfigStructure()
    config.value.preferences.log_level = level
    await saveConfig()
  }

  async function setDeviceMitigation(enabled) {
    ensureConfigStructure()
    config.value.preferences.use_device_mitigation = enabled
    await saveConfig()
  }

  async function setDisableTweaks(enabled) {
    ensureConfigStructure()
    config.value.preferences.disable_tweaks = enabled
    await saveConfig()
  }

  async function setAutoMode(enabled) {
    ensureConfigStructure()
    config.value.auto_mode = enabled
    await saveConfig()

    const flagVal = enabled ? '1' : '0'
    try {
      await KernelSU.writeFile(autoFlagPath, flagVal)
    } catch (e) {}
  }

  async function applyProfile(profileName) {
    try {
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_profile ${profileName}`).catch(() => {})
    } catch (e) {}
  }

  async function setCpuLimit(limit) {
    ensureConfigStructure()
    config.value.cpu_limit = limit
    await saveConfig()

    try {
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_cpu_limit ${limit}`).catch(() => {})
    } catch (e) {}
  }

  async function setIoScheduler(scheduler) {
    ensureConfigStructure()
    config.value.io_scheduler = scheduler
    await saveConfig()

    try {
      exec(`/data/adb/modules/lumina/system/bin/luminad set_io_scheduler ${scheduler}`).catch(() => {})
    } catch (e) {}
  }

  async function setRamTweaks(enabled) {
    ensureConfigStructure()
    config.value.ram_tweaks = enabled
    await saveConfig()

    try {
      const val = enabled ? 1 : 0
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_ram_tweaks ${val}`).catch(() => {})
    } catch (e) {}
  }

  async function setMtkTweaks(enabled) {
    ensureConfigStructure()
    config.value.mtk_tweaks = enabled
    await saveConfig()

    try {
      const val = enabled ? 1 : 0
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_mtk_tweaks ${val}`).catch(() => {})
    } catch (e) {}
  }

  async function setTcpBbr(enabled) {
    ensureConfigStructure()
    config.value.tcp_bbr = enabled
    await saveConfig()

    try {
      const val = enabled ? 1 : 0
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_tcp_bbr ${val}`).catch(() => {})
    } catch (e) {}
  }

  async function setDisableThermal(enabled) {
    ensureConfigStructure()
    config.value.disable_thermal = enabled
    await saveConfig()

    try {
      const val = enabled ? 1 : 0
      exec(`/data/adb/modules/lumina/system/bin/luminad apply_disable_thermal ${val}`).catch(() => {})
    } catch (e) {}
  }

  async function setRefreshRate(rate) {
    ensureConfigStructure()
    config.value.refresh_rate = rate
    await saveConfig()

    try {
      exec(`/data/adb/modules/lumina/system/bin/luminad set_refresh_rate "${rate}"`).catch(() => {})
    } catch (e) {}
  }

  async function setRenderEngine(engine) {
    ensureConfigStructure()
    config.value.render_engine = engine
    await saveConfig()

    try {
      exec(`/data/adb/modules/lumina/system/bin/luminad set_render_engine "${engine}"`).catch(() => {})
    } catch (e) {}
  }

  function setBalanceGovernor(governor) {
    ensureConfigStructure()
    config.value.cpu_governor.balance = governor
    saveConfig()

    if (
      currentProfile.value === 'balanced' ||
      (currentProfile.value === 'performance' && isLiteModeEnabled.value)
    ) {
      try {
        exec(`/data/adb/modules/lumina/system/bin/luminad change_cpu_gov ${governor}`).catch(() => {})
      } catch (e) {}
    }
  }

  function setPowersaveGovernor(governor) {
    ensureConfigStructure()
    config.value.cpu_governor.powersave = governor
    saveConfig()

    if (currentProfile.value === 'powersave') {
      try {
        exec(`/data/adb/modules/lumina/system/bin/luminad change_cpu_gov ${governor}`).catch(() => {})
      } catch (e) {}
    }
  }

  function setCpuGovernorProfile(profile, governor) {
    if (profile === 'balance') {
      setBalanceGovernor(governor)
    } else if (profile === 'powersave') {
      setPowersaveGovernor(governor)
    } else {
      throw new Error('Invalid CPU governor profile. Must be "balance" or "powersave"')
    }
  }

  function updateConfig(newConfig) {
    if (!config.value) {
      config.value = JSON.parse(JSON.stringify(DEFAULT_MOCK_CONFIG))
    }

    config.value = {
      ...config.value,
      ...newConfig,
      preferences: {
        ...config.value.preferences,
        ...(newConfig.preferences || {}),
      },
      cpu_governor: {
        ...config.value.cpu_governor,
        ...(newConfig.cpu_governor || {}),
      },
      cpu_limit: newConfig.cpu_limit ?? config.value.cpu_limit,
      io_scheduler: newConfig.io_scheduler ?? config.value.io_scheduler,
      auto_mode: newConfig.auto_mode ?? config.value.auto_mode,
      ram_tweaks: newConfig.ram_tweaks ?? config.value.ram_tweaks,
      mtk_tweaks: newConfig.mtk_tweaks ?? config.value.mtk_tweaks,
      tcp_bbr: newConfig.tcp_bbr ?? config.value.tcp_bbr,
      language: newConfig.language ?? config.value.language,
      disable_thermal: newConfig.disable_thermal ?? config.value.disable_thermal,
      refresh_rate: newConfig.refresh_rate ?? config.value.refresh_rate,
      render_engine: newConfig.render_engine ?? config.value.render_engine,
    }
    saveConfig()
  }

  return {
    config,

    preferences,
    cpuGovernor,
    isLiteModeEnabled,
    logLevel,
    isDeviceMitigationEnabled,
    isDisableTweaksEnabled,
    balanceGovernor,
    powersaveGovernor,
    cpuLimit,
    ioScheduler,
    isAutoModeEnabled,
    isRamTweaksEnabled,
    isMtkTweaksEnabled,
    isTcpBbrEnabled,
    isDisableThermalEnabled,
    refreshRate,
    renderEngine,
    language,
    isLoaded,

    loadConfig,
    saveConfig,  
    setLanguage,
    setLiteMode,
    setLogLevel,
    setDeviceMitigation,
    setAutoMode,
    applyProfile,
    setCpuLimit,
    setIoScheduler,
    setRamTweaks,
    setMtkTweaks,
    setTcpBbr,
    setDisableThermal,
    setRefreshRate,
    setRenderEngine,
    setBalanceGovernor,
    setPowersaveGovernor,
    setCpuGovernorProfile,
    updateConfig,
  }
})
