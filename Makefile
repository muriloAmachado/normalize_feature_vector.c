# Definição do compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -Wall -g

# Nome dos executáveis
TARGETS = main.out newton-raphson.out sse-instructions.out lookup-table.out

# Lista de arquivos fonte
SRCS = main.c newton-raphson.c sse-instructions.c lookup-table.c

# Regras para compilar cada arquivo fonte em um executável separado
main.out: main.c
	$(CXX) $(CXXFLAGS) -o main.out main.c

newton-raphson.out: newton-raphson.c
	$(CXX) $(CXXFLAGS) -o newton-raphson.out newton-raphson.c

sse-instructions.out: sse-instructions.c
	$(CXX) $(CXXFLAGS) -o sse-instructions.out sse-instructions.c

lookup-table.out: lookup-table.c
	$(CXX) $(CXXFLAGS) -o lookup-table.out lookup-table.c

# Regra para compilar todos os executáveis
all: $(TARGETS)

# Limpa os arquivos compilados
clean:
	rm -f $(TARGETS)

# Indica que clean e all não são arquivos
.PHONY: clean all