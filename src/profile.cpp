#include "profile.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

namespace Profile {

struct CpuNode {
    std::string policy_path, default_gov, default_min_freq, default_max_freq;
    long hardware_min = 0, hardware_max = 0;
    std::vector<long> available_freqs;
};

struct GpuContext {
    std::string devfreq_path, default_gov, default_min_freq, max_freq;
    std::string power_policy_path, opp_v2_path;
    bool detected = false;
};

static bool g_in_performance = false;
static bool g_has_mtk = false;
static std::vector<CpuNode> g_cpu_nodes;
static GpuContext g_gpu;

static inline void w(const std::string& p, const std::string& v) { Utils::write_sysfs(p, v); }
static inline void wif(const std::string& p, const std::string& v) { if (access(p.c_str(), F_OK) == 0) w(p, v); }
static inline bool has(const std::string& p) { return access(p.c_str(), F_OK) == 0; }

static long find_nearest_freq(const std::vector<long>& freqs, long target) {
    if (freqs.empty()) return target;
    for (long f : freqs) if (f >= target) return f;
    return freqs.back();
}

static void discover_nodes() {
    g_has_mtk = Utils::is_mediatek();
    g_cpu_nodes.clear();

    if (DIR* cpu_dir = opendir("/sys/devices/system/cpu/cpufreq")) {
        while (dirent* e = readdir(cpu_dir)) {
            std::string name = e->d_name;
            if (name.rfind("policy", 0) != 0) continue;

            CpuNode n;
            n.policy_path      = "/sys/devices/system/cpu/cpufreq/" + name;
            n.default_gov      = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_governor"));
            n.default_min_freq = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_min_freq"));
            n.default_max_freq = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_max_freq"));

            std::stringstream ss(Utils::read_sysfs(n.policy_path + "/scaling_available_frequencies"));
            for (long f; ss >> f; ) n.available_freqs.push_back(f);
            std::sort(n.available_freqs.begin(), n.available_freqs.end());

            if (!n.available_freqs.empty()) {
                n.hardware_min = n.available_freqs.front();
                n.hardware_max = n.available_freqs.back();
            } else {
                try {
                    n.hardware_min = std::stol(n.default_min_freq);
                    n.hardware_max = std::stol(n.default_max_freq);
                } catch (...) {}
            }
            if (n.default_gov.empty()) n.default_gov = "schedutil";
            g_cpu_nodes.push_back(n);
        }
        closedir(cpu_dir);
    }

    if (DIR* df = opendir("/sys/class/devfreq")) {
        while (dirent* e = readdir(df)) {
            std::string name = e->d_name;
            if (name.find("mali") == std::string::npos &&
                name.find("gpu")  == std::string::npos &&
                name.find("kgsl") == std::string::npos) continue;

            g_gpu.devfreq_path     = "/sys/class/devfreq/" + name;
            g_gpu.default_gov      = Utils::trim(Utils::read_sysfs(g_gpu.devfreq_path + "/governor"));
            g_gpu.default_min_freq = Utils::trim(Utils::read_sysfs(g_gpu.devfreq_path + "/min_freq"));

            std::string avail = Utils::read_sysfs(g_gpu.devfreq_path + "/available_frequencies");
            if (!avail.empty()) {
                std::stringstream ss(avail); uint64_t f, mx = 0;
                while (ss >> f) mx = std::max(mx, f);
                g_gpu.max_freq = std::to_string(mx);
            } else {
                g_gpu.max_freq = Utils::trim(Utils::read_sysfs(g_gpu.devfreq_path + "/max_freq"));
            }
            if (g_gpu.default_gov.empty()) g_gpu.default_gov = "simple_ondemand";
            g_gpu.detected = true;
            break;
        }
        closedir(df);
    }

    const char* pp[] = {
        "/sys/devices/platform/soc/13000000.mali/power_policy",
        "/sys/class/misc/mali0/device/power_policy"
    };
    for (auto p : pp) if (has(p)) { g_gpu.power_policy_path = p; break; }

    if (has("/proc/gpufreqv2/fix_target_opp_index")) {
        g_gpu.opp_v2_path = "/proc/gpufreqv2/fix_target_opp_index";
    }
}

static void apply_cpu(int mode, bool is_lite = false) {
    for (const auto& c : g_cpu_nodes) {
        if (c.hardware_max == 0) continue;
        const std::string& P = c.policy_path;

        if (mode == 1) {
            if (is_lite) {
                w(P + "/scaling_governor", "schedutil");
                long t = c.hardware_min + (long)((c.hardware_max - c.hardware_min) * 0.65);
                w(P + "/scaling_max_freq", std::to_string(c.hardware_max));
                w(P + "/scaling_min_freq", std::to_string(find_nearest_freq(c.available_freqs, t)));
                w(P + "/schedutil/rate_limit_us", "1000");
            } else {
                bool has_perf = Utils::read_sysfs(P + "/scaling_available_governors").find("performance") != std::string::npos;
                w(P + "/scaling_governor", has_perf ? "performance" : "schedutil");
                w(P + "/scaling_max_freq", std::to_string(c.hardware_max));
                w(P + "/scaling_min_freq", std::to_string(c.hardware_max));
                w(P + "/schedutil/rate_limit_us", "500");
            }
        } else if (mode == 0) {
            w(P + "/scaling_governor", c.default_gov);
            w(P + "/scaling_min_freq", c.default_min_freq);
            w(P + "/scaling_max_freq", c.default_max_freq);
            w(P + "/schedutil/rate_limit_us", "2000");
        } else if (mode == 3) {
            w(P + "/scaling_governor", c.default_gov);
            long t = c.hardware_min + (long)((c.hardware_max - c.hardware_min) * 0.45);
            w(P + "/scaling_min_freq", c.default_min_freq);
            w(P + "/scaling_max_freq", std::to_string(find_nearest_freq(c.available_freqs, t)));
            w(P + "/schedutil/rate_limit_us", "4000");
        }
    }
}

static const std::vector<std::string> kLimiters = {
    "ignore_batt_oc", "ignore_batt_percent",
    "ignore_low_batt", "ignore_thermal_protect", "ignore_pbm_limited"
};

static void apply_gpu_profile(bool enable, bool is_lite) {
    if (g_has_mtk) {
        if (!g_gpu.opp_v2_path.empty()) {
            w(g_gpu.opp_v2_path, "-1");
            if (enable && !is_lite) w(g_gpu.opp_v2_path, "0");
        } else if (has("/proc/gpufreq/gpufreq_opp_freq")) {
            w("/proc/gpufreq/gpufreq_opp_freq", "0");
        }
        for (const auto& l : kLimiters) {
            w("/proc/gpufreq/gpufreq_power_limited", l + (enable ? " 1" : " 0"));
        }
        if (!g_gpu.power_policy_path.empty()) {
            chmod(g_gpu.power_policy_path.c_str(), 0644);
            w(g_gpu.power_policy_path, (enable && !is_lite) ? "always_on" : "coarse_demand");
        }
        w("/proc/mtk_batoc_throttling/battery_oc_protect_stop", enable ? "stop 1" : "stop 0");
        w("/sys/module/sspm_v3/holders/ged/parameters/is_GED_KPI_enabled", enable ? "0" : "1");
    }

    if (g_gpu.detected) {
        if (enable && !is_lite && !g_gpu.max_freq.empty()) {
            w(g_gpu.devfreq_path + "/governor", "performance");
            w(g_gpu.devfreq_path + "/min_freq", g_gpu.max_freq);
        } else {
            w(g_gpu.devfreq_path + "/governor", g_gpu.default_gov);
            w(g_gpu.devfreq_path + "/min_freq", g_gpu.default_min_freq);
        }
    }
}

static void set_kernel_tweaks(int mode, bool is_lite = false) {
    bool perf = (mode == 1);
    w("/dev/cpuctl/top-app/cpu.uclamp.latency_sensitive", perf ? "1" : "0");
    w("/dev/cpuctl/top-app/cpu.uclamp.min", perf ? (is_lite ? "25" : "40") : "0");
    w("/proc/sys/vm/vfs_cache_pressure", perf ? "80" : (mode == 3 ? "120" : "100"));

    if (g_has_mtk) {
        wif("/proc/ppm/policy/pwr_thro", perf ? "0" : "1");
        wif("/proc/ppm/policy/thermal",  perf ? "0" : "1");
        w("/sys/module/ged/parameters/ged_boost_enable", perf ? "1" : "0");
        w("/sys/module/ged/parameters/boost_gpu_enable", perf ? "1" : "0");
        w("/sys/module/ged/parameters/boost_extra", perf ? (is_lite ? "20" : "50") : "0");
        w("/sys/module/ged/parameters/gpu_idle", perf ? "0" : "1");
    }
}

void init() {
    discover_nodes();
}

void apply_boot_tweaks() {
    if (DIR* d = opendir("/sys/block")) {
        while (dirent* e = readdir(d)) {
            std::string n = e->d_name;
            if (n == "." || n == "..") continue;
            std::string q = "/sys/block/" + n + "/queue";
            if (!has(q)) continue;
            w(q + "/scheduler", "none");
            w(q + "/iostats", "0");
            w(q + "/read_ahead_kb", "128");
            w(q + "/nr_requests", "64");
        }
        closedir(d);
    }
    w("/proc/sys/vm/compaction_proactiveness", "0");
    w("/proc/sys/vm/stat_interval", "15");
}

void apply_performance(bool is_lite, bool dis_thermal) {
    apply_cpu(1, is_lite);
    apply_gpu_profile(true, is_lite);
    set_kernel_tweaks(1, is_lite);
    w("/sys/class/thermal/thermal_zone0/mode", (dis_thermal && !is_lite) ? "disabled" : "enabled");
    g_in_performance = true;
}

void restore_balanced() {
    apply_cpu(0);
    apply_gpu_profile(false, false);
    set_kernel_tweaks(0);
    w("/sys/class/thermal/thermal_zone0/mode", "enabled");
    g_in_performance = false;
}

void apply_powersave() {
    apply_cpu(3);
    apply_gpu_profile(false, false);
    set_kernel_tweaks(3);
    w("/sys/class/thermal/thermal_zone0/mode", "enabled");
    g_in_performance = false;
}

bool is_in_performance() {
    return g_in_performance;
}

} // namespace Profile
