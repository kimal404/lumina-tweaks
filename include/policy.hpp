#pragma once
#include <string>
#include <vector>

namespace Policy {

struct CpuNode {
    std::string path;
    std::string stock_gov;
    std::string stock_min;
    std::string stock_max;
    long hw_min = 0;
    long hw_max = 0;
    std::vector<long> freqs;
};

void init();
void update(float temp_c, float cpu_load, bool is_lite);
void restore_stock();

}
