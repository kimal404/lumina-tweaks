#include "hardware.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

HardwareManager& HardwareManager::getInstance() {
    static HardwareManager instance;
    return instance;
}

bool HardwareManager::writeSysfs(const std::string& path, const std::string& value) {
    int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) return false;
    ssize_t bytes_written = write(fd, value.c_str(), value.size());
    close(fd);
    return bytes_written == static_cast<ssize_t>(value.size());
}

std::string HardwareManager::readSysfs(const std::string& path) {
    std::ifstream file(path);
    std::string data;
    if (file.is_open()) {
        file >> data;
    }
    return data;
}

bool HardwareManager::init() {
    probeCpu();
    probePlatformGpu();
    probeThermal();
    return !state.cpu_policies.empty();
}

PlatformType HardwareManager::getPlatform() const {
    return state.platform;
}

bool HardwareManager::probeCpu() {
    state.cpu_policies.clear();
    DIR* dir = opendir("/sys/devices/system/cpu/cpufreq");
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.' || std::string(entry->d_name).rfind("policy", 0) != 0) continue;

        CpuPolicy policy;
        policy.path = std::string("/sys/devices/system/cpu/cpufreq/") + entry->d_name;
        policy.policy_id = std::stoi(std::string(entry->d_name).substr(6));

        policy.default_gov = readSysfs(policy.path + "/scaling_governor");
        policy.default_min = std::strtoul(readSysfs(policy.path + "/scaling_min_freq").c_str(), nullptr, 10);
        policy.default_max = std::strtoul(readSysfs(policy.path + "/scaling_max_freq").c_str(), nullptr, 10);

        std::ifstream avail_gov(policy.path + "/scaling_available_governors");
        std::string gov_line;
        std::getline(avail_gov, gov_line);
        policy.has_performance_gov = (gov_line.find("performance") != std::string::npos);

        std::ifstream avail_freq(policy.path + "/scaling_available_frequencies");
        uint32_t freq, max_avail = 0;
        while (avail_freq >> freq) {
            if (freq > max_avail) max_avail = freq;
        }
        policy.hardware_max = (max_avail > 0) ? max_avail : policy.default_max;

        state.cpu_policies.push_back(policy);
    }
    closedir(dir);

    std::sort(state.cpu_policies.begin(), state.cpu_policies.end(), 
              [](const CpuPolicy& a, const CpuPolicy& b) { return a.policy_id < b.policy_id; });
    return true;
}

void HardwareManager::probePlatformGpu() {
    if (probeMediatek()) {
        state.platform = PlatformType::Mediatek;
    } else if (probeSnapdragon()) {
        state.platform = PlatformType::Snapdragon;
    } else if (probeUnisoc()) {
        state.platform = PlatformType::Unisoc;
    } else if (probeKirin()) {
        state.platform = PlatformType::Kirin;
    } else {
        state.platform = PlatformType::Unknown;
    }
}

bool HardwareManager::probeMediatek() {
    if (access("/sys/kernel/ged/hal/custom_boost_gpu_freq", W_OK) == 0) {
        state.gpu_node = "/sys/kernel/ged/hal/custom_boost_gpu_freq";
        return true;
    }
    return false;
}

bool HardwareManager::probeSnapdragon() {
    const std::string kgsl_dir = "/sys/class/kgsl/kgsl-3d0/devfreq";
    if (access((kgsl_dir + "/min_freq").c_str(), W_OK) == 0) {
        state.gpu_node = kgsl_dir;
        state.gpu_default_gov = readSysfs(kgsl_dir + "/governor");
        state.gpu_default_min = std::strtoull(readSysfs(kgsl_dir + "/min_freq").c_str(), nullptr, 10);
        state.gpu_hardware_max = std::strtoull(readSysfs(kgsl_dir + "/max_freq").c_str(), nullptr, 10);
        return true;
    }
    return false;
}

