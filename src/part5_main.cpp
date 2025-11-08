#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <map>
#include "ProcessScanner.h"
#include "CpuUsage.h"
#include "Dashboard.h"

int main(int argc, char** argv) {
    int iterations = 10;
    int interval = 1;
    int topN = 6;

    if (argc > 1) iterations = std::stoi(argv[1]);
    if (argc > 2) interval = std::stoi(argv[2]);
    if (argc > 3) topN = std::stoi(argv[3]);

    std::vector<RawCpu> a_cpu, b_cpu;
    std::vector<ProcessSnapshot> a_ps, b_ps;

    // First capture
    CpuUsage::readAll(a_cpu);
    ProcessScanner::scanAll(a_ps);

    for (int iter = 1; iter <= iterations; ++iter) {
        std::this_thread::sleep_for(std::chrono::seconds(interval));

        // Second capture
        CpuUsage::readAll(b_cpu);
        ProcessScanner::scanAll(b_ps);

        // CPU usage
        double totalCpu = 0.0;
        auto perCore = CpuUsage::computePercent(a_cpu, b_cpu);
        if (!perCore.empty()) totalCpu = perCore[0];

        // Build process maps
        auto mapA = ProcessScanner::toMap(a_ps);
        auto mapB = ProcessScanner::toMap(b_ps);

        // System total CPU jiffies delta
        uint64_t sysTotalA = 0, sysTotalB = 0;
        for (auto &r : a_cpu) sysTotalA += r.total;
        for (auto &r : b_cpu) sysTotalB += r.total;
        uint64_t sysDelta = (sysTotalB > sysTotalA) ? (sysTotalB - sysTotalA) : 0;

        // Build process stats for dashboard
        std::vector<PStat> stats;
        stats.reserve(mapB.size());

        for (auto &pair : mapB) {
            int pid = pair.first;
            auto &cur = pair.second;
            auto it = mapA.find(pid);
            if (it == mapA.end()) continue; // process not in previous snapshot

            uint64_t jA = it->second.totalJiffies;
            uint64_t jB = cur.totalJiffies;
            uint64_t dJ = (jB > jA) ? (jB - jA) : 0;

            double pct = 0.0;
            if (sysDelta > 0) {
                pct = 100.0 * double(dJ) / double(sysDelta);
                if (pct < 0) pct = 0;
                if (pct > 100) pct = 100;
            }

            stats.push_back({ pid, cur.cmdline, pct, cur.rssKB });
        }

        // Prepare CPU and MEM sorted lists
        auto byCpu = stats;
        auto byMem = stats;

        std::sort(byCpu.begin(), byCpu.end(), [](auto &a, auto &b) {
            return a.cpuPercent > b.cpuPercent;
        });

        std::sort(byMem.begin(), byMem.end(), [](auto &a, auto &b) {
            return a.rssKB > b.rssKB;
        });

        if ((int)byCpu.size() > topN) byCpu.resize(topN);
        if ((int)byMem.size() > topN) byMem.resize(topN);

        // Read memory
        MemoryInfo mem;
        mem.readFromProc();

        // Render Dashboard UI
        Dashboard::render(iter, totalCpu, perCore, mem, byCpu, byMem);

        // Swap snapshots
        a_cpu = b_cpu;
        a_ps = b_ps;
    }

    return 0;
}
