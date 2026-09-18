#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <sys/stat.h>

namespace Utils {

static std::string s_last_module_status = "";

bool write_sysfs(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << value;
    return true;
}

bool write_sysfs_verify(const std::string& path, const std::string& value, int retries, bool lock_ro) {
    chmod(path.c_str(), 0666);
    for (int i = 0; i < retries; ++i) {
        write_sysfs(path, value);
        if (trim(read_sysfs(path)) == trim(value)) {
            if (lock_ro) chmod(path.c_str(), 0444);
            return true;
        }
        usleep(50000);
    }
    if (lock_ro) chmod(path.c_str(), 0444);
    return false;
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
    std::string dt_model = read_sysfs("/proc/device-tree/model");
    std::string dt_compat = read_sysfs("/proc/device-tree/compatible");
    std::string dt = dt_model + " " + dt_compat;
    std::transform(dt.begin(), dt.end(), dt.begin(), ::tolower);
    if (dt.find("mt") != std::string::npos || dt.find("mediatek") != std::string::npos) return true;

    return (access("/proc/ppm", F_OK) == 0 || 
            access("/sys/module/ged", F_OK) == 0 ||
            access("/proc/gpufreqv2", F_OK) == 0 ||
            access("/sys/kernel/helio-dvfsrc", F_OK) == 0);
}

bool is_snapdragon() {
    std::string dt_model = read_sysfs("/proc/device-tree/model");
    std::string dt_compat = read_sysfs("/proc/device-tree/compatible");
    std::string dt = dt_model + " " + dt_compat;
    std::transform(dt.begin(), dt.end(), dt.begin(), ::tolower);
    if (dt.find("qcom") != std::string::npos || dt.find("qualcomm") != std::string::npos || dt.find("snapdragon") != std::string::npos) return true;

    return (access("/sys/class/kgsl/kgsl-3d0", F_OK) == 0 || 
            access("/dev/kgsl-3d0", F_OK) == 0 ||
            access("/sys/devices/soc0/qcom,chip-id", F_OK) == 0);
}

bool is_unisoc() {
    std::string dt_model = read_sysfs("/proc/device-tree/model");
    std::string dt_compat = read_sysfs("/proc/device-tree/compatible");
    std::string dt = dt_model + " " + dt_compat;
    std::transform(dt.begin(), dt.end(), dt.begin(), ::tolower);
    if (dt.find("unisoc") != std::string::npos || 
        dt.find("sprd") != std::string::npos || 
        dt.find("ums") != std::string::npos ||
        dt.find("sp9") != std::string::npos ||
        dt.find("sp7") != std::string::npos) {
        return true;
    }

    return (access("/sys/module/sprd_mali", F_OK) == 0 ||
            access("/proc/sprd_thermal", F_OK) == 0 ||
            access("/sys/class/devfreq/unisoc-gpu", F_OK) == 0 ||
            access("/sys/class/devfreq/scene-frequency", F_OK) == 0 ||
            access("/sys/devices/platform/soc/soc:gpu", F_OK) == 0);
}

SocType get_soc_type() {
    if (is_unisoc()) return SocType::UNISOC;
    if (is_mediatek()) return SocType::MEDIATEK;
    if (is_snapdragon()) return SocType::SNAPDRAGON;
    return SocType::GENERIC;
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
    int b_temp = 0;
    std::string raw_b = trim(read_sysfs("/sys/class/power_supply/battery/temp"));
    if (!raw_b.empty()) {
        try {
            long t = std::stol(raw_b);
            if (t > 10000) b_temp = (int)(t / 1000);
            else if (t > 100) b_temp = (int)(t / 10);
            else b_temp = (int)t;
        } catch (...) {}
    }
    return b_temp;
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

void renice_game(int pid, bool boost) {
    if (pid <= 0) return;
    if (boost) {
        std::string cmd = "renice -n -10 -p " + std::to_string(pid) + " >/dev/null 2>&1 &";
        system(cmd.c_str());
        std::string oom = "echo -500 > /proc/" + std::to_string(pid) + "/oom_score_adj 2>/dev/null &";
        system(oom.c_str());
    } else {
        std::string cmd = "renice -n 0 -p " + std::to_string(pid) + " >/dev/null 2>&1 &";
        system(cmd.c_str());
        std::string oom = "echo 0 > /proc/" + std::to_string(pid) + "/oom_score_adj 2>/dev/null &";
        system(oom.c_str());
    }
}

}
