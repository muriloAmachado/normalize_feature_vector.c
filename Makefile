# Definição do compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -Wall -g

# Nome do executável
TARGET = main.out

# Lista de arquivos fonte
SRCS = main.c

HEADERS = 

# Regra padrão para compilar o executável
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Limpa os arquivos compilados
clean:
	rm -f $(TARGET)

# Indica que clean não é um arquivo
.PHONY: clean