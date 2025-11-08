#include "CpuStats.h"
#include <fstream>
#include <sstream>

bool CpuStats::readFromProc() {
    std::ifstream f("/proc/stat");
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("cpu ", 0) == 0) {
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel;
            ticks.clear();
            uint64_t v;
            while (iss >> v) ticks.push_back(v);
            total = 0;
            for (auto x : ticks) total += x;
            // idle is ticks[3] (idle) + ticks[4] (iowait) if present
            idle = 0;
            if (ticks.size() > 3) {
                idle = ticks[3];
                if (ticks.size() > 4) idle += ticks[4];
            }
            return true;
        }
    }
    return false;
}

#include <sstream>
std::string CpuStats::toString() const {
    std::ostringstream oss;
    oss << "CPU total ticks: " << total << " idle ticks: " << idle;
    return oss.str();
}
