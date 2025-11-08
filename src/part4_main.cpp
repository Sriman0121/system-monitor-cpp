#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <map>
#include "ProcessScanner.h"
#include "CpuUsage.h"

struct ProcessStatDelta {
    int pid;
    std::string cmdline;
    double cpuPercent;
    uint64_t rssKB;
};

int main(int argc, char** argv) {
    int iterations = 10;
    int interval = 2;
    int topN = 5;


    if (argc > 1) iterations = std::stoi(argv[1]);
    if (argc > 2) interval = std::stoi(argv[2]);
    if (argc > 3) topN = std::stoi(argv[3]);

    std::vector<RawCpu> sysA, sysB;
    if (!CpuUsage::readAll(sysA)) {
        std::cerr << "Failed to read /proc/stat (sysA)\n";
        return 1;
    }

    std::vector<ProcessSnapshot> procsA;
    if (!ProcessScanner::scanAll(procsA)) {
        std::cerr << "Failed to scan processes (procsA)\n";
        return 1;
    }


    auto mapA = ProcessScanner::toMap(procsA);

    for (int it = 0; it < iterations; ++it) {
        std::this_thread::sleep_for(std::chrono::seconds(interval));

        if (!CpuUsage::readAll(sysB)) {
            std::cerr << "Failed to read /proc/stat (sysB)\n";
            return 1;
        }

        std::vector<ProcessSnapshot> procsB;
        if (!ProcessScanner::scanAll(procsB)) {
            std::cerr << "Failed to scan processes (procsB)\n";
            return 1;
        }


        auto mapB = ProcessScanner::toMap(procsB);

        uint64_t totalA = sysA[0].total;
        uint64_t totalB = sysB[0].total;
        uint64_t totalDelta = (totalB >= totalA) ? (totalB - totalA) : 0;

        std::vector<ProcessStatDelta> deltas;
        for (const auto &p2 : procsB) {
            int pid = p2.pid;
            auto itA = mapA.find(pid);
            uint64_t jiffA = 0;
            if (itA != mapA.end()) jiffA = itA->second.totalJiffies;
            uint64_t jiffB = p2.totalJiffies;


            uint64_t procDelta = (jiffB >= jiffA) ? (jiffB - jiffA) : 0;
            double cpuPerc = 0.0;
            if (totalDelta > 0) cpuPerc = 100.0 * double(procDelta) / double(totalDelta);

            ProcessStatDelta psd;
            psd.pid = pid;
            psd.cmdline = p2.cmdline.empty()? std::to_string(pid) : p2.cmdline;
            psd.cpuPercent = cpuPerc;
            psd.rssKB = p2.rssKB;
            deltas.push_back(psd);
        }


        std::sort(deltas.begin(), deltas.end(), [](const ProcessStatDelta &a, const ProcessStatDelta &b){
            return a.cpuPercent > b.cpuPercent;
        });

        std::cout << "=== Iteration " << (it+1) << " (top " << topN << ") ===\n";
        std::cout << "Top by CPU:\n";
        int shown = 0;
        for (const auto &d : deltas) {
            std::cout.setf(std::ios::fixed); std::cout.precision(2);
            std::cout << "PID " << d.pid << " CPU " << d.cpuPercent << "% RSS " << d.rssKB << " KB CMD: " << d.cmdline << "\n";
            if (++shown >= topN) break;
        }


        std::sort(deltas.begin(), deltas.end(), [](const ProcessStatDelta &a, const ProcessStatDelta &b){
            return a.rssKB > b.rssKB;
        });

        std::cout << "\nTop by Memory:\n";
        shown = 0;
        for (const auto &d : deltas) {
            std::cout << "PID " << d.pid << " RSS " << d.rssKB << " KB CPU " << d.cpuPercent << "% CMD: " << d.cmdline << "\n";
            if (++shown >= topN) break;
        }

        std::cout << "----------------------------------------\n";
        sysA = sysB;
        mapA = mapB;
    }

    return 0;
}
