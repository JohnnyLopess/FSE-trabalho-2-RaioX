# Makefile

# Compiler e flags
# Adicionada a flag -DDEBUG para ativar os prints de depuração que colocamos no código.
# Para desativar os prints, basta remover -DDEBUG desta linha.
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude -DDEBUG

# Diretórios e arquivos
SRC     = src
BIN     = main

SRCS    = \
    $(SRC)/main.c \
    $(SRC)/modbus.c \
    $(SRC)/uart.c \
    $(SRC)/botoes.c \
    $(SRC)/raiox.c \
    $(SRC)/encoder.c \
    $(SRC)/motores.c \
    $(SRC)/dashboard.c \
    $(SRC)/calibracao.c \
	$(SRC)/sensor_BMP280.c \
	$(SRC)/pid.c \
	$(SRC)/posicionamento.c\
	$(SRC)/presets.c \

# Define o alvo padrão como 'help'
.DEFAULT_GOAL := help

.PHONY: help build run clean

help:
	@echo "Uso: make <alvo>"
	@echo
	@echo "Alvos disponíveis:"
	@echo "  build    Compila o programa"
	@echo "  run      Executa o programa (depende de build)"
	@echo "  clean    Remove o binário gerado"
	@echo "  help     Exibe esta mensagem"

build: $(BIN)
	@echo "Compilação concluída."

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -lwiringPi -o $(BIN)

run: build
	@echo "Executando $(BIN)..."
	# CORRIGIDO: Adicionado um '-' no início da linha.
	# Isso faz com que o 'make' ignore o erro de interrupção (Ctrl+C),
	# permitindo que o nosso programa trate o sinal e encerre de forma limpa.
	-./$(BIN)

clean:
	@rm -f $(BIN)
	@echo "Binário removido."