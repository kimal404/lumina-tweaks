#include "config.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <sys/inotify.h>
#include <cstring>

AppConfig g_config;

namespace ConfigManager {

void reload() {
    std::lock_guard<std::mutex> lock(g_config.mtx);

    // 1. Baca preferensi global dari config.json
    {
        std::ifstream f("/data/adb/.config/lumina/config.json");
        if (f.is_open()) {
            std::stringstream ss;
            ss << f.rdbuf();
            std::string s = ss.str();
            g_config.disable_thermal = (s.find("\"disable_thermal\": true") != std::string::npos ||
                                        s.find("\"disable_thermal\":true") != std::string::npos ||
                                        s.find("\"disable_thermal\": 1") != std::string::npos);

            g_config.lite_mode = (s.find("\"lite_mode\": true") != std::string::npos ||
                                  s.find("\"lite_mode\":true") != std::string::npos ||
                                  s.find("\"lite_mode\": \"on\"") != std::string::npos ||
                                  s.find("\"lite_mode\": 1") != std::string::npos);

            g_config.dnd_mode = (s.find("\"enable_dnd\": true") != std::string::npos ||
                                 s.find("\"enable_dnd\":true") != std::string::npos ||
                                 s.find("\"dnd_mode\": true") != std::string::npos ||
                                 s.find("\"enable_dnd\": 1") != std::string::npos);
        }
    }

    // 2. Baca daftar package dan rule per-app dari gamelist.json
    g_config.gamelist.clear();
    g_config.game_rules.clear();

    {
        std::ifstream f("/data/adb/.config/lumina/gamelist.json");
        if (f.is_open()) {
            std::string line;
            std::string current_pkg = "";

            while (std::getline(f, line)) {
                size_t colon = line.find(':');
                size_t brace = line.find('{');

                // Deteksi awal objek package: "com.package.name": {
                if (brace != std::string::npos && colon != std::string::npos) {
                    size_t q1 = line.find('"');
                    size_t q2 = (q1 != std::string::npos) ? line.find('"', q1 + 1) : std::string::npos;
                    if (q1 != std::string::npos && q2 != std::string::npos && q2 < colon) {
                        std::string pkg = line.substr(q1 + 1, q2 - q1 - 1);
                        if (pkg.find('.') != std::string::npos) {
                            current_pkg = pkg;
                            g_config.gamelist.insert(pkg);
                            g_config.game_rules[pkg] = GameRule();
                        }
                    }
                    continue;
                }

                if (current_pkg.empty()) continue;

                if (line.find('}') != std::string::npos) {
                    current_pkg = "";
                    continue;
                }

                if (line.find("\"lite_mode\"") != std::string::npos) {
                    if (line.find("\"on\"") != std::string::npos) {
                        g_config.game_rules[current_pkg].lite_mode = "on";
                    } else if (line.find("\"off\"") != std::string::npos) {
                        g_config.game_rules[current_pkg].lite_mode = "off";
                    } else if (line.find("\"default\"") != std::string::npos) {
                        g_config.game_rules[current_pkg].lite_mode = "default";
                    }
                }

                if (line.find("\"enable_dnd\"") != std::string::npos) {
                    if (line.find("true") != std::string::npos) {
                        g_config.game_rules[current_pkg].enable_dnd = true;
                    } else if (line.find("false") != std::string::npos) {
                        g_config.game_rules[current_pkg].enable_dnd = false;
                    }
                }
            }
        }
    }

    std::cout << "[CONFIG] Terbaca: " << g_config.gamelist.size() << " game dari gamelist.json" << std::endl;
    for (const auto& g : g_config.gamelist) {
        auto rule = g_config.game_rules[g];
        std::cout << "[CONFIG] -> " << g << " [Lite: " << rule.lite_mode 
                  << ", DND: " << (rule.enable_dnd ? "ON" : "OFF") << "]" << std::endl;
    }
}

void start_watcher() {
    reload();

    std::thread([]() {
        int fd = inotify_init();
        if (fd < 0) return;

        int wd = inotify_add_watch(fd, "/data/adb/.config/lumina", IN_CLOSE_WRITE | IN_MOVED_TO);
        if (wd < 0) {
            close(fd);
            return;
        }

        char buffer[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
        while (true) {
            int length = read(fd, buffer, sizeof(buffer));
            if (length <= 0) break;

            int i = 0;
            bool should_reload = false;
            while (i < length) {
                struct inotify_event* event = (struct inotify_event*)&buffer[i];
                if (event->len > 0) {
                    if (strcmp(event->name, "config.json") == 0 || strcmp(event->name, "gamelist.json") == 0) {
                        should_reload = true;
                    }
                }
                i += sizeof(struct inotify_event) + event->len;
            }

            if (should_reload) {
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                reload();
            }
        }
        close(fd);
    }).detach();
}

} // namespace ConfigManager
