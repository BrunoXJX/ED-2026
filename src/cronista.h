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
#ifndef CRONISTA_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho CRONISTA_H. */
#define CRONISTA_H  /* linha: Define a macro ou constante simbolica CRONISTA_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */
#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

struct cronista_s {  /* linha: Comeca a estrutura cronista_s e agrupa os seus campos. */
    FILE *acoes;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FILE *motor;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FILE *historico;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
};  /* linha: Fecha a declaracao composta atual. */

/* Abre os ficheiros CSV de auditoria e historico da simulacao
   (cria a pasta logs se necessario). */
int  cronista_abrir(Cronista_t *c);  /* linha: Declara a funcao cronista_abrir para uso por outros modulos. */

/* Fecha os ficheiros. */
void cronista_fechar(Cronista_t *c);  /* linha: Declara a funcao cronista_fechar para uso por outros modulos. */

/* Regista uma acao do utilizador.
   Linha CSV: tick;categoria;descricao */
void cronista_acao(Cronista_t *c, int tick,  /* linha: Inicia ou continua a definicao da funcao cronista_acao. */
                   const char *categoria, const char *descricao);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Regista um evento automatico do motor.
   Linha CSV: tick;tipo;descricao */
void cronista_evento(Cronista_t *c, int tick,  /* linha: Inicia ou continua a definicao da funcao cronista_evento. */
                     const char *tipo, const char *descricao);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Guarda uma fotografia numerica do estado actual. E chamada
   pelo motor no fim de cada tick para permitir analisar a
   evolucao da simulacao em folha de calculo. */
void cronista_snapshot(Cronista_t *c, const Supermercado_t *s);  /* linha: Declara a funcao cronista_snapshot para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
