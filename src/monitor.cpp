#include "monitor.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "profile.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <signal.h>
#include <unistd.h>

namespace Monitor {

static std::atomic<bool> g_running{false};
static std::string g_active_game = "";
static pid_t g_game_pid = -1;
static bool g_thermal_alerted = false;
static bool g_dnd_applied = false;
static bool g_eco_active = false;
static bool g_battery_saver_state = false;

static int g_loop_ticks = 0;
static int g_enforce_ticks = 0;

static const int GRACE_PERIOD_TICKS = 24;
static int g_grace_ticks = 0;

static bool g_last_is_lite = false;
static bool g_last_dis_thermal = false;
static int g_thermal_tier = 0;

static pid_t find_pid_by_pkg(const std::string& pkg_name) {
    std::ifstream cgroup("/dev/cpuset/top-app/cgroup.procs");
    pid_t pid;
    while (cgroup >> pid) {
        std::string cmd = Utils::get_cmdline(pid);
        if (cmd.find(pkg_name) != std::string::npos) {
            return pid;
        }
    }
    return -1;
}

static bool is_process_alive(pid_t pid, const std::string& pkg_name) {
    if (pid <= 0) return false;
    if (kill(pid, 0) != 0) return false;

    std::ifstream status_file("/proc/" + std::to_string(pid) + "/status");
    if (!status_file.is_open()) return false;

    std::string line;
    while (std::getline(status_file, line)) {
        if (line.rfind("State:", 0) == 0) {
            if (line.find('Z') != std::string::npos) return false;
            break;
        }
    }

    std::string cmd = Utils::get_cmdline(pid);
    return (cmd.find(pkg_name) != std::string::npos);
}

static void restore_state(const std::string& reason) {
    std::cout << "[MONITOR] Restore -> " << reason << ": " << g_active_game << std::endl;

    if (g_dnd_applied) {
        Utils::set_dnd(false);
        g_dnd_applied = false;
    }

    if (g_game_pid > 0) {
        Utils::renice_game(g_game_pid, false);
    }

    g_thermal_tier = 0;
    Profile::set_lite_thermal_tier(0);

    if (g_battery_saver_state) {
        Profile::apply_powersave();
        g_eco_active = true;
        Utils::update_module_desc("Eco Mode");
        Utils::send_notif("Lumina Tweaks", "Profile Eco");
    } else {
        Profile::restore_balanced();
        g_eco_active = false;
        Utils::update_module_desc("Balanced");
        Utils::send_notif("Lumina Tweaks", "Profile Balance");
    }

    g_active_game = "";
    g_game_pid = -1;
    g_grace_ticks = 0;
    g_enforce_ticks = 0;
    g_last_is_lite = false;
    g_last_dis_thermal = false;
}

void loop() {
    Utils::update_module_desc("Balanced");

    while (g_running) {
        int sleep_ms = g_active_game.empty() ? 1000 : 500;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));

        if (++g_loop_ticks >= 2) {
            g_loop_ticks = 0;
            int b_temp = Utils::get_battery_temp();

            if (!g_active_game.empty() && g_last_is_lite) {
                if (b_temp >= 43) {
                    if (g_thermal_tier != 4) {
                        g_thermal_tier = 4;
                        Profile::set_lite_thermal_tier(4);
                        Utils::send_notif("Lumina AI", "Suhu " + std::to_string(b_temp) + "°C: Proteksi Aktif (Clock 75%)");
                        Utils::update_module_desc("Lite [Protection 75% - " + std::to_string(b_temp) + "°C] | " + g_active_game);
                    }
                } else if (b_temp >= 40) {
                    if (g_thermal_tier != 3) {
                        g_thermal_tier = 3;
                        Profile::set_lite_thermal_tier(3);
                        Utils::update_module_desc("Lite [Adaptive 85% - " + std::to_string(b_temp) + "°C] | " + g_active_game);
                    }
                } else if (b_temp >= 39) {
                    if (g_thermal_tier != 2) {
                        g_thermal_tier = 2;
                        Profile::set_lite_thermal_tier(2);
                        Utils::update_module_desc("Lite [Adaptive 90% - " + std::to_string(b_temp) + "°C] | " + g_active_game);
                    }
                } else if (b_temp >= 37) {
                    if (g_thermal_tier != 1) {
                        g_thermal_tier = 1;
                        Profile::set_lite_thermal_tier(1);
                        Utils::update_module_desc("Lite [Maintain 95% - " + std::to_string(b_temp) + "°C] | " + g_active_game);
                    }
                } else if (b_temp < 37) {
                    if (g_thermal_tier != 0) {
                        g_thermal_tier = 0;
                        Profile::set_lite_thermal_tier(0);
                        Utils::update_module_desc("Performance Lite | " + g_active_game);
                    }
                }
            } else {
                if (b_temp >= 45 && !g_thermal_alerted) {
                    Utils::send_notif("Lumina Tweaks", "Thermal Warning (" + std::to_string(b_temp) + "°C)");
                    g_thermal_alerted = true;
                } else if (b_temp <= 42) {
                    g_thermal_alerted = false;
                }
            }

            if (g_active_game.empty()) {
                g_battery_saver_state = Utils::is_battery_saver();

                if (g_battery_saver_state && !g_eco_active) {
                    Profile::apply_powersave();
                    g_eco_active = true;
                    Utils::update_module_desc("Eco Mode");
                    Utils::send_notif("Lumina Tweaks", "Profile Eco");
                } else if (!g_battery_saver_state && g_eco_active) {
                    Profile::restore_balanced();
                    g_eco_active = false;
                    Utils::update_module_desc("Balanced");
                    Utils::send_notif("Lumina Tweaks", "Profile Balance");
                }
            }
        }

