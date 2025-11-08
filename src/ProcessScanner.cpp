#include "ProcessScanner.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cerrno>

static bool isNumber(const std::string &s) {
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return !s.empty();
}

static uint64_t try_stoull(const std::string &s) {
    try { return std::stoull(s); } catch (...) { return 0; }
}
static long try_stol(const std::string &s) {
    try { return std::stol(s); } catch (...) { return 0; }
}

bool ProcessScanner::scanAll(std::vector<ProcessSnapshot>& out) {
    out.clear();
    DIR *d = opendir("/proc");
    if (!d) return false;
    struct dirent *entry;
    long pageSizeKB = sysconf(_SC_PAGESIZE) / 1024; // convert pages to KB (fallback not used for VmRSS)
    while ((entry = readdir(d)) != nullptr) {
        std::string name(entry->d_name);
        if (!isNumber(name)) continue;
        int pid = 0;
        try { pid = std::stoi(name); } catch (...) { continue; }
        ProcessSnapshot ps;
        ps.pid = pid;

        // read /proc/<pid>/stat for utime (14) and stime (15)
        std::string statPath = "/proc/" + name + "/stat";
        std::ifstream fstat(statPath);
        if (fstat) {
            std::string content;
            std::getline(fstat, content);
            std::istringstream iss(content);
            int parsedPid;
            std::string comm;
            char ch;
            if (iss >> parsedPid) {
                // read comm (inside parentheses)
                iss >> ch; // '('
                std::getline(iss, comm, ')'); // read until ')'
                std::string rest;
                std::getline(iss, rest);
                std::istringstream r2(rest);
                std::vector<std::string> fields;
                std::string tok;
                while (r2 >> tok) fields.push_back(tok);

                // compute jiffies safely
                if (fields.size() >= 15) { // ensure utime/stime exist
                    uint64_t utime = try_stoull(fields.size() > 13 ? fields[13] : "0");
                    uint64_t stime = try_stoull(fields.size() > 14 ? fields[14] : "0");
                    ps.totalJiffies = utime + stime;
                } else {
                    ps.totalJiffies = 0;
                }
            } else {
                ps.totalJiffies = 0;
            }
        } else {
            ps.totalJiffies = 0;
        }

        // Prefer reading VmRSS (in KB) from /proc/<pid>/status which is portable
        ps.rssKB = 0;
        std::string statusForRss = "/proc/" + name + "/status";
        std::ifstream frss(statusForRss);
        if (frss) {
            std::string line;
            while (std::getline(frss, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {
                    std::istringstream issr(line);
                    std::string key;
                    uint64_t val = 0;
                    std::string unit;
                    if (issr >> key >> val >> unit) {
                        ps.rssKB = val; // VmRSS already in KB
                    }
                    break;
                }
            }
        }

        // read cmdline (args separated by '\0')
        std::string cmdPath = "/proc/" + name + "/cmdline";
        std::ifstream fcmd(cmdPath);
        if (fcmd) {
            std::string tmp;
            std::getline(fcmd, tmp, '\0');
            if (!tmp.empty()) {
                std::string outcmd = tmp;
                std::replace(outcmd.begin(), outcmd.end(), '\0', ' ');
                ps.cmdline = outcmd;
            } else {
                std::ifstream fcomm("/proc/" + name + "/comm");
                if (fcomm) {
                    std::string c;
                    std::getline(fcomm, c);
                    ps.cmdline = c;
                }
            }
        } else {
            std::ifstream fcomm("/proc/" + name + "/comm");
            if (fcomm) {
                std::string c;
                std::getline(fcomm, c);
                ps.cmdline = c;
            }
        }

        // read uid from /proc/<pid>/status
        std::string statusPath = "/proc/" + name + "/status";
        std::ifstream fstatus(statusPath);
        if (fstatus) {
            std::string line;
            while (std::getline(fstatus, line)) {
                if (line.rfind("Uid:", 0) == 0) {
                    std::istringstream iss(line);
                    std::string key;
                    int uid;
                    if (iss >> key >> uid) {
                        ps.uid = uid;
                    }
                    break;
                }
            }
        }

        out.push_back(ps);
    }
    closedir(d);
    return true;
}

std::unordered_map<int, ProcessSnapshot> ProcessScanner::toMap(const std::vector<ProcessSnapshot>& v) {
    std::unordered_map<int, ProcessSnapshot> m;
    for (const auto &p : v) m[(int)p.pid] = p;
    return m;
}
