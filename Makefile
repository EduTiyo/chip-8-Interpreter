CXX = g++
CXXFLAGS = -Wall -std=c++17
SRC = src/main.cpp src/c8vm.cpp src/display.cpp
TARGET = chip8

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
