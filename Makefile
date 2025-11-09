# ==== CONFIGURATION ====
CXX := g++
CXXFLAGS := -Wall -std=c++17
INCLUDE := -Iinclude -Ilib/lector/include

# ==== BUILD MODE ====
# Puedes compilar con:
#   make MODE=gui     -> usa wxWidgets
#   make MODE=terminal -> usa SFML (por defecto)
MODE ?= terminal

# ==== PATHS ====
SRC_DIR := src
BUILD_DIR := build
EXEC := $(BUILD_DIR)/servicio

# ==== SOURCE FILES ====
SRC := $(shell find $(SRC_DIR) lib -name "*.cpp" 2>/dev/null)
OBJ := $(SRC:%.cpp=$(BUILD_DIR)/%.o)

# ==== MODE: wxWidgets ====
ifeq ($(MODE),gui)
    GUI_FLAGS := `wx-config --cxxflags`
    GUI_LIBS := `wx-config --libs`
    CXXFLAGS += -DUSE_GUI $(GUI_FLAGS)
    # 🔧 Añadimos también SFML al modo GUI
    LDFLAGS := $(GUI_LIBS) -lsfml-graphics -lsfml-window -lsfml-system
    MSG := "🪟 Building GUI version (wxWidgets + SFML)"
else

# ==== MODE: terminal (default) ====
    # Try to get SFML flags from pkg-config
    PKG_CFLAGS := $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system 2>/dev/null)
    PKG_LIBS := $(shell pkg-config --libs sfml-graphics sfml-window sfml-system 2>/dev/null)
    SFML_LIB_DIR ?=
    LDFLAGS ?= $(if $(PKG_LIBS),$(PKG_LIBS),$(if $(SFML_LIB_DIR),-L$(SFML_LIB_DIR) -lsfml-graphics -lsfml-window -lsfml-system,-lsfml-graphics -lsfml-window -lsfml-system))
    CXXFLAGS += $(PKG_CFLAGS)
    MSG := "🖥️ Building terminal version (SFML)"
endif

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