        std::string top_game = "";
        bool is_lite = false;
        bool dis_thermal = false;
        bool dnd_enabled = false;
        bool tweaks_enabled = true;
        bool is_still_in_config = false;
        bool active_game_enabled = true;

        {
            std::lock_guard<std::mutex> lock(g_config.mtx);
            top_game = Utils::find_game_in_cgroup(g_config.gamelist);

            if (!top_game.empty()) {
                is_lite        = g_config.is_lite_for(top_game);
                dis_thermal    = g_config.disable_thermal;
                dnd_enabled    = g_config.is_dnd_for(top_game);
                tweaks_enabled = g_config.is_enabled_for(top_game);
            }

            if (!g_active_game.empty()) {
                is_still_in_config  = (g_config.gamelist.find(g_active_game) != g_config.gamelist.end());
                active_game_enabled = g_config.is_enabled_for(g_active_game);
            }
        }

        if (!g_active_game.empty() && !is_still_in_config) {
            restore_state("Dihapus dari konfigurasi");
            continue;
        }

        if (!g_active_game.empty() && !active_game_enabled) {
            restore_state("Tweaks dinonaktifkan");
            continue;
        }

        if (!top_game.empty() && tweaks_enabled) {
            g_grace_ticks = 0;

            bool state_changed = (g_active_game != top_game) || 
                                 (is_lite != g_last_is_lite) || 
                                 (dis_thermal != g_last_dis_thermal);

            if (state_changed) {
                g_active_game = top_game;
                g_last_is_lite = is_lite;
                g_last_dis_thermal = dis_thermal;
                g_game_pid = find_pid_by_pkg(top_game);
                g_enforce_ticks = 0;
                g_thermal_tier = 0;
                Profile::set_lite_thermal_tier(0);

                Profile::apply_performance(is_lite, dis_thermal);
                g_eco_active = false;

                if (g_game_pid > 0) {
                    Utils::renice_game(g_game_pid, true);
                }

                std::string mode_str = is_lite ? "Performance Lite" : "Performance";
                Utils::update_module_desc(mode_str + " | " + g_active_game);

                if (dnd_enabled && !g_dnd_applied) {
                    Utils::set_dnd(true);
                    g_dnd_applied = true;
                } else if (!dnd_enabled && g_dnd_applied) {
                    Utils::set_dnd(false);
                    g_dnd_applied = false;
                }

                if (is_lite) {
                    Utils::send_notif("Lumina Tweaks", "Profile Performance Lite");
                } else {
                    Utils::send_notif("Lumina Tweaks", "Profile Performance");
                }
            } else {
                if (++g_enforce_ticks >= 8) {
                    g_enforce_ticks = 0;
                    Profile::apply_performance(g_last_is_lite, g_last_dis_thermal);
                }
            }
        } else {
            if (!g_active_game.empty()) {
                if (!tweaks_enabled) {
                    restore_state("Tweaks dinonaktifkan");
                } else if (!is_process_alive(g_game_pid, g_active_game)) {
                    restore_state("Game ditutup permanen (Hard Exit)");
                } else {
                    g_grace_ticks++;
                    if (g_grace_ticks >= GRACE_PERIOD_TICKS) {
                        restore_state("Batas waktu alt-tab habis (12s)");
                    }
                }
            }
        }
    }
}

void start() {
    g_running = true;
    loop();
}

void stop() {
    if (g_dnd_applied) {
        Utils::set_dnd(false);
        g_dnd_applied = false;
    }
    Utils::update_module_desc("Balanced");
    g_running = false;
}

}
