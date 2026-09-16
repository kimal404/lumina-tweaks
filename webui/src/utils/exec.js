export async function exec(command) {
  try {
    // 1. KernelSU Native WebUI Bridge
    if (window.ksu && typeof window.ksu.exec === 'function') {
      const { errno, stdout, stderr } = await window.ksu.exec(command)
      if (errno === 0) return stdout
      console.error('[KSU Exec Error]:', stderr)
      return ''
    }
    
    // 2. APatch / Universal WebUI Bridge Fallback
    if (window.exec) {
      return await window.exec(command)
    }

    console.warn('[Exec Warning] WebUI Environment bukan di dalam KernelSU App.')
    return ''
  } catch (err) {
    console.error('[Exec Exception]:', err)
    return ''
  }
}
