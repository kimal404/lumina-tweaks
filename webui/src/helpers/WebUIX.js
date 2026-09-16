export const exec = async (command) => {
  if (typeof window !== 'undefined') {

    if (window.mmrl && window.mmrl.exec) {
      return await window.mmrl.exec(command)
    }

    if (window.ksu && window.ksu.exec) {
      return await window.ksu.exec(command)
    }

    if (window.exec) {
      return await window.exec(command)
    }
  }
  throw new Error('Bridge root tidak ditemukan.')
}


export const toast = (message) => {
  if (typeof window !== 'undefined') {
    if (window.mmrl && window.mmrl.toast) {
      window.mmrl.toast(message)
      return
    }
    if (window.ksu && window.ksu.toast) {
      window.ksu.toast(message)
      return
    }
  }
  console.log('[Toast]:', message)
}
