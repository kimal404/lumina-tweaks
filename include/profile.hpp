#pragma once

namespace Profile {
    void init();
    void apply_boot_tweaks();
    void apply_performance(bool is_lite, bool dis_thermal);
    void restore_balanced();
    void apply_powersave();
    bool is_in_performance();
}