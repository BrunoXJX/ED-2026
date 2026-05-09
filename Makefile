# =====================================================================
#  Makefile — Compra Aqui Lda. (Gestao de Caixas, ED 2025/2026)
# =====================================================================
#  Compila com avisos rigorosos e gera o executavel "compraaqui".
#  Funciona tanto com GNU make como mingw32-make.
#
#  Alvos:
#    make             — compila tudo (default = compraaqui)
#    make run         — compila e corre
#    make clean       — apaga objectos e binarios
#    make rebuild     — clean + tudo
# =====================================================================

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -O2 -g -Isrc
LDFLAGS =

SRC_DIR = src
BUILD   = build

SOURCES = \
    $(SRC_DIR)/vigilante_memoria.c   \
    $(SRC_DIR)/configuracao.c        \
    $(SRC_DIR)/catalogo_artigos.c    \
    $(SRC_DIR)/ficha_cliente.c       \
    $(SRC_DIR)/fila_atendimento.c    \
    $(SRC_DIR)/terminal_caixa.c      \
    $(SRC_DIR)/elenco_terminais.c    \
    $(SRC_DIR)/indice_pesquisa.c     \
    $(SRC_DIR)/calendario_eventos.c  \
    $(SRC_DIR)/cronista.c            \
    $(SRC_DIR)/supermercado.c        \
    $(SRC_DIR)/motor_simulacao.c     \
    $(SRC_DIR)/painel_metricas.c     \
    $(SRC_DIR)/leitor_dados.c        \
    $(SRC_DIR)/interface_consola.c   \
    $(SRC_DIR)/principal.c

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SOURCES))

EXEC = compraaqui

.PHONY: all run clean rebuild

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -rf $(BUILD) $(EXEC) $(EXEC).exe

rebuild: clean all
