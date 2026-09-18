#include "profile.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

namespace Profile {

struct CpuNode {
    std::string policy_path;
    std::string stock_gov;
    std::string stock_min_freq;
    std::string stock_max_freq;
    long hardware_min = 0;
    long hardware_max = 0;
    std::vector<long> available_freqs;
};

struct BlockNode {
    std::string queue_path;
    std::string stock_scheduler;
    std::string stock_read_ahead;
    std::string stock_nr_requests;
};

struct DevfreqNode {
    std::string path;
    std::string default_gov;
    std::string default_min_freq;
    std::string default_max_freq;
    std::string hardware_min_freq;
    std::string hardware_max_freq;
    bool detected = false;
};

struct KgslContext {
    std::string path;
    std::string default_min_pwrlevel;
    std::string default_max_pwrlevel;
    std::string default_idle_timer;
    bool detected = false;
};

struct BusDcvsNode {
    std::string path;
    std::string default_min_freq;
    std::string hardware_max_freq;
};

static bool g_in_performance = false;
static int g_lite_tier = 0;
static SocType g_soc = SocType::GENERIC;
static std::vector<CpuNode> g_cpu_nodes;
static std::vector<BlockNode> g_block_nodes;
static DevfreqNode g_generic_gpu;
static std::string g_power_policy_path = "";
static std::string g_opp_v2_path = "";

static std::string g_stock_vfs = "100";
static std::string g_stock_swappiness = "60";
static std::string g_stock_dirty_ratio = "20";

static KgslContext g_kgsl;
static std::vector<BusDcvsNode> g_bus_nodes;

static inline void w(const std::string& p, const std::string& v) { Utils::write_sysfs(p, v); }
static inline void wv(const std::string& p, const std::string& v, int retries, bool lock) { Utils::write_sysfs_verify(p, v, retries, lock); }
static inline void wv(const std::string& p, const std::string& v, bool lock = false) { Utils::write_sysfs_verify(p, v, 5, lock); }
static inline void wif(const std::string& p, const std::string& v) { if (access(p.c_str(), F_OK) == 0) w(p, v); }
static inline bool has(const std::string& p) { return access(p.c_str(), F_OK) == 0; }

static long find_nearest_freq(const std::vector<long>& freqs, long target) {
    if (freqs.empty()) return target;
    for (long f : freqs) if (f >= target) return f;
    return freqs.back();
}

static void probe_devfreq_node(const std::string& path, DevfreqNode& node) {
    node.path = path;
    node.default_gov = Utils::trim(Utils::read_sysfs(path + "/governor"));
    node.default_min_freq = Utils::trim(Utils::read_sysfs(path + "/min_freq"));
    node.default_max_freq = Utils::trim(Utils::read_sysfs(path + "/max_freq"));

    std::string avail = Utils::read_sysfs(path + "/available_frequencies");
    if (!avail.empty()) {
        std::stringstream ss(avail);
        uint64_t f;
        uint64_t mn = UINT64_MAX, mx = 0;
        while (ss >> f) {
            if (f < mn) mn = f;
            if (f > mx) mx = f;
        }
        node.hardware_min_freq = (mn != UINT64_MAX) ? std::to_string(mn) : node.default_min_freq;
        node.hardware_max_freq = (mx > 0) ? std::to_string(mx) : node.default_max_freq;
    } else {
        node.hardware_min_freq = node.default_min_freq;
        node.hardware_max_freq = node.default_max_freq;
    }

    if (node.default_gov.empty() || node.default_gov == "performance") {
        node.default_gov = "simple_ondemand";
    }
    node.detected = true;
}

static void discover_nodes() {
    g_soc = Utils::get_soc_type();
    g_cpu_nodes.clear();
    g_block_nodes.clear();
    g_bus_nodes.clear();

    std::string vfs = Utils::trim(Utils::read_sysfs("/proc/sys/vm/vfs_cache_pressure"));
    if (!vfs.empty()) g_stock_vfs = vfs;
    std::string swp = Utils::trim(Utils::read_sysfs("/proc/sys/vm/swappiness"));
    if (!swp.empty()) g_stock_swappiness = swp;
    std::string drt = Utils::trim(Utils::read_sysfs("/proc/sys/vm/dirty_ratio"));
    if (!drt.empty()) g_stock_dirty_ratio = drt;

    if (DIR* cpu_dir = opendir("/sys/devices/system/cpu/cpufreq")) {
        while (dirent* e = readdir(cpu_dir)) {
            std::string name = e->d_name;
            if (name.rfind("policy", 0) != 0) continue;

            CpuNode n;
            n.policy_path    = "/sys/devices/system/cpu/cpufreq/" + name;
            n.stock_gov      = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_governor"));
            n.stock_min_freq = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_min_freq"));
            n.stock_max_freq = Utils::trim(Utils::read_sysfs(n.policy_path + "/scaling_max_freq"));

            std::stringstream ss(Utils::read_sysfs(n.policy_path + "/scaling_available_frequencies"));
            for (long f; ss >> f; ) n.available_freqs.push_back(f);
            std::sort(n.available_freqs.begin(), n.available_freqs.end());

            if (!n.available_freqs.empty()) {
                n.hardware_min = n.available_freqs.front();
                n.hardware_max = n.available_freqs.back();
            } else {
                try {
                    std::string c_min = Utils::read_sysfs(n.policy_path + "/cpuinfo_min_freq");
                    std::string c_max = Utils::read_sysfs(n.policy_path + "/cpuinfo_max_freq");
                    n.hardware_min = std::stol(!c_min.empty() ? c_min : n.stock_min_freq);
                    n.hardware_max = std::stol(!c_max.empty() ? c_max : n.stock_max_freq);
                } catch (...) {}
            }

            if (n.stock_gov.empty()) n.stock_gov = "schedutil";
            if (n.stock_min_freq.empty()) n.stock_min_freq = std::to_string(n.hardware_min);
            if (n.stock_max_freq.empty()) n.stock_max_freq = std::to_string(n.hardware_max);

            g_cpu_nodes.push_back(n);
        }
        closedir(cpu_dir);
    }

    if (DIR* d = opendir("/sys/block")) {
        while (dirent* e = readdir(d)) {
            std::string n = e->d_name;
            if (n == "." || n == ".." || n.rfind("loop", 0) == 0 || n.rfind("ram", 0) == 0 || n.rfind("zram", 0) == 0) continue;

            std::string q = "/sys/block/" + n + "/queue";
            if (!has(q)) continue;

            BlockNode bn;
            bn.queue_path = q;
            bn.stock_read_ahead = Utils::trim(Utils::read_sysfs(q + "/read_ahead_kb"));
            bn.stock_nr_requests = Utils::trim(Utils::read_sysfs(q + "/nr_requests"));

            std::string raw_sched = Utils::read_sysfs(q + "/scheduler");
            size_t open_b = raw_sched.find('[');
            size_t close_b = raw_sched.find(']');
            if (open_b != std::string::npos && close_b != std::string::npos && close_b > open_b) {
                bn.stock_scheduler = raw_sched.substr(open_b + 1, close_b - open_b - 1);
            } else {
                bn.stock_scheduler = "mq-deadline";
            }
            g_block_nodes.push_back(bn);
        }
        closedir(d);
    }

    if (g_soc != SocType::UNISOC) {
        const std::vector<std::string> gpu_fallback_chain = {
            "/sys/class/devfreq/gpu",
            "/sys/class/devfreq/gpufreq"
        };
        for (const auto& path : gpu_fallback_chain) {
            if (has(path) && !g_generic_gpu.detected) {
                probe_devfreq_node(path, g_generic_gpu);
                break;
            }
        }

        if (!g_generic_gpu.detected) {
            if (DIR* df = opendir("/sys/class/devfreq")) {
                while (dirent* e = readdir(df)) {
                    std::string name = e->d_name;
                    std::string path = "/sys/class/devfreq/" + name;
                    if (name.find("mali") != std::string::npos || name.find("gpu") != std::string::npos || name.find("kgsl") != std::string::npos) {
                        probe_devfreq_node(path, g_generic_gpu);
                        break;
                    }
                }
                closedir(df);
            }
        }
    }

    if (g_soc == SocType::SNAPDRAGON) {
        if (has("/sys/class/kgsl/kgsl-3d0")) {
            g_kgsl.path = "/sys/class/kgsl/kgsl-3d0";
            g_kgsl.default_min_pwrlevel = Utils::trim(Utils::read_sysfs(g_kgsl.path + "/min_pwrlevel"));
            g_kgsl.default_max_pwrlevel = Utils::trim(Utils::read_sysfs(g_kgsl.path + "/max_pwrlevel"));
            g_kgsl.default_idle_timer   = Utils::trim(Utils::read_sysfs(g_kgsl.path + "/idle_timer"));
            g_kgsl.detected = true;
        }

        for (const char* bus : {"DDR", "LLCC", "L3"}) {
            std::string bp = std::string("/sys/devices/system/cpu/bus_dcvs/") + bus;
            if (has(bp)) {
                BusDcvsNode b;
                b.path = bp;
                b.default_min_freq = Utils::trim(Utils::read_sysfs(bp + "/hw_min_freq"));
                b.hardware_max_freq = Utils::trim(Utils::read_sysfs(bp + "/hw_max_freq"));
                g_bus_nodes.push_back(b);
            }
        }
    }

    const char* pp[] = {
        "/sys/devices/platform/soc/13000000.mali/power_policy",
        "/sys/class/misc/mali0/device/power_policy"
    };
    for (auto p : pp) {
        if (has(p)) {
            g_power_policy_path = p;
            break;
        }
    }

    if (has("/proc/gpufreqv2/fix_target_opp_index")) {
        g_opp_v2_path = "/proc/gpufreqv2/fix_target_opp_index";
    }
}

static void apply_cpu(int mode, bool is_lite = false) {
    for (const auto& c : g_cpu_nodes) {
        if (c.hardware_max == 0) continue;
        const std::string& P = c.policy_path;

        chmod((P + "/scaling_min_freq").c_str(), 0666);
        chmod((P + "/scaling_max_freq").c_str(), 0666);
        chmod((P + "/scaling_governor").c_str(), 0666);

        if (mode == 1) {
            if (is_lite) {
                w(P + "/scaling_min_freq", std::to_string(c.hardware_min));
                w(P + "/scaling_max_freq", std::to_string(c.hardware_max));

                wv(P + "/scaling_governor", "schedutil", 5, false);
                wif(P + "/schedutil/up_rate_limit_us", "800");
                wif(P + "/schedutil/down_rate_limit_us", "2500");
                wif(P + "/schedutil/rate_limit_us", "1000");

                double ceil_ratio = 0.75;
                if (g_lite_tier == 1)      ceil_ratio = 0.75 * 0.95;
                else if (g_lite_tier == 2) ceil_ratio = 0.75 * 0.90;
                else if (g_lite_tier == 3) ceil_ratio = 0.75 * 0.85;
                else if (g_lite_tier == 4) ceil_ratio = 0.75 * 0.75;

                long floor_t = c.hardware_min + (long)((c.hardware_max - c.hardware_min) * 0.35);
                long ceil_t  = c.hardware_min + (long)((c.hardware_max - c.hardware_min) * ceil_ratio);

                wv(P + "/scaling_max_freq", std::to_string(find_nearest_freq(c.available_freqs, ceil_t)), 5, true);
                wv(P + "/scaling_min_freq", std::to_string(find_nearest_freq(c.available_freqs, floor_t)), 5, true);
            } else {
                bool has_perf = Utils::read_sysfs(P + "/scaling_available_governors").find("performance") != std::string::npos;
                wv(P + "/scaling_max_freq", std::to_string(c.hardware_max), 5, true);
                wv(P + "/scaling_min_freq", std::to_string(c.hardware_max), 5, true);
                wv(P + "/scaling_governor", has_perf ? "performance" : "schedutil", 5, false);
                wif(P + "/schedutil/rate_limit_us", "500");
            }
        } 
        else if (mode == 0) {
            w(P + "/scaling_min_freq", std::to_string(c.hardware_min));
            w(P + "/scaling_max_freq", std::to_string(c.hardware_max));

            wv(P + "/scaling_governor", c.stock_gov, 5, false);
            wv(P + "/scaling_max_freq", c.stock_max_freq, 5, false);
            wv(P + "/scaling_min_freq", c.stock_min_freq, 5, false);
        } 
        else if (mode == 3) {
            w(P + "/scaling_min_freq", std::to_string(c.hardware_min));
            w(P + "/scaling_max_freq", std::to_string(c.hardware_max));

            wv(P + "/scaling_governor", c.stock_gov, 5, false);
            long t = c.hardware_min + (long)((c.hardware_max - c.hardware_min) * 0.45);
            std::string target_min = (c.hardware_min > 0) ? std::to_string(c.hardware_min) : c.stock_min_freq;

            wv(P + "/scaling_max_freq", std::to_string(find_nearest_freq(c.available_freqs, t)), 5, false);
            wv(P + "/scaling_min_freq", target_min, 5, false);
            wif(P + "/schedutil/rate_limit_us", "2500");
        }
    }
}

static void set_io_latency(bool low_latency) {
    if (low_latency) {
        for (const auto& b : g_block_nodes) {
            bool is_emmc = (b.queue_path.find("mmcblk") != std::string::npos);
            w(b.queue_path + "/read_ahead_kb", is_emmc ? "256" : "32");
            w(b.queue_path + "/nr_requests", is_emmc ? "128" : "32");
            w(b.queue_path + "/scheduler", "mq-deadline");
        }
    } else {
        for (const auto& b : g_block_nodes) {
            if (!b.stock_read_ahead.empty()) w(b.queue_path + "/read_ahead_kb", b.stock_read_ahead);
            if (!b.stock_nr_requests.empty()) w(b.queue_path + "/nr_requests", b.stock_nr_requests);
            if (!b.stock_scheduler.empty()) w(b.queue_path + "/scheduler", b.stock_scheduler);
        }
    }
}

static void set_kernel_tweaks(int mode, bool is_lite = false) {
    if (mode == 1) {
        w("/dev/cpuctl/top-app/cpu.uclamp.latency_sensitive", "1");
        w("/dev/cpuctl/top-app/cpu.uclamp.min", is_lite ? "25" : "40");
        w("/proc/sys/vm/swappiness", "30");
        w("/proc/sys/vm/dirty_ratio", "15");
        w("/proc/sys/vm/vfs_cache_pressure", "60");
        system("stop logd 2>/dev/null");
    } else {
        w("/dev/cpuctl/top-app/cpu.uclamp.latency_sensitive", "0");
        w("/dev/cpuctl/top-app/cpu.uclamp.min", "0");
        w("/proc/sys/vm/swappiness", g_stock_swappiness);
        w("/proc/sys/vm/dirty_ratio", g_stock_dirty_ratio);
        w("/proc/sys/vm/vfs_cache_pressure", g_stock_vfs);
        system("start logd 2>/dev/null");
    }
}

static const std::vector<std::string> kLimiters = {
    "ignore_batt_oc", "ignore_batt_percent",
    "ignore_low_batt", "ignore_thermal_protect", "ignore_pbm_limited"
};

static void apply_mediatek_performance(bool is_lite) {
    if (!g_opp_v2_path.empty()) {
        w(g_opp_v2_path, "-1");
        if (!is_lite) w(g_opp_v2_path, "0");
    } else if (has("/proc/gpufreq/gpufreq_opp_freq")) {
        w("/proc/gpufreq/gpufreq_opp_freq", "0");
    }

    for (const auto& l : kLimiters) {
        w("/proc/gpufreq/gpufreq_power_limited", l + " 1");
    }

    if (!g_power_policy_path.empty()) {
        chmod(g_power_policy_path.c_str(), 0644);
        w(g_power_policy_path, is_lite ? "coarse_demand" : "always_on");
    }
    w("/proc/mtk_batoc_throttling/battery_oc_protect_stop", "stop 1");

    chmod("/sys/kernel/helio-dvfsrc/dvfsrc_force_vcore_dvfs_opp", 0666);
    w("/sys/kernel/helio-dvfsrc/dvfsrc_force_vcore_dvfs_opp", "0");

    chmod("/sys/kernel/helio-dvfsrc/dvfsrc_force_ddr_opp", 0666);
    wif("/sys/kernel/helio-dvfsrc/dvfsrc_force_ddr_opp", "0");

    wif("/sys/kernel/helio-dvfsrc/dvfsrc_req_ddr_opp", "0");
    wif("/sys/devices/platform/boot_dramboost/dramboost/dramboost", "1");
    wif("/proc/cpufreq/cpufreq_cci_mode", "1");
    wif("/proc/cpufreq/cpufreq_power_mode", "3");
    wif("/sys/devices/system/cpu/eas/enable", "0");

    wif("/sys/kernel/fpsgo/common/force_onoff", "0");
    wif("/sys/kernel/eara_thermal/enable", "0");
    wif("/proc/ppm/policy/pwr_thro", "0");
    wif("/proc/ppm/policy/thermal", "0");

    w("/sys/module/sspm_v3/holders/ged/parameters/is_GED_KPI_enabled", "0");
    w("/sys/module/ged/parameters/ged_boost_enable", "1");
    w("/sys/module/ged/parameters/boost_gpu_enable", "1");
    w("/sys/module/ged/parameters/boost_extra", is_lite ? "20" : "50");
    w("/sys/module/ged/parameters/gpu_idle", "0");
}

static void restore_mediatek_balanced() {
    if (!g_opp_v2_path.empty()) w(g_opp_v2_path, "-1");
    for (const auto& l : kLimiters) w("/proc/gpufreq/gpufreq_power_limited", l + " 0");
    if (!g_power_policy_path.empty()) w(g_power_policy_path, "coarse_demand");
    w("/proc/mtk_batoc_throttling/battery_oc_protect_stop", "stop 0");

    chmod("/sys/kernel/helio-dvfsrc/dvfsrc_force_vcore_dvfs_opp", 0666);
    w("/sys/kernel/helio-dvfsrc/dvfsrc_force_vcore_dvfs_opp", "-1");
    wif("/sys/kernel/helio-dvfsrc/dvfsrc_force_vcore_dvfs_opp", "255");

    chmod("/sys/kernel/helio-dvfsrc/dvfsrc_force_ddr_opp", 0666);
    wif("/sys/kernel/helio-dvfsrc/dvfsrc_force_ddr_opp", "-1");
    wif("/sys/kernel/helio-dvfsrc/dvfsrc_force_ddr_opp", "255");

    wif("/sys/kernel/helio-dvfsrc/dvfsrc_req_ddr_opp", "-1");
    wif("/sys/devices/platform/boot_dramboost/dramboost/dramboost", "0");
    wif("/proc/cpufreq/cpufreq_cci_mode", "0");
    wif("/proc/cpufreq/cpufreq_power_mode", "0");
    wif("/sys/devices/system/cpu/eas/enable", "1");

    wif("/sys/kernel/fpsgo/common/force_onoff", "1");
    wif("/sys/kernel/eara_thermal/enable", "1");
    wif("/proc/ppm/policy/pwr_thro", "1");
    wif("/proc/ppm/policy/thermal", "1");

    w("/sys/module/sspm_v3/holders/ged/parameters/is_GED_KPI_enabled", "1");
    w("/sys/module/ged/parameters/ged_boost_enable", "0");
    w("/sys/module/ged/parameters/boost_gpu_enable", "0");
    w("/sys/module/ged/parameters/boost_extra", "0");
    w("/sys/module/ged/parameters/gpu_idle", "1");
}

static void apply_snapdragon_performance(bool is_lite) {
    if (g_kgsl.detected) {
        wif(g_kgsl.path + "/throttling", "0");
        wif(g_kgsl.path + "/force_clk_on", "1");
        wif(g_kgsl.path + "/force_bus_on", "1");
        wif(g_kgsl.path + "/force_rail_on", "1");
        wif(g_kgsl.path + "/force_no_nap", "1");
        wif(g_kgsl.path + "/bus_split", "0");
        wif(g_kgsl.path + "/idle_timer", "1000");

        if (!is_lite) {
            wif(g_kgsl.path + "/max_pwrlevel", "0");
            wif(g_kgsl.path + "/min_pwrlevel", "0");
        }
    }

    for (const auto& b : g_bus_nodes) {
        if (!b.hardware_max_freq.empty()) {
            wif(b.path + "/hw_min_freq", b.hardware_max_freq);
        }
    }

    if (g_generic_gpu.detected && !is_lite && !g_generic_gpu.hardware_max_freq.empty()) {
        w(g_generic_gpu.path + "/governor", "performance");
        w(g_generic_gpu.path + "/min_freq", g_generic_gpu.hardware_max_freq);
    }

    wif("/sys/module/msm_thermal/parameters/enabled", "N");
    wif("/sys/module/msm_thermal/core_control/enabled", "0");
}

static void restore_snapdragon_balanced() {
    if (g_kgsl.detected) {
        wif(g_kgsl.path + "/throttling", "1");
        wif(g_kgsl.path + "/force_clk_on", "0");
        wif(g_kgsl.path + "/force_bus_on", "0");
        wif(g_kgsl.path + "/force_rail_on", "0");
        wif(g_kgsl.path + "/force_no_nap", "0");
        wif(g_kgsl.path + "/bus_split", "1");
        wif(g_kgsl.path + "/idle_timer", g_kgsl.default_idle_timer.empty() ? "80" : g_kgsl.default_idle_timer);

        if (!g_kgsl.default_min_pwrlevel.empty()) {
            wif(g_kgsl.path + "/min_pwrlevel", g_kgsl.default_min_pwrlevel);
        }
        if (!g_kgsl.default_max_pwrlevel.empty()) {
            wif(g_kgsl.path + "/max_pwrlevel", g_kgsl.default_max_pwrlevel);
        }
    }

    for (const auto& b : g_bus_nodes) {
        if (!b.default_min_freq.empty()) {
            wif(b.path + "/hw_min_freq", b.default_min_freq);
        }
    }

    if (g_generic_gpu.detected) {
        chmod((g_generic_gpu.path + "/governor").c_str(), 0666);
        chmod((g_generic_gpu.path + "/min_freq").c_str(), 0666);
        std::string g_gov = g_generic_gpu.default_gov;
        if (g_gov.empty() || g_gov == "performance") g_gov = "simple_ondemand";
        wv(g_generic_gpu.path + "/governor", g_gov, 5, false);

        std::string g_min = !g_generic_gpu.hardware_min_freq.empty() ? g_generic_gpu.hardware_min_freq : g_generic_gpu.default_min_freq;
        if (!g_min.empty()) {
            wv(g_generic_gpu.path + "/min_freq", g_min, 5, false);
        }
    }

    wif("/sys/module/msm_thermal/parameters/enabled", "Y");
    wif("/sys/module/msm_thermal/core_control/enabled", "1");
}

void set_lite_thermal_tier(int tier) {
    if (g_lite_tier == tier) return;
    g_lite_tier = tier;
    if (g_in_performance) {
        apply_cpu(1, true);
    }
}

void init() {
    discover_nodes();
    std::string soc_str = "Generic";
    if (g_soc == SocType::MEDIATEK) soc_str = "MediaTek";
    else if (g_soc == SocType::SNAPDRAGON) soc_str = "Snapdragon";
    else if (g_soc == SocType::UNISOC) soc_str = "Unisoc";

    std::cout << "[PROFILE] SoC Terdeteksi: " << soc_str << std::endl;
}

void apply_boot_tweaks() {
    w("/proc/sys/kernel/panic_on_oops", "0");
    w("/proc/sys/kernel/panic_on_warn", "0");

    w("/proc/sys/vm/compaction_proactiveness", "0");
    w("/proc/sys/vm/watermark_boost_factor", "0");
    w("/proc/sys/vm/stat_interval", "15");

    w("/proc/sys/net/ipv4/tcp_fastopen", "3");
    w("/proc/sys/net/ipv4/tcp_autocorking", "1");
    wif("/proc/sys/net/core/default_qdisc", "fq_codel");

    std::string tcp_avail = Utils::read_sysfs("/proc/sys/net/ipv4/tcp_available_congestion_control");
    std::vector<std::string> tcp_priority = {"bbr3", "bbr2", "bbr", "westwood", "cubic"};
    for (const auto& algo : tcp_priority) {
        if (tcp_avail.find(algo) != std::string::npos) {
            w("/proc/sys/net/ipv4/tcp_congestion_control", algo);
            break;
        }
    }

    const std::vector<std::string> sched_nodes = {
        "/proc/sys/kernel/sched_lib_name",
        "/proc/sys/walt/sched_lib_name",
        "/sys/devices/system/cpu/sched_lib_name"
    };
    std::string engine_libs = "UnityMain,libunity.so,libmain.so,libUE4.so,libUE5.so,libUnreal.so,libgodot_android.so,libil2cpp.so,libcocos2dcpp.so";
    for (const auto& node : sched_nodes) {
        if (has(node)) {
            w(node, engine_libs);
            wif(node + "_mask_force", "255");
        }
    }

    if (DIR* td = opendir("/sys/class/thermal")) {
        while (dirent* te = readdir(td)) {
            std::string tn = te->d_name;
            if (tn.rfind("thermal_zone", 0) != 0) continue;

            std::string tz = "/sys/class/thermal/" + tn;
            std::string avail_govs = Utils::read_sysfs(tz + "/available_policies");
            if (avail_govs.find("step_wise") != std::string::npos) {
                w(tz + "/policy", "step_wise");
            }
        }
        closedir(td);
    }
}

void apply_performance(bool is_lite, bool dis_thermal) {
    apply_cpu(1, is_lite);
    set_io_latency(true);
    set_kernel_tweaks(1, is_lite);

    if (g_soc != SocType::UNISOC) {
        w("/sys/class/thermal/thermal_zone0/mode", (dis_thermal && !is_lite) ? "disabled" : "enabled");
    }

    switch (g_soc) {
        case SocType::MEDIATEK:
            apply_mediatek_performance(is_lite);
            break;
        case SocType::SNAPDRAGON:
            apply_snapdragon_performance(is_lite);
            break;
        case SocType::UNISOC:
            break;
        default:
            if (g_generic_gpu.detected && !is_lite && !g_generic_gpu.hardware_max_freq.empty()) {
                w(g_generic_gpu.path + "/governor", "performance");
                w(g_generic_gpu.path + "/min_freq", g_generic_gpu.hardware_max_freq);
            }
            break;
    }

    g_in_performance = true;
}

void restore_balanced() {
    g_lite_tier = 0;
    apply_cpu(0);
    set_io_latency(false);
    set_kernel_tweaks(0);

    if (g_soc != SocType::UNISOC) {
        w("/sys/class/thermal/thermal_zone0/mode", "enabled");
    }

    switch (g_soc) {
        case SocType::MEDIATEK:
            restore_mediatek_balanced();
            break;
        case SocType::SNAPDRAGON:
            restore_snapdragon_balanced();
            break;
        case SocType::UNISOC:
            break;
        default:
            if (g_generic_gpu.detected) {
                chmod((g_generic_gpu.path + "/governor").c_str(), 0666);
                chmod((g_generic_gpu.path + "/min_freq").c_str(), 0666);
                std::string g_gov = g_generic_gpu.default_gov;
                if (g_gov.empty() || g_gov == "performance") g_gov = "simple_ondemand";
                wv(g_generic_gpu.path + "/governor", g_gov, 5, false);

                std::string g_min = !g_generic_gpu.hardware_min_freq.empty() ? g_generic_gpu.hardware_min_freq : g_generic_gpu.default_min_freq;
                if (!g_min.empty()) {
                    wv(g_generic_gpu.path + "/min_freq", g_min, 5, false);
                }
            }
            break;
    }

    g_in_performance = false;
}

void apply_powersave() {
    g_lite_tier = 0;
    apply_cpu(3);
    set_io_latency(false);
    set_kernel_tweaks(3);
    w("/sys/class/thermal/thermal_zone0/mode", "enabled");

    if (g_soc == SocType::MEDIATEK) {
        restore_mediatek_balanced();
    } else if (g_soc == SocType::SNAPDRAGON) {
        restore_snapdragon_balanced();
    }

    g_in_performance = false;
}

bool is_in_performance() {
    return g_in_performance;
}

}
