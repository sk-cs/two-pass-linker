CXX=g++
CXXFLAGS=-g -Wall -Wextra -O0
TARGET=linker
SOURCE=linker.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
	
clean:
	rm -f $(TARGET)
	rm -rf *.dSYM

.PHONY: all gdb clean