#include "elenco_terminais.h"
#include "terminal_caixa.h"
#include "vigilante_memoria.h"

#include <stdio.h>

void elenco_iniciar(ElencoTerminais_t *e)
{
    if (!e) return;
    e->cabeca = NULL;
    e->total  = 0;
}

void elenco_libertar(ElencoTerminais_t *e)
{
    NoTerminal_t *cur, *prox;
    if (!e) return;
    cur = e->cabeca;
    while (cur) {
        prox = cur->seguinte;
        if (cur->terminal) terminal_destruir(cur->terminal);
        vigia_devolver(cur);
        cur = prox;
    }
    e->cabeca = NULL;
    e->total  = 0;
}

int elenco_adicionar(ElencoTerminais_t *e, Terminal_t *t)
{
    NoTerminal_t *no, *cur, *ant;
    if (!e || !t) return 0;

    no = (NoTerminal_t *) vigia_pedir(sizeof(NoTerminal_t));
    if (!no) return 0;
    no->terminal = t;
    no->seguinte = NULL;

    if (!e->cabeca || e->cabeca->terminal->id > t->id) {
        no->seguinte = e->cabeca;
        e->cabeca    = no;
    } else {
        cur = e->cabeca;
        ant = NULL;
        while (cur && cur->terminal->id < t->id) {
            ant = cur;
            cur = cur->seguinte;
        }
        no->seguinte = cur;
        if (ant) ant->seguinte = no;
    }
    e->total++;
    return 1;
}

Terminal_t *elenco_obter_por_id(const ElencoTerminais_t *e, int id)
{
    NoTerminal_t *cur;
    if (!e) return NULL;
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->id == id) return cur->terminal;
    }
    return NULL;
}

int elenco_total(const ElencoTerminais_t *e)
{
    return (e) ? e->total : 0;
}

int elenco_total_abertos(const ElencoTerminais_t *e)
{
    NoTerminal_t *cur;
    int           n = 0;
    if (!e) return 0;
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_ABERTO) n++;
    }
    return n;
}

int elenco_total_activos(const ElencoTerminais_t *e)
{
    NoTerminal_t *cur;
    int           n = 0;
    if (!e) return 0;
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->estado != EST_TERMINAL_FECHADO) n++;
    }
    return n;
}

Terminal_t *elenco_melhor_para_novo(const ElencoTerminais_t *e)
{
    NoTerminal_t *cur;
    Terminal_t   *melhor = NULL;
    long          melhor_t = -1;
    if (!e) return NULL;
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_ABERTO) {
            long t = terminal_tempo_estimado_total(cur->terminal);
            if (melhor == NULL || t < melhor_t) {
                melhor   = cur->terminal;
                melhor_t = t;
            }
        }
    }
    return melhor;
}

Terminal_t *elenco_primeiro_fechado(const ElencoTerminais_t *e)
{
    NoTerminal_t *cur;
    if (!e) return NULL;
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_FECHADO) return cur->terminal;
    }
    return NULL;
}

void elenco_imprimir(const ElencoTerminais_t *e)
{
    NoTerminal_t *cur;
    if (!e || !e->cabeca) {
        printf("  (sem terminais)\n");
        return;
    }
    for (cur = e->cabeca; cur; cur = cur->seguinte) {
        terminal_imprimir_resumo(cur->terminal);
    }
}
