#include "Dashboard.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

// ANSI color codes
static const char* CLEAR = "\033[2J\033[H"; // clear screen + move home
static const char* BOLD = "\033[1m";
static const char* RESET = "\033[0m";
static const char* CYAN = "\033[36m";

// draw a simple horizontal bar of width 'w' for percentage [0..100]
static std::string percentBar(double percent, int w = 30) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    int filled = int(std::round((percent / 100.0) * w));
    std::string s;
    s.reserve(w);
    for (int i = 0; i < filled; ++i) s.push_back('#');
    for (int i = filled; i < w; ++i) s.push_back('-');
    return s;
}

// choose color for a percentage (green/yellow/red)
static const char* colorForPercent(double p) {
    if (p >= 70.0) return RED;
    if (p >= 30.0) return YELLOW;
    return GREEN;
}

void Dashboard::render(int iteration,
                       double totalCpu,
                       const std::vector<double>& perCore,
                       const MemoryInfo& mem,
                       const std::vector<PStat>& topCpu,
                       const std::vector<PStat>& topMem) {
    std::cout << CLEAR;

    // Header
    std::cout << BOLD << CYAN << "╔════════════════ System Monitor — Dashboard ════════════════╗" << RESET << "\n";
    std::cout << BOLD << "Iteration " << iteration << RESET << "\n\n";

    // CPU summary with bar
    const int BAR_W = 36;
    std::string bar = percentBar(totalCpu, BAR_W);
    const char* barColor = colorForPercent(totalCpu);
    std::cout << "CPU Total: "
              << std::fixed << std::setprecision(2) << totalCpu << "% "
              << barColor << "[" << bar << "]" << RESET << "\n";

    // Per-core small list on next line (print up to 16 cores nicely)
    std::cout << "Per-core: ";
    for (size_t i = 1; i < perCore.size(); ++i) {
        double p = perCore[i];
        const char* c = colorForPercent(p);
        std::cout << c << "c" << (i-1) << ":" << std::fixed << std::setprecision(1) << p << "%" << RESET;
        if (i + 1 < perCore.size()) std::cout << "  ";
        if ((i % 8) == 0) std::cout << "\n          ";
    }
    std::cout << "\n\n";

    // Memory summary, color if high usage
    double memPct = mem.usedPercent();
    const char* memColor = colorForPercent(memPct);
    std::cout << "Memory: "
              << mem.memUsedKB() << " KB used / " << mem.memTotalKB << " KB ("
              << memColor << std::fixed << std::setprecision(2) << memPct << "%" << RESET << ")\n\n";

    // Top by CPU
    std::cout << BOLD << "Top by CPU" << RESET << "\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(8) << "CPU%" << std::setw(12) << "RSS(KB)" << "CMD\n";
    int shown = 0;
    for (const auto &p : topCpu) {
        if (shown++ >= 10) break;
        // color processes that use significant CPU
        const char* col = (p.cpuPercent >= 20.0) ? RED : (p.cpuPercent >= 5.0 ? YELLOW : RESET);
        std::cout << col
                  << std::left << std::setw(8) << p.pid
                  << std::setw(8) << std::fixed << std::setprecision(2) << p.cpuPercent
                  << std::setw(12) << p.rssKB
                  << p.cmd
                  << RESET << "\n";
    }
    std::cout << "\n";

    // Top by Memory
    std::cout << BOLD << "Top by Memory" << RESET << "\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(12) << "RSS(KB)" << std::setw(8) << "CPU%" << "CMD\n";
    shown = 0;
    for (const auto &p : topMem) {
        if (shown++ >= 10) break;
        // highlight processes using >100MB (~102400 KB)
        const char* col = (p.rssKB >= 102400) ? YELLOW : RESET;
        std::cout << col
                  << std::left << std::setw(8) << p.pid
                  << std::setw(12) << p.rssKB
                  << std::setw(8) << std::fixed << std::setprecision(2) << p.cpuPercent
                  << p.cmd
                  << RESET << "\n";
    }

    std::cout << "\n" << CYAN << "╚══════════════════════════════════════════════════════════╝" << RESET << "\n";
    std::cout << "(press Ctrl+C to quit)\n";
    std::cout.flush();
}
