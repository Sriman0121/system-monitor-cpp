#include "MemoryInfo.h"
#include <fstream>
#include <string>
#include <sstream>

bool MemoryInfo::readFromProc() {
    std::ifstream f("/proc/meminfo");
    if (!f.is_open()) return false;
    memTotalKB = 0;
    memAvailableKB = 0;
    std::string key, unit;
    uint64_t value;
    while (f >> key >> value >> unit) {
        if (key == "MemTotal:") memTotalKB = value;
        else if (key == "MemAvailable:") memAvailableKB = value;
        // continue reading
    }
    return (memTotalKB > 0);
}

uint64_t MemoryInfo::memUsedKB() const {
    if (memTotalKB >= memAvailableKB) return memTotalKB - memAvailableKB;
    return 0;
}

double MemoryInfo::usedPercent() const {
    if (memTotalKB == 0) return 0.0;
    return 100.0 * double(memUsedKB()) / double(memTotalKB);
}

std::string MemoryInfo::toString() const {
    std::ostringstream oss;
    oss << "Memory: total " << memTotalKB << " KB, available " << memAvailableKB
        << " KB, used " << memUsedKB() << " KB (" << usedPercent() << "%)";
    return oss.str();
}
