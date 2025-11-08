
CXX = g++

CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude

SRC = $(wildcard src/*.cpp)

OBJ = $(SRC:.cpp=.o)

TARGET = bin/system-monitor



all: $(TARGET)



$(TARGET): $(SRC)

	@mkdir -p bin

	$(CXX) $(CXXFLAGS) $^ -o $@



clean:

	rm -rf bin $(OBJ)



.PHONY: all clean

