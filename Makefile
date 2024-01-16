CXX=g++
CXXFLAGS=-Wall -std=c++17 -O3
SRC_DIR = src
TARGET=main

HEADERS = $(wildcard $(SRC_DIR)/*.h)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

all: $(TARGET)

$(TARGET): main.cpp $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp $(SOURCES)

clean:
	rm -f $(TARGET)
