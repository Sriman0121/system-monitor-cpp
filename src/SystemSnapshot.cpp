#include "SystemSnapshot.h"

bool SystemSnapshot::capture() {
    bool ok1 = cpu.readFromProc();
    bool ok2 = mem.readFromProc();
    return ok1 && ok2;
}

#include <sstream>
std::string SystemSnapshot::toString() const {
    std::ostringstream oss;
    oss << cpu.toString() << "\n" << mem.toString();
    return oss.str();
}
