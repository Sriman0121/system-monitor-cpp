#include "ProcessInfo.h"
#include <sstream>

std::string ProcessInfo::toString() const {
    std::ostringstream oss;
    oss << "PID: " << pid << " UID: " << uid << " RSS: " << rssKB << " KB CMD: " << cmdline;
    return oss.str();
}
