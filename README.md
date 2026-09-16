<p>
  <h1 align="center">Lumina Tweaks</h1>
  <p align="center">Lightweight Native C++ Android Optimization Daemon</p>
</p>

---

## Overview
**Lumina Tweaks** adalah modul optimisi system berbasis daemon native C++ (`luminad`) yang berjalan langsung di level kernel (`sysfs` dan `cgroups`) tanpa melewati framework Java Android.

---

HÃ Features

* **Native C++ Engine (`luminad`)**: Manipulasi node kernel langsung via posix syscall, memangkas beban fork() shell.
* **Dynamic Polling**: 1000ms saat idle untuk C-State deep sleep, dan 500ms saat gaming.
* **Anti-Overwrite OEM Enforcement**: Mencegah daemon thermal vendor (Joyose, PowerHAL, Mi_Thermald) menimpa profil.
* **Flash Wear Protection**: Cache status di RAM mencegah penulisan berulang ke storage UFS (module.prop).

---

HÃ Profiles

* **Profile Balance**: Mode standard harian, governor schedutil.
* **Profile Peformace**: Mode gaming maksimal, clock dikinci. **Profile Peformace Lite**: Mode gaming hemat daya dengan scaling adaptif.  
* **Profile Eco**: Otomatis aktif saat Battery Saver Android nyala.

---

HÃ Build from Source

```bash
clang++ -O3 -std=c++17 -pthread -Iinclude \
  src/utils.cpp src/config.cpp src/profile.cpp src/hardware.cpp src/monitor.cpp src/main.cpp \
  -o luminad
```