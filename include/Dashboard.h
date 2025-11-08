#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <string>
#include <vector>
#include <cstdint>
#include "MemoryInfo.h"

struct PStat {
    int pid;
    std::string cmd;
    double cpuPercent;
    uint64_t rssKB;
};

class Dashboard {
public:
    // Render the dashboard. Clears screen and prints summary + two lists.
    // totalCpu: aggregate cpu percent
    // perCore: per-core percentages (first element is total)
    // mem: MemoryInfo object (memTotalKB, memAvailableKB)
    // topCpu, topMem: vectors of PStat (already sorted)
    // iteration: current iteration number
    static void render(int iteration,
                       double totalCpu,
                       const std::vector<double>& perCore,
                       const MemoryInfo& mem,
                       const std::vector<PStat>& topCpu,
                       const std::vector<PStat>& topMem);
};
#endif
