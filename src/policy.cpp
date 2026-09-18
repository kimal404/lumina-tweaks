#include "policy.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

namespace Policy {

static std::vector<CpuNode> g_cpus;
static double g_current_ratio = 1.0;
static bool g_in_throttle = false;

static std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::string out;
    std::getline(file, out);
    size_t first = out.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = out.find_last_not_of(" \t\n\r");
    return out.substr(first, last - first + 1);
}

static bool write_file(const std::string& path, const std::string& val) {
    chmod(path.c_str(), 0666);
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << val;
    return true;
}

static long find_nearest(const std::vector<long>& freqs, long target) {
    if (freqs.empty()) return target;
    for (long f : freqs) if (f >= target) return f;
    return freqs.back();
}

void init() {
    g_cpus.clear();
    DIR* dir = opendir("/sys/devices/system/cpu/cpufreq");
    if (!dir) return;

    while (dirent* e = readdir(dir)) {
        std::string name = e->d_name;
        if (name.rfind("policy", 0) != 0) continue;

        CpuNode n;
        n.path = "/sys/devices/system/cpu/cpufreq/" + name;
        n.stock_gov = read_file(n.path + "/scaling_governor");
        n.stock_min = read_file(n.path + "/scaling_min_freq");
        n.stock_max = read_file(n.path + "/scaling_max_freq");

        std::stringstream ss(read_file(n.path + "/scaling_available_frequencies"));
        for (long f; ss >> f; ) n.freqs.push_back(f);
        std::sort(n.freqs.begin(), n.freqs.end());

        if (!n.freqs.empty()) {
            n.hw_min = n.freqs.front();
            n.hw_max = n.freqs.back();
        } else {
            try {
                n.hw_min = std::stol(read_file(n.path + "/cpuinfo_min_freq"));
                n.hw_max = std::stol(read_file(n.path + "/cpuinfo_max_freq"));
            } catch (...) {}
        }

        if (n.stock_gov.empty()) n.stock_gov = "schedutil";
        if (n.stock_min.empty()) n.stock_min = std::to_string(n.hw_min);
        if (n.stock_max.empty()) n.stock_max = std::to_string(n.hw_max);

        g_cpus.push_back(n);
    }
    closedir(dir);
}

void update(float temp_c, float cpu_load, bool is_lite) {
    (void)cpu_load;
    double target_ratio = 1.0;

    if (temp_c < 37.0f) {
        g_in_throttle = false;
        target_ratio = 1.0;
    } else if (temp_c >= 37.0f && temp_c < 39.0f) {
        target_ratio = g_in_throttle ? 0.95 : 1.0;
    } else if (temp_c >= 39.0f && temp_c <= 40.0f) {
        target_ratio = 0.90;
        g_in_throttle = true;
    } else if (temp_c > 40.0f && temp_c <= 43.0f) {
        target_ratio = 0.85;
        g_in_throttle = true;
    } else if (temp_c > 43.0f) {
        target_ratio = 0.75;
        g_in_throttle = true;
    }

    if (is_lite) {
        target_ratio *= 0.75;
    }

    if (g_current_ratio > target_ratio) {
        g_current_ratio = std::max(target_ratio, g_current_ratio - 0.05);
    } else if (g_current_ratio < target_ratio) {
        g_current_ratio = std::min(target_ratio, g_current_ratio + 0.05);
    }

    for (const auto& c : g_cpus) {
        if (c.hw_max == 0) continue;

        write_file(c.path + "/scaling_min_freq", std::to_string(c.hw_min));
        write_file(c.path + "/scaling_max_freq", std::to_string(c.hw_max));

        write_file(c.path + "/scaling_governor", "schedutil");

        long floor_f = c.hw_min + (long)((c.hw_max - c.hw_min) * 0.30);
        long ceil_f  = c.hw_min + (long)((c.hw_max - c.hw_min) * g_current_ratio);

        long clamped_ceil  = find_nearest(c.freqs, ceil_f);
        long clamped_floor = find_nearest(c.freqs, floor_f);

        if (clamped_floor > clamped_ceil) clamped_floor = clamped_ceil;

        write_file(c.path + "/scaling_max_freq", std::to_string(clamped_ceil));
        write_file(c.path + "/scaling_min_freq", std::to_string(clamped_floor));
    }
}

void restore_stock() {
    for (const auto& c : g_cpus) {
        write_file(c.path + "/scaling_min_freq", std::to_string(c.hw_min));
        write_file(c.path + "/scaling_max_freq", std::to_string(c.hw_max));
        write_file(c.path + "/scaling_governor", c.stock_gov);
        write_file(c.path + "/scaling_max_freq", c.stock_max_freq);
        write_file(c.path + "/scaling_min_freq", c.stock_min_freq);
    }
    g_current_ratio = 1.0;
    g_in_throttle = false;
}

}
