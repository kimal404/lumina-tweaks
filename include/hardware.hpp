#pragma once
#include <string>
#include <vector>

enum class PlatformType {
    Unknown,
    Mediatek,
    Snapdragon,
    Unisoc,
    Kirin
};

struct CpuPolicy {
    std::string path;
    int policy_id;
    std::string default_gov;
    uint32_t default_min;
    uint32_t default_max;
    uint32_t hardware_max;
    bool has_performance_gov;
};

struct HardwareState {
    PlatformType platform = PlatformType::Unknown;
    std::vector<CpuPolicy> cpu_policies;
    std::string gpu_node;
    std::string gpu_default_gov;
    uint64_t gpu_default_min;
    uint64_t gpu_hardware_max;
    bool thermal_supported;
    std::string thermal_default_mode;
};

class HardwareManager {
public:
    static HardwareManager& getInstance();
    bool init();
    void applyPerformance();
    void restoreBaseline();
    PlatformType getPlatform() const;

private:
    HardwareManager() = default;
    HardwareState state;

    bool probeCpu();
    bool probeThermal();
    bool probeMediatek();
    bool probeSnapdragon();
    bool probeUnisoc();
    bool probeKirin();
    void probePlatformGpu();

    void applyMediatekGpu();
    void applySnapdragonGpu();
    void applyUnisocGpu();
    void applyKirinGpu();

    void restoreMediatekGpu();
    void restoreSnapdragonGpu();
    void restoreUnisocGpu();
    void restoreKirinGpu();

    static bool writeSysfs(const std::string& path, const std::string& value);
    static std::string readSysfs(const std::string& path);
};
