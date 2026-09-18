#include "config.hpp"
#include "utils.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <sys/inotify.h>
#include <cstring>

using json = nlohmann::json;
AppConfig g_config;

namespace ConfigManager {

static bool parse_bool(const json& j, const std::string& key, bool default_val) {
    if (!j.contains(key)) return default_val;
    const auto& val = j[key];
    if (val.is_boolean()) return val.get<bool>();
    if (val.is_number()) return val.get<int>() != 0;
    if (val.is_string()) {
        std::string s = val.get<std::string>();
        return (s == "true" || s == "on" || s == "1");
    }
    return default_val;
}

void reload() {
    std::lock_guard<std::mutex> lock(g_config.mtx);

    // 1. Baca preferensi global dari config.json
    {
        std::ifstream f("/data/adb/.config/lumina/config.json");
        if (f.is_open()) {
            try {
                json j = json::parse(f);
                g_config.disable_thermal = parse_bool(j, "disable_thermal", false);
                g_config.lite_mode       = parse_bool(j, "lite_mode", false);
                g_config.dnd_mode        = parse_bool(j, "enable_dnd", parse_bool(j, "dnd_mode", false));
            } catch (const json::parse_error& e) {
                std::cerr << "[CONFIG] Gagal parse config.json: " << e.what() << std::endl;
            }
        }
    }

    // 2. Baca daftar package dan rule per-app dari gamelist.json
    {
        std::ifstream f("/data/adb/.config/lumina/gamelist.json");
        if (f.is_open()) {
            try {
                json j = json::parse(f);
                if (j.is_object()) {
                    decltype(g_config.gamelist) temp_gamelist;
                    decltype(g_config.game_rules) temp_rules;

                    for (auto& [pkg, data] : j.items()) {
                        if (pkg.empty() || pkg.find('.') == std::string::npos) continue;

                        temp_gamelist.insert(pkg);
                        GameRule rule;

                        if (data.is_object()) {
                            rule.enabled = parse_bool(data, "enabled", true);
                            if (data.contains("lite_mode")) {
                                if (data["lite_mode"].is_string()) {
                                    rule.lite_mode = data["lite_mode"].get<std::string>();
                                } else if (data["lite_mode"].is_boolean()) {
                                    rule.lite_mode = data["lite_mode"].get<bool>() ? "on" : "off";
                                }
                            }
                            rule.enable_dnd = parse_bool(data, "enable_dnd", false);
                        }
                        temp_rules[pkg] = rule;
                    }

                    g_config.gamelist = std::move(temp_gamelist);
                    g_config.game_rules = std::move(temp_rules);
                }
            } catch (const json::parse_error& e) {
                std::cerr << "[CONFIG] Race write gamelist.json, pertahankan data lama." << std::endl;
            }
        }
    }

    std::cout << "[CONFIG] Terbaca: " << g_config.gamelist.size() << " game dari gamelist.json" << std::endl;
    for (const auto& g : g_config.gamelist) {
        auto rule = g_config.game_rules[g];
        std::cout << "[CONFIG] -> " << g 
                  << " [Enabled: " << (rule.enabled ? "ON" : "OFF")
                  << ", Lite: " << rule.lite_mode 
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
