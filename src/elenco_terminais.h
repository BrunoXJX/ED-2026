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
#ifndef ELENCO_TERMINAIS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho ELENCO_TERMINAIS_H. */
#define ELENCO_TERMINAIS_H  /* linha: Define a macro ou constante simbolica ELENCO_TERMINAIS_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct no_terminal_s {  /* linha: Comeca a estrutura no_terminal_s e agrupa os seus campos. */
    Terminal_t   *terminal;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    NoTerminal_t *seguinte;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
};  /* linha: Fecha a declaracao composta atual. */

struct elenco_terminais_s {  /* linha: Comeca a estrutura elenco_terminais_s e agrupa os seus campos. */
    NoTerminal_t *cabeca;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

void        elenco_iniciar (ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_iniciar para uso por outros modulos. */
void        elenco_libertar(ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_libertar para uso por outros modulos. */

/* Adiciona um terminal mantendo a lista por id crescente. */
int         elenco_adicionar(ElencoTerminais_t *e, Terminal_t *t);  /* linha: Declara a funcao elenco_adicionar para uso por outros modulos. */

/* Procura terminal pelo id. NULL se nao existir. */
Terminal_t *elenco_obter_por_id(const ElencoTerminais_t *e, int id);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Numero de terminais (qualquer estado). */
int         elenco_total      (const ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_total para uso por outros modulos. */

/* Numero de terminais que aceitam novos clientes (i.e. ABERTO). */
int         elenco_total_abertos(const ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_total_abertos para uso por outros modulos. */

/* Numero de terminais activos = ABERTO + DRENANDO. */
int         elenco_total_activos(const ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_total_activos para uso por outros modulos. */

/* Devolve o terminal aberto com o menor tempo estimado de
   espera. NULL se nenhum aberto. Politica de balanceamento. */
Terminal_t *elenco_melhor_para_novo(const ElencoTerminais_t *e);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Devolve o primeiro terminal FECHADO encontrado, ou NULL. */
Terminal_t *elenco_primeiro_fechado(const ElencoTerminais_t *e);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Imprime resumo de todos os terminais. */
void        elenco_imprimir(const ElencoTerminais_t *e);  /* linha: Declara a funcao elenco_imprimir para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
