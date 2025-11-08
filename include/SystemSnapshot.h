#ifndef SYSTEMSNAPSHOT_H
#define SYSTEMSNAPSHOT_H

#include "CpuStats.h"
#include "MemoryInfo.h"
#include <string>

struct SystemSnapshot {
    CpuStats cpu;
    MemoryInfo mem;

    bool capture();
    std::string toString() const;
};

#endif
