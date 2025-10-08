# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -Iinclude

# Carpetas
SRC_DIR = src
OBJ_DIR = build

# Ejecutable
EXEC = Pong

# Buscar todos los archivos .cpp en src/
SRC = $(wildcard $(SRC_DIR)/*.cpp)

# Convertir los .cpp en .o en la carpeta build
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Regla principal
all: $(EXEC)

# Enlazar los objetos para crear el ejecutable
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar cada archivo .cpp en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear la carpeta build si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpiar archivos compilados
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean
