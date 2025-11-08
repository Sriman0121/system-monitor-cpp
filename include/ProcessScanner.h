#ifndef PROCESSSCANNER_H
#define PROCESSSCANNER_H

#include "ProcessInfo.h"
#include "CpuUsage.h"
#include <vector>
#include <unordered_map>

struct ProcessSnapshot {
    int pid = 0;
    std::string cmdline;
    uint64_t rssKB = 0;
    uint64_t totalJiffies = 0; // utime + stime
    int uid = 0;
};

class ProcessScanner {
public:
    // read all processes and return their snapshots
    // returns false on failure
    static bool scanAll(std::vector<ProcessSnapshot>& out);

    // helper to map pid -> snapshot
    static std::unordered_map<int, ProcessSnapshot> toMap(const std::vector<ProcessSnapshot>& v);
};

#endif
