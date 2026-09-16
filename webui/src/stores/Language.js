import { defineStore } from 'pinia'
import { i18n } from '@/main'
import languages from '@/locales/languages.json'
import { checkLanguageFile, loadLocaleMessages } from '@/helpers/Locales'

export const useLanguageStore = defineStore('language', {
  state: () => {
    // 1. Kunci bawaan dari localStorage (Default 'id' jika belum pernah pilih)
    const savedLang = localStorage.getItem('preferred-language') || 'id'
    return {
      currentLanguage: savedLang,
      availableLanguages: languages,
      userPreference: savedLang,
    }
  },

  getters: {
    isRTL: (state) => {
      return state.availableLanguages[state.currentLanguage]?.dir === 'rtl'
    },
    currentLanguageDir: (state) => {
      return state.availableLanguages[state.currentLanguage]?.dir || 'ltr'
    },
    languageName: (state) => (locale) => {
      return state.availableLanguages[locale]?.name || locale
    },
    // 2. Hapus logika "Follow System", langsung tampilkan nama bahasa aktif
    displayText: (state) => {
      const langData = state.availableLanguages[state.currentLanguage]
      return langData?.name || state.currentLanguage
    },
  },

  actions: {
    async setLanguage(locale) {
      try {
        if (!this.availableLanguages[locale]) {
          console.warn(`Language ${locale} is not available`)
          return false
        }

        const hasFile = await checkLanguageFile(locale)
        if (!hasFile) {
          console.warn(`Translation file not found for locale: ${locale}`)
          return false
        }

        const messages = await loadLocaleMessages(locale)

        // Set locale di vue-i18n (Toleran Vue I18n v8 / v9)
        if (i18n.global.locale.value !== undefined) {
          i18n.global.setLocaleMessage(locale, messages)
          i18n.global.locale.value = locale
        } else {
          i18n.global.setLocaleMessage(locale, messages)
          i18n.global.locale = locale
        }

        this.currentLanguage = locale
        this.userPreference = locale

        // 3. Selalu kunci ke localStorage
        localStorage.setItem('preferred-language', locale)

        this.updateHtmlAttributes()

        window.dispatchEvent(
          new CustomEvent('language-changed', {
            detail: { preference: this.userPreference, language: locale },
          }),
        )

        return true
      } catch (error) {
        console.error(`Failed to set language to ${locale}:`, error)
        return false
      }
    },

    // 4. Inisialisasi otomatis memuat bahasa terakhir dari localStorage saat app dinyalakan
    async initializeLanguage() {
      const savedLang = localStorage.getItem('preferred-language') || 'id'
      await this.setLanguage(savedLang)
      this.updateHtmlAttributes()
    },

    updateHtmlAttributes() {
      const html = document.documentElement
      const dir = this.currentLanguageDir
      html.setAttribute('dir', dir)
      html.setAttribute('lang', this.currentLanguage)

      document.body.classList.toggle('rtl', dir === 'rtl')
      document.body.classList.toggle('ltr', dir === 'ltr')
    },

    getAvailableLanguages() {
      return Object.entries(this.availableLanguages).map(([code, data]) => ({
        code,
        name: data.name,
        dir: data.dir,
      }))
    },
  },
})
