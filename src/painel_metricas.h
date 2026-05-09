/*
 * painel_metricas.h
 * -----------------
 * Estatisticas finais e diagnosticos. Usa as metricas
 * incrementais que ja existem no Supermercado_t (sem ter de
 * recalcular nada do zero), com excepcao dos rankings entre
 * terminais — esses sao percorridos uma vez no fim.
 */
#ifndef PAINEL_METRICAS_H
#define PAINEL_METRICAS_H

#include "nucleo_tipos.h"

/* Mostra um snapshot do estado actual. */
void painel_snapshot(const Supermercado_t *s);

/* Relatorio detalhado por terminal. */
void painel_relatorio_terminais(const Supermercado_t *s);

/* Estatisticas globais finais. */
void painel_relatorio_global(const Supermercado_t *s);

/* Diagnostico de consistencia interna. Devolve 1 se ok. */
int  painel_diagnostico(const Supermercado_t *s);

/* Exporta um resumo .txt para o disco com as principais metricas. */
int  painel_exportar_resumo(const Supermercado_t *s, const char *caminho);

#endif
