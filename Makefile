# --- Makefile Maestro ---

# 1. Lista de directorios a compilar
SUBDIRS := feeder projects/00_printer

# 2. Configuración general
.PHONY: all clean $(SUBDIRS)

# Regla por defecto: compilar todo
all: $(SUBDIRS)
	@echo "--------------------------------------"
	@echo ">>> Construcción completa."
	@echo ">>> Ejecutables en ./bin/"
	@echo "--------------------------------------"

# Regla para delegar en cada subdirectorio
$(SUBDIRS):
	@echo ">>> Entrando en $@"
	@$(MAKE) -C $@

# Regla de limpieza global
clean:
	@echo ">>> Limpiando binarios y objetos..."
	@rm -rf bin/*
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done