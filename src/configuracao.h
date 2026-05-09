/*
 * configuracao.h
 * --------------
 * Tabela de parametros do supermercado. E carregada do
 * ficheiro Configuracao.txt no arranque ou preenchida com
 * os valores por defeito.
 *
 * Os nomes seguem o enunciado: MAX_ESPERA, N_CAIXAS, etc.
 */
#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

#include "nucleo_tipos.h"

/* Valores por defeito vindos do enunciado. */
#define D_MAX_ESPERA       120
#define D_N_CAIXAS         6
#define D_TEMPO_PRODUTO    6
#define D_MAX_PRECO        40
#define D_MAX_FILA         7
#define D_MIN_FILA         3
#define D_TICK_VERIFICACAO 5      /* periodicidade da verificacao automatica */

struct configuracao_s {
    int max_espera;
    int n_caixas;
    int tempo_produto;
    int max_preco;
    int max_fila;
    int min_fila;
    int tick_verificacao;
};

/* Preenche com os valores por defeito. */
void configuracao_defeitos(Configuracao_t *c);

/* Verifica se a tabela e coerente. Devolve 1 se ok, 0 se nao. */
int  configuracao_valida (const Configuracao_t *c);

/* Mostra os valores no ecra. */
void configuracao_mostrar(const Configuracao_t *c);

#endif
