import { defineStore } from 'pinia'
import { ref } from 'vue'
import { exec } from 'kernelsu'
import * as KernelSU from '@/helpers/KernelSU'

const configPath = '/data/adb/.config/lumina'
const modPath = '/data/adb/modules/lumina'

export const useHomeStore = defineStore('home', () => {
  const daemonPidRaw = ref('')
  const moduleVersion = ref('')
  const currentProfileRaw = ref('')
  const kernelVersion = ref('')
  const chipsetName = ref('')
  const deviceName = ref('')
  const androidSDK = ref('')
  const daemonStatusRaw = ref('loading') // 'loading', 'running', 'stopped', 'error'
  const daemonError = ref('')
  const logoImage = ref('/lumina_happy.avif')
  const isInitialized = ref(false)

  let profileInterval = null
  let daemonInterval = null

  // Actions
  async function initializeData() {
    if (isInitialized.value) return

    await Promise.all([
      getServiceState(),
      getAndroidSDK(),
      getModuleVersion(),
      getCurrentProfile(),
      getKernelVersion(),
      getDeviceInfo(),
    ])

    startProfileMonitoring()
    startDaemonMonitoring()
    isInitialized.value = true
  }

  function startProfileMonitoring() {
    stopProfileMonitoring()

    profileInterval = setInterval(() => {
      getCurrentProfile()
    }, 1000)
  }

  function stopProfileMonitoring() {
    if (profileInterval) {
      clearInterval(profileInterval)
      profileInterval = null
    }
  }

  function startDaemonMonitoring() {
    stopDaemonMonitoring()

    daemonInterval = setInterval(() => {
      getServiceState()
    }, 1000)
  }

  function stopDaemonMonitoring() {
    if (daemonInterval) {
      clearInterval(daemonInterval)
      daemonInterval = null
    }
  }

  async function getServiceState() {
    try {
      const { errno, stdout } = await exec('su -c "pgrep luminad"')
      const pid = stdout.trim().split('\n')[0]

      if (errno === 0 && pid) {
        daemonPidRaw.value = pid
        daemonStatusRaw.value = 'running'
        daemonError.value = ''
        logoImage.value = '/lumina_happy.avif'
        return
      }

      setDaemonStopped()
      return
    } catch (error) {
      setDaemonError(error.message)
    }
  }

  function setDaemonStopped() {
    daemonStatusRaw.value = 'stopped'
    daemonPidRaw.value = ''
    daemonError.value = ''
    logoImage.value = '/lumina_sleeping.avif'
  }

  function setDaemonError(message) {
    daemonStatusRaw.value = 'error'
    daemonError.value = message
    logoImage.value = '/lumina_sleeping.avif'
  }

  async function getAndroidSDK() {
    try {
      if (!KernelSU.isKSUWebUI()) {
        throw new Error('Not running on KSU WebUI')
      }

      const { stdout } = await exec('getprop ro.build.version.sdk')
      androidSDK.value = stdout.trim()
    } catch (error) {
      androidSDK.value = 'unknown'
    }
  }

  async function getModuleVersion() {
    try {
      const propPath = `${modPath}/module.prop`
      const content = await KernelSU.readFile(propPath)
      const match = content.match(/^version=(.*)$/m)
      moduleVersion.value = match ? match[1].trim() : 'unknown'
    } catch (error) {
      moduleVersion.value = 'unknown'
    }
  }

  async function getCurrentProfile() {
    try {
      const govPath = '/sys/devices/system/cpu/cpufreq/policy0/scaling_governor'
      const { errno, stdout } = await exec(`cat ${govPath}`)

      if (errno === 0) {
        const gov = stdout.trim()

        if (gov === 'performance') {
          currentProfileRaw.value = 'performance'
        } else if (gov === 'powersave') {
          currentProfileRaw.value = 'powersave'
        } else if (gov === 'schedutil') {
          currentProfileRaw.value = 'balanced'
        } else {
          currentProfileRaw.value = 'balanced'
        }
        return
      }

      currentProfileRaw.value = 'balanced'
    } catch (error) {
      currentProfileRaw.value = 'unknown'
    }
  }

  async function getKernelVersion() {
    try {
      if (!KernelSU.isKSUWebUI()) {
        throw new Error('Not running on KSU WebUI')
      }

      const { stdout } = await exec('uname -r -m')
      kernelVersion.value = stdout.trim()
    } catch (error) {
      kernelVersion.value = 'unknown'
    }
  }

  // Pengambilan Nama Perangkat & Chipset Murni Dinamis
  async function getDeviceInfo() {
    try {
      if (!KernelSU.isKSUWebUI()) {
        throw new Error('Not running on KSU WebUI')
      }

      // 1. Coba panggil binary C++ luminad
      const { errno, stdout } = await exec(`${modPath}/luminad get_device_info`)
      if (errno === 0 && stdout.trim()) {
        try {
          const data = JSON.parse(stdout.trim())
          if (data.device_name) deviceName.value = data.device_name
          if (data.chipset) chipsetName.value = data.chipset
          return
        } catch (e) {
          // Lanjut ke fallback jika gagal parse JSON
        }
      }

      // 2. Fallback Lokal (Pencarian Dinamis dari System Prop)
      const devRes = await exec('getprop ro.product.marketname || getprop ro.product.model || getprop ro.product.brand')
      deviceName.value = devRes.stdout.trim() || 'Android Device'

      const chipRes = await exec('getprop ro.soc.model || getprop ro.chipname || getprop ro.board.platform')
      chipsetName.value = chipRes.stdout.trim().toUpperCase() || 'Android SoC'
    } catch (error) {
      deviceName.value = 'Android Device'
      chipsetName.value = 'unknown'
    }
  }

  return {
    // Raw state
    daemonPidRaw,
    moduleVersion,
    currentProfileRaw,
    kernelVersion,
    chipsetName,
    deviceName,
    androidSDK,
    daemonStatusRaw,
    daemonError,
    logoImage,
    isInitialized,

    // Actions
    initializeData,
    stopProfileMonitoring,
    stopDaemonMonitoring,
    getServiceState,
    getAndroidSDK,
    getModuleVersion,
    getCurrentProfile,
    getKernelVersion,
    getDeviceInfo,
  }
})
