CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

SRC_FILES = $(SRC_DIR)/client.cpp
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/client.exe

LIBS = -lws2_32

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)

rebuild: clean all
