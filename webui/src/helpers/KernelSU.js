import { exec, toast } from 'kernelsu'
import { getTranslation } from '@/helpers/Locales'

export function isKSUWebUI() {
  return typeof ksu !== 'undefined' || typeof exec !== 'undefined'
}

export function isRunningOnWebUIX() {
  return typeof $lumina !== 'undefined' && Object.keys($lumina).length > 0
}

export function createShortcut() {
  if (typeof ksu !== 'undefined' && typeof ksu.createShortcut === 'function') {
    ksu.createShortcut()
    return
  }
  const shortcut_unavailable = getTranslation('toast.shortcut_unavailable')
  if (typeof toast === 'function') toast(shortcut_unavailable)
}

export async function readFile(filePath) {
  const { errno, stdout, stderr } = await exec(`[ -f "${filePath}" ] && cat "${filePath}"`)
  if (errno != 0) {
    throw new Error(`File cannot be read: ${stderr}`)
  }
  return stdout.trim()
}

export async function writeFile(filePath, content) {
  const escapedContent = content.replace(/'/g, "'\\''")
  await exec(`echo '${escapedContent}' > "${filePath}"`)
}

export async function fileExists(filePath) {
  const { errno } = await exec(`[ -f "${filePath}" ]`)
  return errno === 0
}

export async function openWebsite(link) {
  setTimeout(() => {
    exec(`/system/bin/am start -a android.intent.action.VIEW -d ${link}`).then(({ errno }) => {
      if (errno !== 0 && typeof toast === 'function') {
        const failed_toast = getTranslation('toast.failed_open_extrenal_browser')
        toast(failed_toast)
      }
    })
  }, 100)
}

export async function launchApp(packageName) {
  await exec(`monkey -p ${packageName} -c android.intent.category.LAUNCHER 1`)
}

export async function openAppInfo(packageName) {
  await exec(`am start -a android.settings.APPLICATION_DETAILS_SETTINGS -d package:${packageName}`)
}

export async function listApps() {
  if (typeof ksu !== 'undefined' && typeof ksu.listUserPackages !== 'undefined') {
    return JSON.parse(ksu.listUserPackages())
  } else {
    const { errno, stdout, stderr } = await exec('pm list packages -3')
    if (errno !== 0) {
      throw new Error(`Failed to list user packages: ${stderr}`)
    }
    return stdout
      .split('\n')
      .filter((line) => line.startsWith('package:'))
      .map((line) => line.substring(8).trim())
  }
}

export async function getAppLabel(packageName) {
  try {
    if (typeof ksu !== 'undefined' && typeof ksu.getPackagesInfo !== 'undefined') {
      const packageNamesJson = JSON.stringify([packageName])
      const result = JSON.parse(ksu.getPackagesInfo(packageNamesJson))
      if (result.length > 0 && result[0].error) {
        throw new Error(result[0].error)
      }
      return result[0].appLabel || packageName
    } else {
      return packageName
    }
  } catch (error) {
    return packageName
  }
}

export async function getBatchAppLabel(packageNames) {
  try {
    if (typeof ksu !== 'undefined' && typeof ksu.getPackagesInfo !== 'undefined') {
      const packageNamesJson = JSON.stringify(packageNames)
      const result = JSON.parse(ksu.getPackagesInfo(packageNamesJson))
      return result.map((info) => ({
        packageName: info.packageName,
        appName: info.appLabel || info.packageName,
      }))
    } else {
      return packageNames.map((packageName) => ({
        packageName,
        appName: packageName,
      }))
    }
  } catch (error) {
    return packageNames.map((packageName) => ({
      packageName,
      appName: packageName,
    }))
  }
}

export async function getAppIcon(packageName, size = 100) {
  if (typeof ksu !== 'undefined' && typeof ksu.listPackages !== 'undefined') {
    return `ksu://icon/${packageName}`
  }
  return ''
}

export async function getBatchAppIcons(packageNames, size = 100) {
  if (typeof ksu !== 'undefined' && typeof ksu.listPackages !== 'undefined') {
    return packageNames.map((packageName) => ({
      packageName,
      icon: `ksu://icon/${packageName}`,
    }))
  }
  return packageNames.map((packageName) => ({
    packageName,
    icon: '',
  }))
}

/**
 * Fitur Simpan Log Agresif LUMina
 */
export async function saveLog() {
  try {
    if (typeof toast === 'function') {
      toast('Mengumpulkan log LUMina...')
    }

    const now = new Date()
    const timestamp = now.getFullYear() + '-' +
      String(now.getMonth() + 1).padStart(2, '0') + '-' +
      String(now.getDate()).padStart(2, '0') + '_' +
      String(now.getHours()).padStart(2, '0') + '-' +
      String(now.getMinutes()).padStart(2, '0') + '-' +
      String(now.getSeconds()).padStart(2, '0')

    const logFileName = `Lumina_Log_${timestamp}.txt`
    const logPath = `/sdcard/${logFileName}`

    const logCmd = `
LOG_FILE="${logPath}"
echo "==========================================" > "$LOG_FILE"
echo "           LUMINA TWEAKS LOG REPORT       " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
echo "Waktu         : $(date)" >> "$LOG_FILE"
echo "Brand / Model : $(getprop ro.product.brand) $(getprop ro.product.model) ($(getprop ro.product.device))" >> "$LOG_FILE"
echo "Android Ver   : $(getprop ro.build.version.release) (SDK $(getprop ro.build.version.sdk))" >> "$LOG_FILE"
echo "Kernel Ver    : $(uname -a)" >> "$LOG_FILE"
echo "SELinux       : $(getenforce 2>/dev/null || echo 'Unknown')" >> "$LOG_FILE"
echo "Root Engine   : $(which ksu magisk apatch 2>/dev/null || echo 'Unknown')" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

echo "==========================================" >> "$LOG_FILE"
echo "           STATUS MODUL & CONFIG          " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
if [ -d "/data/adb/modules/lumina" ]; then
  ls -la /data/adb/modules/lumina/ >> "$LOG_FILE" 2>&1
else
  echo "Directory /data/adb/modules/lumina tidak ditemukan!" >> "$LOG_FILE"
fi
echo "" >> "$LOG_FILE"

echo "==========================================" >> "$LOG_FILE"
echo "            CPU FREQ & THERMAL            " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
  if [ -f "$cpu/cpufreq/scaling_cur_freq" ]; then
    echo "$(basename $cpu): $(cat $cpu/cpufreq/scaling_cur_freq 2>/dev/null) Hz (gov: $(cat $cpu/cpufreq/scaling_governor 2>/dev/null))" >> "$LOG_FILE"
  fi
done
echo "" >> "$LOG_FILE"

echo "==========================================" >> "$LOG_FILE"
echo "          MODULE EXECUTION LOGS           " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
for lfile in /data/adb/modules/lumina/*.log /data/local/tmp/lumina*.log; do
  if [ -f "$lfile" ]; then
    echo "--- File: $lfile ---" >> "$LOG_FILE"
    cat "$lfile" >> "$LOG_FILE" 2>&1
    echo "" >> "$LOG_FILE"
  fi
done

echo "==========================================" >> "$LOG_FILE"
echo "          SYSTEM LOG (DMESG TAIL)         " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
dmesg 2>/dev/null | tail -n 80 >> "$LOG_FILE" 2>&1
echo "" >> "$LOG_FILE"

echo "==========================================" >> "$LOG_FILE"
echo "          SYSTEM LOG (LOGCAT TAIL)        " >> "$LOG_FILE"
echo "==========================================" >> "$LOG_FILE"
logcat -d -t 100 2>/dev/null >> "$LOG_FILE" 2>&1

chmod 666 "$LOG_FILE"
`

    const { errno, stderr } = await exec(logCmd)
    if (errno === 0) {
      alert(`Log berhasil disimpan di:\n/sdcard/${logFileName}`)
      return true
    } else {
      alert(`Gagal menyimpan log: ${stderr}`)
      return false
    }
  } catch (err) {
    alert(`Error: ${err.message}`)
    return false
  }
}
