#ifndef MEMORYINFO_H
#define MEMORYINFO_H

#include <cstdint>
#include <string>

struct MemoryInfo {
    uint64_t memTotalKB = 0;
    uint64_t memAvailableKB = 0;

    uint64_t memUsedKB() const;
    double usedPercent() const;
    bool readFromProc();
    std::string toString() const;
};

#endif
