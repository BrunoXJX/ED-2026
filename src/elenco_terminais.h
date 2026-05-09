/*
 * elenco_terminais.h
 * ------------------
 * Lista ligada de terminais. Em vez de usar um array fixo,
 * usamos uma lista para que abrir/fechar caixas possa
 * acrescentar/manter terminais sem realocacoes nem limites
 * artificiais.
 *
 * Os terminais sao mantidos por id crescente.
 */
#ifndef ELENCO_TERMINAIS_H
#define ELENCO_TERMINAIS_H

#include "nucleo_tipos.h"

struct no_terminal_s {
    Terminal_t   *terminal;
    NoTerminal_t *seguinte;
};

struct elenco_terminais_s {
    NoTerminal_t *cabeca;
    int           total;
};

void        elenco_iniciar (ElencoTerminais_t *e);
void        elenco_libertar(ElencoTerminais_t *e);

/* Adiciona um terminal mantendo a lista por id crescente. */
int         elenco_adicionar(ElencoTerminais_t *e, Terminal_t *t);

/* Procura terminal pelo id. NULL se nao existir. */
Terminal_t *elenco_obter_por_id(const ElencoTerminais_t *e, int id);

/* Numero de terminais (qualquer estado). */
int         elenco_total      (const ElencoTerminais_t *e);

/* Numero de terminais que aceitam novos clientes (i.e. ABERTO). */
int         elenco_total_abertos(const ElencoTerminais_t *e);

/* Numero de terminais activos = ABERTO + DRENANDO. */
int         elenco_total_activos(const ElencoTerminais_t *e);

/* Devolve o terminal aberto com o menor tempo estimado de
   espera. NULL se nenhum aberto. Politica de balanceamento. */
Terminal_t *elenco_melhor_para_novo(const ElencoTerminais_t *e);

/* Devolve o primeiro terminal FECHADO encontrado, ou NULL. */
Terminal_t *elenco_primeiro_fechado(const ElencoTerminais_t *e);

/* Imprime resumo de todos os terminais. */
void        elenco_imprimir(const ElencoTerminais_t *e);

#endif
