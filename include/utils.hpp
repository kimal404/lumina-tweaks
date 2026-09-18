#pragma once

#include <string>
#include <unordered_set>

enum class SocType {
    GENERIC,
    MEDIATEK,
    SNAPDRAGON,
    UNISOC
};

namespace Utils {
    bool write_sysfs(const std::string& path, const std::string& value);
    bool write_sysfs_verify(const std::string& path, const std::string& value, int retries = 5, bool lock_ro = false);
    std::string read_sysfs(const std::string& path);
    std::string trim(const std::string& str);
    
    bool is_mediatek();
    bool is_snapdragon();
    bool is_unisoc();
    SocType get_soc_type();

    std::string get_cmdline(int pid);
    std::string find_game_in_cgroup(const std::unordered_set<std::string>& gamelist);
    void send_notif(const std::string& title, const std::string& msg);
    int get_battery_temp();
    void set_dnd(bool enable);
    bool is_battery_saver();
    void update_module_desc(const std::string& status);

    void renice_game(int pid, bool boost);
}
