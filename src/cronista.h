/*
 * cronista.h
 * ----------
 * Sistema de logs duplo. Existem dois ficheiros CSV separados:
 *
 *   logs/acoes_utilizador.csv  — auditoria humana. So contem
 *      acoes feitas pelo utilizador no menu (abrir, fechar,
 *      mover cliente, pesquisa, etc.).
 *
 *   logs/eventos_motor.csv     — rastreio tecnico. Contem
 *      eventos automaticos do motor (atendimento iniciado/
 *      terminado, oferta concedida, abertura/fecho automatico,
 *      ticks de verificacao).
 *
 *   logs/historico_simulacao.csv — fotografia numerica do
 *      estado no fim de cada tick, util para relatorios e
 *      validacao em folha de calculo.
 *
 * A separacao permite filtrar facilmente o que vem do utilizador
 * e o que vem do sistema sem ter de fazer "grep" no mesmo ficheiro.
 */
#ifndef CRONISTA_H
#define CRONISTA_H

#include "nucleo_tipos.h"
#include <stdio.h>

struct cronista_s {
    FILE *acoes;
    FILE *motor;
    FILE *historico;
};

/* Abre os ficheiros CSV de auditoria e historico da simulacao
   (cria a pasta logs se necessario). */
int  cronista_abrir(Cronista_t *c);

/* Fecha os ficheiros. */
void cronista_fechar(Cronista_t *c);

/* Regista uma acao do utilizador.
   Linha CSV: tick;categoria;descricao */
void cronista_acao(Cronista_t *c, int tick,
                   const char *categoria, const char *descricao);

/* Regista um evento automatico do motor.
   Linha CSV: tick;tipo;descricao */
void cronista_evento(Cronista_t *c, int tick,
                     const char *tipo, const char *descricao);

/* Guarda uma fotografia numerica do estado actual. E chamada
   pelo motor no fim de cada tick para permitir analisar a
   evolucao da simulacao em folha de calculo. */
void cronista_snapshot(Cronista_t *c, const Supermercado_t *s);

#endif
