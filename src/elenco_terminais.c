#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

void elenco_iniciar(ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_iniciar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!e) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    e->cabeca = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    e->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void elenco_libertar(ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_libertar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!e) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    cur = e->cabeca;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (cur->terminal) terminal_destruir(cur->terminal);  /* linha: Testa a condicao antes de executar o bloco associado. */
        vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    e->cabeca = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    e->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int elenco_adicionar(ElencoTerminais_t *e, Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao elenco_adicionar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *no, *cur, *ant;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!e || !t) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no = (NoTerminal_t *) vigia_pedir(sizeof(NoTerminal_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!no) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    no->terminal = t;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->seguinte = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (!e->cabeca || e->cabeca->terminal->id > t->id) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        no->seguinte = e->cabeca;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        e->cabeca    = no;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
        cur = e->cabeca;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        ant = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        while (cur && cur->terminal->id < t->id) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
            ant = cur;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        no->seguinte = cur;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (ant) ant->seguinte = no;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    e->total++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Terminal_t *elenco_obter_por_id(const ElencoTerminais_t *e, int id)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!e) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->id == id) return cur->terminal;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return NULL;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int elenco_total(const ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_total. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return (e) ? e->total : 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int elenco_total_abertos(const ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_total_abertos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           n = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!e) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_ABERTO) n++;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return n;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int elenco_total_activos(const ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_total_activos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           n = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!e) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->estado != EST_TERMINAL_FECHADO) n++;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return n;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Terminal_t *elenco_melhor_para_novo(const ElencoTerminais_t *e)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t   *melhor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    long          melhor_t = -1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!e) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_ABERTO) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            long t = terminal_tempo_estimado_total(cur->terminal);  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
            if (melhor == NULL || t < melhor_t) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                melhor   = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                melhor_t = t;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Terminal_t *elenco_primeiro_fechado(const ElencoTerminais_t *e)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!e) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_FECHADO) return cur->terminal;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return NULL;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void elenco_imprimir(const ElencoTerminais_t *e)  /* linha: Inicia ou continua a definicao da funcao elenco_imprimir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!e || !e->cabeca) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("  (sem terminais)\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        return;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    for (cur = e->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        terminal_imprimir_resumo(cur->terminal);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */
