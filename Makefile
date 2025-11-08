CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude

# main app (original)
SRC_MAIN = src/main.cpp
TARGET = bin/system-monitor

# part2 test binary
PART2_SRC = src/part2_main.cpp src/CpuStats.cpp src/MemoryInfo.cpp src/SystemSnapshot.cpp src/ProcessInfo.cpp
PART2_TARGET = bin/part2-test

all: $(TARGET)

$(TARGET): $(SRC_MAIN)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

part2: $(PART2_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $(PART2_TARGET)

clean:
	rm -rf bin *.o

.PHONY: all part2 clean
