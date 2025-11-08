#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <string>
#include <cstdint>

struct ProcessInfo {
    int pid = 0;
    int uid = 0;
    std::string cmdline;
    uint64_t rssKB = 0;

    std::string toString() const;
};

#endif