bool HardwareManager::probeUnisoc() {
    DIR* dir = opendir("/sys/class/devfreq");
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.find(".gpu") != std::string::npos || name.find("gpu") != std::string::npos) {
            std::string path = "/sys/class/devfreq/" + name;
            if (access((path + "/min_freq").c_OK | W_OK) == 0) {
                state.gpu_node = path;
                state.gpu_default_gov = readSysfs(path + "/governor");
                state.gpu_default_min = std::strtoull(readSysfs(path + "/min_freq").c_str(), nullptr, 10);
                
                std::ifstream avail_freq(path + "/available_frequencies");
                uint64_t freq, max_f = 0;
                while (avail_freq >> freq) {
                    if (freq > max_f) max_f = freq;
                }
                state.gpu_hardware_max = (max_f > 0) ? max_f : std::strtoull(readSysfs(path + "/max_freq").c_str(), nullptr, 10);
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}

bool HardwareManager::probeKirin() {
    DIR* dir = opendir("/sys/class/devfreq");
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.find("gpufreq") != std::string::npos || name.find("mali") != std::string::npos) {
            std::string path = "/sys/class/devfreq/" + name;
            if (access((path + "/min_freq").c_str(), W_OK) == 0) {
                state.gpu_node = path;
                state.gpu_default_gov = readSysfs(path + "/governor");
                state.gpu_default_min = std::strtoull(readSysfs(path + "/min_freq").c_str(), nullptr, 10);
                state.gpu_hardware_max = std::strtoull(readSysfs(path + "/max_freq").c_str(), nullptr, 10);
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}

bool HardwareManager::probeThermal() {
    state.thermal_supported = (access("/sys/class/thermal/thermal_zone0/mode", W_OK) == 0);
    if (state.thermal_supported) {
        state.thermal_default_mode = readSysfs("/sys/class/thermal/thermal_zone0/mode");
    }
    return state.thermal_supported;
}

void HardwareManager::applyMediatekGpu() {
    writeSysfs(state.gpu_node, "1");
}

void HardwareManager::applySnapdragonGpu() {
    writeSysfs(state.gpu_node + "/governor", "performance");
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_hardware_max));
}

void HardwareManager::applyUnisocGpu() {
    writeSysfs(state.gpu_node + "/governor", "performance");
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_hardware_max));
}

void HardwareManager::applyKirinGpu() {
    writeSysfs(state.gpu_node + "/governor", "performance");
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_hardware_max));
}

void HardwareManager::restoreMediatekGpu() {
    writeSysfs(state.gpu_node, "0");
}

void HardwareManager::restoreSnapdragonGpu() {
    writeSysfs(state.gpu_node + "/governor", state.gpu_default_gov);
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_default_min));
}

void HardwareManager::restoreUnisocGpu() {
    writeSysfs(state.gpu_node + "/governor", state.gpu_default_gov);
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_default_min));
}

void HardwareManager::restoreKirinGpu() {
    writeSysfs(state.gpu_node + "/governor", state.gpu_default_gov);
    writeSysfs(state.gpu_node + "/min_freq", std::to_string(state.gpu_default_min));
}

void HardwareManager::applyPerformance() {
    for (const auto& policy : state.cpu_policies) {
        if (policy.has_performance_gov) {
            writeSysfs(policy.path + "/scaling_governor", "performance");
        }
        writeSysfs(policy.path + "/scaling_min_freq", std::to_string(policy.hardware_max));
    }

    switch (state.platform) {
        case PlatformType::Mediatek:
            applyMediatekGpu();
            break;
        case PlatformType::Snapdragon:
            applySnapdragonGpu();
            break;
        case PlatformType::Unisoc:
            applyUnisocGpu();
            break;
        case PlatformType::Kirin:
            applyKirinGpu();
            break;
        case PlatformType::Unknown:
            break;
    }

    if (state.thermal_supported) {
        writeSysfs("/sys/class/thermal/thermal_zone0/mode", "disabled");
    }
}

void HardwareManager::restoreBaseline() {
    for (const auto& policy : state.cpu_policies) {
        writeSysfs(policy.path + "/scaling_governor", policy.default_gov);
        writeSysfs(policy.path + "/scaling_min_freq", std::to_string(policy.default_min));
    }

    switch (state.platform) {
        case PlatformType::Mediatek:
            restoreMediatekGpu();
            break;
        case PlatformType::Snapdragon:
            restoreSnapdragonGpu();
            break;
        case PlatformType::Unisoc:
            restoreUnisocGpu();
            break;
        case PlatformType::Kirin:
            restoreKirinGpu();
            break;
        case PlatformType::Unknown:
            break;
    }

    if (state.thermal_supported) {
        writeSysfs("/sys/class/thermal/thermal_zone0/mode", state.thermal_default_mode);
    }
}
