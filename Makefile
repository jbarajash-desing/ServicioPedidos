# ==== CONFIGURATION ====
CXX := g++
# flags básicos
CXXFLAGS := -Wall -std=c++17

# Try to get SFML flags from pkg-config (portable). If pkg-config or SFML not available,
# PKG_CFLAGS / PKG_LIBS will be empty and we fall back to LDFLAGS below.
PKG_CFLAGS := $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system 2>/dev/null)
PKG_LIBS := $(shell pkg-config --libs sfml-graphics sfml-window sfml-system 2>/dev/null)

# If SFML libraries are installed in a custom directory, you can pass SFML_LIB_DIR=/path/to/lib
SFML_LIB_DIR ?=

# LDFLAGS: prefer pkg-config results; if not present, use SFML_LIB_DIR (if given) or fall back to plain -lsfml-*
LDFLAGS ?= $(if $(PKG_LIBS),$(PKG_LIBS),$(if $(SFML_LIB_DIR),-L$(SFML_LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-system,-lsfml-graphics -lsfml-window -lsfml-system))

# ==== PATHS ====
SRC_DIR := src
BUILD_DIR := build
EXEC := $(BUILD_DIR)/servicio

# ==== SOURCE FILES ====
SRC := $(filter-out $(SRC_DIR)/gui/gui.cpp, $(shell find $(SRC_DIR) lib -name "*.cpp" 2>/dev/null))
OBJ := $(SRC:%.cpp=$(BUILD_DIR)/%.o)
INCLUDE := -Iinclude -Ilib/factory/include -Ilib/calendar/include -Ilib/products/include

# ==== RULES ====
all: $(EXEC)

$(EXEC): $(OBJ)
	@echo "🔗 Linking..."
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)
	@echo "✅ Build successful!"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "🧱 Compiling $< ..."
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) $(INCLUDE) -c $< -o $@

run: all
	@echo "🚀 Running program..."
	@$(EXEC)

clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf $(BUILD_DIR)/*

.PHONY: all clean run
