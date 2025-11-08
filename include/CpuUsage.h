#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <vector>
#include <cstdint>
#include <string>

// Holds raw ticks for one CPU line
struct RawCpu {
    std::vector<uint64_t> ticks;
    uint64_t total = 0;
    uint64_t idle = 0;
};

// CpuUsage: read /proc/stat and compute percentages
class CpuUsage {
public:
    // reads all cpu lines (aggregate + per-core) into raws
    static bool readAll(std::vector<RawCpu> &out);

    // compute usage percentages between two snapshots (same length)
    // returns vector of percentages (first element = aggregate "cpu")
    static std::vector<double> computePercent(const std::vector<RawCpu>& a, const std::vector<RawCpu>& b);

    // helper to format percentages into a readable string
    static std::string formatPercentages(const std::vector<double>& p);
};

#endif
