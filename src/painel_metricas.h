/*
 * painel_metricas.h
 * -----------------
 * Estatisticas finais e diagnosticos. Usa as metricas
 * incrementais que ja existem no Supermercado_t (sem ter de
 * recalcular nada do zero), com excepcao dos rankings entre
 * terminais — esses sao percorridos uma vez no fim.
 */
#ifndef PAINEL_METRICAS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho PAINEL_METRICAS_H. */
#define PAINEL_METRICAS_H  /* linha: Define a macro ou constante simbolica PAINEL_METRICAS_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

/* Mostra um snapshot do estado actual. */
void painel_snapshot(const Supermercado_t *s);  /* linha: Declara a funcao painel_snapshot para uso por outros modulos. */

/* Relatorio detalhado por terminal. */
void painel_relatorio_terminais(const Supermercado_t *s);  /* linha: Declara a funcao painel_relatorio_terminais para uso por outros modulos. */

/* Estatisticas globais finais. */
void painel_relatorio_global(const Supermercado_t *s);  /* linha: Declara a funcao painel_relatorio_global para uso por outros modulos. */

/* Diagnostico de consistencia interna. Devolve 1 se ok. */
int  painel_diagnostico(const Supermercado_t *s);  /* linha: Declara a funcao painel_diagnostico para uso por outros modulos. */

/* Exporta um resumo .txt para o disco com as principais metricas. */
int  painel_exportar_resumo(const Supermercado_t *s, const char *caminho);  /* linha: Declara a funcao painel_exportar_resumo para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
