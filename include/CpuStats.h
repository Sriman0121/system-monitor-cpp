#ifndef CPUSTATS_H
#define CPUSTATS_H

#include <cstdint>
#include <vector>
#include <string>

struct CpuStats {
    std::vector<uint64_t> ticks;
    uint64_t total = 0;
    uint64_t idle = 0;

    bool readFromProc();
    std::string toString() const;
};

#endif
