# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++20 -O2

# Directories
SRC_DIR = src
BUILD_DIR = build

# Files
SRC = $(SRC_DIR)/main.cpp
OBJ = $(BUILD_DIR)/main.o
TARGET = hex

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -lncurses

# Compile source files into object files (corrected with -c)
$(OBJ): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# make docs
doc:
	doxygen
# Declare phony targets
.PHONY: all clean
