# ==== CONFIGURATION ====
CXX := g++
CXXFLAGS := -Wall -std=c++17
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system

# ==== PATHS ====
SRC_DIR := src
BUILD_DIR := build
EXEC := $(BUILD_DIR)/servicio

# ==== SOURCE FILES ====
SRC := $(shell find $(SRC_DIR) lib products -name "*.cpp")
OBJ := $(SRC:%.cpp=$(BUILD_DIR)/%.o)
INCLUDE := -Iinclude -Ilib/factory/include -Ilib/calendar/include -Iproducts/include

# ==== RULES ====
all: $(EXEC)

$(EXEC): $(OBJ)
	@echo "🔗 Linking..."
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)
	@echo "✅ Build successful!"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "🧱 Compiling $< ..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

run: all
	@echo "🚀 Running program..."
	@$(EXEC)

clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf $(BUILD_DIR)/*

.PHONY: all clean run
