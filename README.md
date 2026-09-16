<p align="center">
  <h1 align="center">Lumina Tweaks</h1>
  <p align="center">Native System Performance Daemon for Android</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Status-Stable-brightgreen?style=for-the-badge" alt="Status">
  <img src="https://img.shields.io/badge/License-GPL--3.0-blue?style=for-the-badge" alt="License">
  <img src="https://img.shields.io/badge/Android-11+-blue?style=for-the-badge&logo=android" alt="Android Version">
  <img src="https://img.shields.io/badge/Root-KernelSU%20%7C%20APatch%20%7C%20Magisk-black?style=for-the-badge" alt="Root">
</p>

---

## ⚡ Overview
**Lumina Tweaks** is an optimization framework powered by `luminad`, a native C++ runtime daemon. Interfacing directly with Linux kernel nodes (`sysfs`, `cgroups`, and schedulers), it delivers consistent gaming performance while maintaining battery efficiency and system stability.

---

## 🚀 Key Features
* **Native C++ Engine (`luminad`):** Direct kernel dispatching via POSIX system calls, eliminating shell `fork()` overhead and runtime latency.
* **Autonomous Profile Switching:** Instantly switches system profiles when target games are detected in foreground.
* **Kernel Lock Enforcement:** Actively prevents vendor thermal services from overriding tuned CPU/GPU frequencies.
* **WebUI Dashboard:** Integrated Vue-based control panel accessible directly inside KernelSU, APatch, and Magisk.

---

## 📊 Profiles
| Profile | Target State | Description |
| :--- | :--- | :--- |
| **Performance** | Hardcore Gaming | Clocks locked to peak tables; thermal limits unconstrained. |
| **Performance Lite** | Casual Gaming | Adaptive scaling with conservative thermal safety limits. |
| **Balance** | Daily Usage | Default dynamic scaling using `schedutil` governor. |
| **Eco** | Battery Saver | Automatic frequency limits triggered by Android battery saver. |

---

## 🤝 Credits & Acknowledgements
* **[Encore Tweaks](https://github.com/Rem01Gaming/encore)** by [@Rem01Gaming](https://github.com/Rem01Gaming)
  * **WebUI Base:** Frontend WebUI dashboard is adapted and modified from Encore's Vue WebUI.
  * **Tweaks Reference:** Kernel node logic and scheduler tuning reference.
* **[AZenith](https://github.com/Liliya2727/AZenith)** by [@Zexshia](https://github.com/Liliya2727)
  * **Reference Only:** Reference for profile design concepts and system tuning ideas.

---

## ⚖️ License
This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)** in compliance with the modified WebUI base. See [NOTICE.md](NOTICE.md) for full attribution details.
