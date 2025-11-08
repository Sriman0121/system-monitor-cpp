#include <iostream>
#include <thread>
#include <chrono>
#include "CpuUsage.h"

int main(int argc, char** argv) {
    int iterations = 5;
    if (argc > 1) iterations = std::stoi(argv[1]);

    std::vector<RawCpu> a, b;
    if (!CpuUsage::readAll(a)) {
        std::cerr << "Failed to read /proc/stat at T1\n";
        return 1;
    }

    for (int it = 0; it < iterations; ++it) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!CpuUsage::readAll(b)) {
            std::cerr << "Failed to read /proc/stat at T2\n";
            return 1;
        }
        auto perc = CpuUsage::computePercent(a, b);
        std::cout << "=== CPU usage (sample " << (it+1) << ") ===\n";
        std::cout << CpuUsage::formatPercentages(perc) << "\n";
        a = b;
    }
    return 0;
}
