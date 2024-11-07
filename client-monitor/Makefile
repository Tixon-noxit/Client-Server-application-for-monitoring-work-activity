CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src
ADD_TO_STARTUP_SRC = src/addToStartup.cpp

SRC_FILES = $(SRC_DIR)/client_chop_centaur.cpp  # Только клиентская программа
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/client_chop_centaur.exe
ADD_TO_STARTUP_TARGET = $(BIN_DIR)/addToStartup.exe

LIBS = -lws2_32

all: create_dirs $(TARGET) $(ADD_TO_STARTUP_TARGET)

create_dirs:
	@echo "Creating directories..."
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

# Компиляция клиентской программы
$(TARGET): $(OBJ_FILES)
	@echo "Linking client program..."
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LIBS)

# Компиляция программы для добавления в автозапуск
$(ADD_TO_STARTUP_TARGET): $(BUILD_DIR)/addToStartup.o
	@echo "Linking addToStartup..."
	$(CXX) $(BUILD_DIR)/addToStartup.o -o $(ADD_TO_STARTUP_TARGET)

# Компиляция исходных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)

rebuild: clean all
