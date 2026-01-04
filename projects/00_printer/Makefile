# --- Makefile: Printer (Project 00) ---

# Configuración del Compilador
CXX      := g++
CXXFLAGS := -std=c++23 -O3 -march=native -Wall -Wextra

# Rutas Relativas
INCLUDES := -I../../common/include
BIN_DIR  := ../../bin

# Archivos
TARGET_NAME := printer
TARGET      := $(BIN_DIR)/$(TARGET_NAME)

SRCS        := $(wildcard src/*.cpp)

# Reglas
all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	@echo "[Project] Compilando: $(TARGET_NAME)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f $(TARGET)