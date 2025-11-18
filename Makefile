# ==========================================
#   MAKEFILE PARA UNCOMPRESSOR
# ==========================================

CXX      := g++
CXXFLAGS := -Wall -std=c++17 -g
INCLUDES := -Ilib/huffman/include \
            -Ilib/lector/include \
            -Ilib/dictionary/include

BUILD_DIR := build
EXEC      := uncompressor

MAIN_SRC := src/main.cpp

# Busca .cpp en lib/ pero IGNORA tests, build y cmake
LIB_SRCS := $(shell find lib -name "*.cpp" -not -path "*/tests/*" -not -path "*/build/*" -not -path "*/CMakeFiles/*")

ALL_SRCS := $(MAIN_SRC) $(LIB_SRCS)
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(ALL_SRCS)))

all: $(BUILD_DIR)/$(EXEC)

$(BUILD_DIR)/$(EXEC): $(OBJS)
	@echo "🔗 Enlazando ejecutable..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/main.o: src/main.cpp
	@echo "🧱 Compilando main.cpp..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o:
	@echo "🧱 Compilando librería: $(notdir $(@:.o=.cpp))..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(shell find lib -name $(notdir $(@:.o=.cpp))) -o $@

run: all
	@echo "🚀 Ejecutando UnCompressor..."
	@./$(BUILD_DIR)/$(EXEC)

clean:
	@echo "🧹 Limpiando..."
	@rm -rf $(BUILD_DIR)

.PHONY: all clean run
