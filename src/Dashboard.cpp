#include "Dashboard.h"
#include <iostream>
#include <iomanip>

static const char* CLEAR = "\033[2J\033[H"; // clear screen + move home
static const char* BOLD = "\033[1m";
static const char* RESET = "\033[0m";
static const char* RED = "\033[31m";
static const char* GREEN = "\033[32m";
static const char* YELLOW = "\033[33m";

void Dashboard::render(int iteration,
                       double totalCpu,
                       const std::vector<double>& perCore,
                       const MemoryInfo& mem,
                       const std::vector<PStat>& topCpu,
                       const std::vector<PStat>& topMem) {
    std::cout << CLEAR;
    std::cout << BOLD << "System Monitor — Dashboard (iter " << iteration << ")" << RESET << "\n\n";

    // CPU summary
    std::cout << "CPU Total: ";
    std::cout << std::fixed << std::setprecision(2) << totalCpu << "%";
    std::cout << "   | Per-core: ";
    for (size_t i = 1; i < perCore.size() && i <= 8; ++i) {
        std::cout << "c" << (i-1) << ":" << std::fixed << std::setprecision(1) << perCore[i] << "% ";
    }
    if (perCore.size() > 9) std::cout << "...";
    std::cout << "\n";

    // Memory summary
    std::cout << "Memory: " << mem.memUsedKB() << " KB used / " << mem.memTotalKB << " KB (";
    std::cout << std::fixed << std::setprecision(2) << mem.usedPercent() << "%)\n\n";

    // Top CPU
    std::cout << BOLD << "Top by CPU" << RESET << "\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(8) << "CPU%" << std::setw(12) << "RSS(KB)" << "CMD\n";
    int shown = 0;
    for (const auto &p : topCpu) {
        if (shown++ >= 10) break;
        std::cout << std::left << std::setw(8) << p.pid << std::setw(8) << std::fixed << std::setprecision(2) << p.cpuPercent
                  << std::setw(12) << p.rssKB << p.cmd << "\n";
    }
    std::cout << "\n";

    // Top Memory
    std::cout << BOLD << "Top by Memory" << RESET << "\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(8) << "RSS(KB)" << std::setw(8) << "CPU%" << "CMD\n";
    shown = 0;
    for (const auto &p : topMem) {
        if (shown++ >= 10) break;
        std::cout << std::left << std::setw(8) << p.pid << std::setw(8) << p.rssKB << std::setw(8)
                  << std::fixed << std::setprecision(2) << p.cpuPercent << p.cmd << "\n";
    }

    std::cout << "\n(press Ctrl+C to quit early)\n";
    std::cout.flush();
}
