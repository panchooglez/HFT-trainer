# --- Makefile: Feeder ---

# Configuración del Compilador
CXX      := g++
# Flags HFT: C++23, Optimización Máxima, Instrucciones Nativas CPU
CXXFLAGS := -std=c++23 -O3 -march=native -Wall -Wextra

# Rutas Relativas
INCLUDES := -I../common/include
BIN_DIR  := ../bin

# Archivos
TARGET   := $(BIN_DIR)/market_feeder
SRCS     := $(wildcard src/*.cpp)

# Reglas
all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	@echo "[Feeder] Compilando: $@"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f $(TARGET)