#include <iostream>
#include "SystemSnapshot.h"

int main() {
    SystemSnapshot snap;
    if (!snap.capture()) {
        std::cerr << "Failed to capture system snapshot. Are you on Linux with /proc available?" << std::endl;
        return 1;
    }
    std::cout << "=== System Snapshot ===" << std::endl;
    std::cout << snap.toString() << std::endl;
    return 0;
}
