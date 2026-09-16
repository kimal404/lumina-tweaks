#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

struct GameRule {
    std::string lite_mode = "default";
    bool enable_dnd = false;
};

struct AppConfig {
    bool lite_mode = false;
    bool disable_thermal = false;
    bool dnd_mode = false;

    std::unordered_set<std::string> gamelist;
    std::unordered_map<std::string, GameRule> game_rules;
    std::mutex mtx;

    bool is_lite_for(const std::string& pkg) const {
        auto it = game_rules.find(pkg);
        if (it != game_rules.end()) {
            if (it->second.lite_mode == "on") return true;
            if (it->second.lite_mode == "off") return false;
        }
        return lite_mode;
    }

    bool is_dnd_for(const std::string& pkg) const {
        auto it = game_rules.find(pkg);
        if (it != game_rules.end()) {
            return it->second.enable_dnd;
        }
        return dnd_mode;
    }
};

extern AppConfig g_config;

namespace ConfigManager {
    void reload();
    void start_watcher();
}
