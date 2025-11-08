CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude -pthread

# Main placeholder app
SRC_MAIN = src/main.cpp
TARGET = bin/system-monitor

# Part 2
PART2_SRC = src/part2_main.cpp src/CpuStats.cpp src/MemoryInfo.cpp src/SystemSnapshot.cpp src/ProcessInfo.cpp
PART2_TARGET = bin/part2-test

# Part 3
PART3_SRC = src/part3_main.cpp src/CpuUsage.cpp
PART3_TARGET = bin/part3-test

# Part 4
PART4_SRC = src/part4_main.cpp src/ProcessScanner.cpp src/CpuUsage.cpp src/Dashboard.cpp
PART4_TARGET = bin/part4-test

# ✅ Part 5 (Dashboard Mode)
PART5_SRC = src/part5_main.cpp src/ProcessScanner.cpp src/CpuUsage.cpp src/Dashboard.cpp src/MemoryInfo.cpp
PART5_TARGET = bin/part5-dashboard

all: $(TARGET)

$(TARGET): $(SRC_MAIN)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

part2: $(PART2_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $(PART2_TARGET)

part3: $(PART3_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $(PART3_TARGET)

part4: $(PART4_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $(PART4_TARGET)

part5: $(PART5_SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $(PART5_TARGET)

clean:
	@rm -rf bin *.o

.PHONY: all part2 part3 part4 part5 clean
