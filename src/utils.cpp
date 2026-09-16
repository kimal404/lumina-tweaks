#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdlib>
#include <cstdio>

namespace Utils {

static std::string s_last_module_status = "";

bool write_sysfs(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << value;
    return true;
}

std::string read_sysfs(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::string content;
    std::getline(file, content);
    return content;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool is_mediatek() {
    return (access("/proc/ppm", F_OK) == 0 || access("/sys/module/ged", F_OK) == 0);
}

std::string get_cmdline(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::string cmd;
    std::getline(file, cmd, '\0');
    return cmd;
}

std::string find_game_in_cgroup(const std::unordered_set<std::string>& gamelist) {
    if (gamelist.empty()) return "";

    std::ifstream file("/dev/cpuset/top-app/cgroup.procs");
    if (!file.is_open()) return "";

    int pid;
    while (file >> pid) {
        std::string cmd = get_cmdline(pid);
        if (cmd.empty()) continue;

        if (gamelist.find(cmd) != gamelist.end()) {
            return cmd;
        }

        size_t colon_pos = cmd.find(':');
        if (colon_pos != std::string::npos) {
            std::string base_pkg = cmd.substr(0, colon_pos);
            if (gamelist.find(base_pkg) != gamelist.end()) {
                return base_pkg;
            }
        }
    }
    return "";
}

void send_notif(const std::string& title, const std::string& msg) {
    std::string cmd = "su 2000 -c \"cmd notification post -S bigtext -t '" + title + "' 'lumina_notif' '" + msg + "'\" >/dev/null 2>&1 &";
    system(cmd.c_str());
}

int get_battery_temp() {
    std::string raw = trim(read_sysfs("/sys/class/power_supply/battery/temp"));
    if (raw.empty()) {
        raw = trim(read_sysfs("/sys/class/thermal/thermal_zone0/temp"));
    }
    if (raw.empty()) return 0;

    try {
        long t = std::stol(raw);
        if (t > 10000) return (int)(t / 1000);
        if (t > 100) return (int)(t / 10);
        return (int)t;
    } catch (...) {
        return 0;
    }
}

void set_dnd(bool enable) {
    std::string cmd = enable ? "cmd notification set_zen_mode 1 >/dev/null 2>&1"
                             : "cmd notification set_zen_mode 0 >/dev/null 2>&1";
    system(cmd.c_str());
}

bool is_battery_saver() {
    FILE* fp = popen("settings get global low_power 2>/dev/null", "r");
    if (!fp) return false;
    char buf[16] = {0};
    bool active = false;
    if (fgets(buf, sizeof(buf), fp) != nullptr) {
        active = (buf[0] == '1');
    }
    pclose(fp);
    return active;
}

void update_module_desc(const std::string& status) {
    if (s_last_module_status == status) return;
    s_last_module_status = status;

    const std::string prop_path = "/data/adb/modules/lumina/module.prop";
    std::ifstream file(prop_path);
    if (!file.is_open()) return;

    std::string line;
    std::string new_content = "";
    while (std::getline(file, line)) {
        if (line.rfind("description=", 0) == 0) {
            new_content += "description=[" + status + "] Adaptive C++ engine active.\n";
        } else {
            new_content += line + "\n";
        }
    }
    file.close();

    std::ofstream out(prop_path);
    if (out.is_open()) {
        out << new_content;
    }
}

}
