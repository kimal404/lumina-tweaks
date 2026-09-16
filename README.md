<p align="center">
  <h1 align="center">Lumina Tweaks</h1>
  <p align="center">Native System Performance Daemon for Android</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Status-Stable-brightgreen?style=flat-square" alt="Status">
  <img src="https://img.shields.io/badge/License-GPL--3.0-blue?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/Android-11+-green?style=flat-square&logo=android" alt="Android">
  <img src="https://img.shields.io/badge/Root-KernelSU%20%7C%20APatch%20%7C%20Magisk-black?style=flat-square" alt="Root">
</p>

---

## Overview
**Lumina Tweaks** adalah modul optimasi performa berbasis daemon native C++ (`luminad`) dengan antarmuka WebUI. Berjalan langsung di level kernel node (`sysfs`, `cgroups`) untuk menjaga stabilitas FPS game dan efisiensi daya harian tanpa membebani runtime Android.

---

## Features
* **Native C++ Daemon (`luminad`):** Akses sysfs via syscall Linux langsung, tanpa overhead sub-shell.
* **Auto Profile Switch:** Deteksi game aktif via cgroup dan terapkan profil otomatis.
* **Anti-Overwrite:** Mengunci clock profil agar tidak ditimpa oleh thermal daemon OEM.
* **WebUI Dashboard:** Kontrol profil dan aturan game langsung dari manager root.

---

## Profiles
* **Performance:** Clock maksimal untuk game berat.
* **Performance Lite:** Target FPS stabil dengan suhu terkontrol.
* **Balance:** Mode default harian berbasis governor `schedutil`.
* **Eco:** Pembatasan clock otomatis saat Battery Saver Android aktif.

---

## Credits
* **[Encore Tweaks](https://github.com/Rem01Gaming/encore)** by [@Rem01Gaming](https://github.com/Rem01Gaming) — Base WebUI (Vue) & referensi tweak.
* **[AZenith](https://github.com/Liliya2727/AZenith)** by [@Zexshia](https://github.com/Liliya2727) — Referensi kode.

---

## License
Project ini dilisensikan di bawah **GNU General Public License v3.0 (GPL-3.0)** sesuai basis WebUI yang digunakan. Detail atribusi ada di [NOTICE.md](NOTICE.md).
