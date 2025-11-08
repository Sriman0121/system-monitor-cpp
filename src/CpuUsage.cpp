#include "CpuUsage.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

static bool parseCpuLine(const std::string &line, RawCpu &out) {
    std::istringstream iss(line);
    std::string label;
    if (!(iss >> label)) return false;
    if (label.size() < 3 || label.substr(0,3) != "cpu") return false;
    out.ticks.clear();
    uint64_t v;
    while (iss >> v) out.ticks.push_back(v);
    out.total = 0;
    for (auto x : out.ticks) out.total += x;
    out.idle = 0;
    if (out.ticks.size() > 3) {
        out.idle = out.ticks[3];
        if (out.ticks.size() > 4) out.idle += out.ticks[4];
    }
    return true;
}

bool CpuUsage::readAll(std::vector<RawCpu> &out) {
    std::ifstream f("/proc/stat");
    if (!f.is_open()) return false;
    out.clear();
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("cpu", 0) == 0) {
            RawCpu r;
            if (parseCpuLine(line, r)) out.push_back(r);
        } else {
            break;
        }
    }
    return !out.empty();
}

std::vector<double> CpuUsage::computePercent(const std::vector<RawCpu>& a, const std::vector<RawCpu>& b) {
    std::vector<double> res;
    size_t n = std::min(a.size(), b.size());
    res.resize(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        uint64_t totalDelta = 0;
        if (b[i].total >= a[i].total) totalDelta = b[i].total - a[i].total;
        uint64_t idleDelta = 0;
        if (b[i].idle >= a[i].idle) idleDelta = b[i].idle - a[i].idle;
        double percent = 0.0;
        if (totalDelta > 0) {
            percent = 100.0 * double(totalDelta - idleDelta) / double(totalDelta);
            if (percent < 0.0) percent = 0.0;
            if (percent > 100.0) percent = 100.0;
        }
        res[i] = percent;
    }
    return res;
}

std::string CpuUsage::formatPercentages(const std::vector<double>& p) {
    std::ostringstream oss;
    if (p.empty()) return "";
    oss.setf(std::ios::fixed);
    oss.precision(2);
    oss << "Total CPU: " << p[0] << "%\n";
    for (size_t i = 1; i < p.size(); ++i) {
        oss << "cpu" << (i - 1) << ": " << p[i] << "%";
        if (i < p.size() - 1) oss << "  ";
    }
    oss << "\n";
    return oss.str();
}
