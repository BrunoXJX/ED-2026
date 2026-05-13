#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <assert.h>  /* linha: Importa assert para documentar invariantes internas em tempo de execucao. */
#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

void calendario_iniciar(Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_iniciar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    c->cabeca = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void calendario_libertar(Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_libertar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!c) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    cur = c->cabeca;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    c->cabeca = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int calendario_agendar(Calendario_t   *c,  /* linha: Inicia ou continua a definicao da funcao calendario_agendar. */
                       int             instante,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                       TipoEvento_t    tipo,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                       Terminal_t     *terminal,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                       FichaCliente_t *ficha)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *novo, *cur, *ant;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!c) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    novo = (Evento_t *) vigia_pedir(sizeof(Evento_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!novo) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    novo->instante     = instante;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    novo->tipo         = tipo;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    novo->terminal     = terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    novo->ficha        = ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    novo->versao_ficha = (ficha) ? ficha->versao : 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    novo->seguinte     = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    /* Insercao ordenada por instante crescente. */
    if (!c->cabeca || c->cabeca->instante > instante) {  /* linha: Testa o caso especial em que a lista esta vazia ou o novo evento fica no inicio. */
        novo->seguinte = c->cabeca;  /* linha: Liga o novo evento ao antigo primeiro elemento da lista. */
        c->cabeca      = novo;  /* linha: Atualiza a cabeca para o novo evento passar a ser o primeiro. */
        c->total++;  /* linha: Atualiza o contador de eventos guardados no calendario. */
        return 1;  /* linha: Termina a funcao indicando que o agendamento foi bem sucedido. */
    }  /* linha: Fecha o bloco do caso especial de insercao no inicio. */

    /* Caso geral: avancar ate encontrar o ponto de insercao. */
    cur = c->cabeca;  /* linha: Comeca a procura pelo primeiro evento da lista. */
    ant = NULL;  /* linha: Inicializa o ponteiro anterior antes de percorrer a lista. */
    while (cur && cur->instante <= instante) {  /* linha: Avanca enquanto o evento atual deve ficar antes do novo. */
        ant = cur;  /* linha: Guarda o evento anterior ao ponto onde o novo sera inserido. */
        cur = cur->seguinte;  /* linha: Passa para o proximo evento da lista ordenada. */
    }  /* linha: Fecha o ciclo de procura da posicao correta. */
    assert(ant != NULL);  /* linha: Confirma a invariante de que existe um elemento anterior no caso geral. */
    novo->seguinte = cur;  /* linha: Liga o novo evento ao elemento que ficara depois dele. */
    ant->seguinte  = novo;  /* linha: Liga o elemento anterior ao novo evento inserido. */

    c->total++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int calendario_proximo_instante(const Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_proximo_instante. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c || !c->cabeca) return -1;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return c->cabeca->instante;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Evento_t *calendario_extrair_proximo(Calendario_t *c)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *e;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!c || !c->cabeca) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    e = c->cabeca;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->cabeca = e->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    e->seguinte = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->total--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return e;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void calendario_libertar_evento(Evento_t *e)  /* linha: Inicia ou continua a definicao da funcao calendario_libertar_evento. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (e) vigia_devolver(e);  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int calendario_total(const Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_total. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return (c) ? c->total : 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int calendario_total_obsoletos(const Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_total_obsoletos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int       n = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!c) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = c->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->ficha && cur->ficha->versao != cur->versao_ficha) n++;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return n;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static const char *nome_tipo(TipoEvento_t t)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    switch (t) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case EVT_FIM_ATENDIMENTO:   return "FIM_ATENDIMENTO";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EVT_OFERTA_DEVIDA:     return "OFERTA_DEVIDA";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EVT_VERIFICAR_BALANCO: return "VERIFICAR_BALANCO";  /* linha: Define um caso especifico dentro da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return "?";  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void calendario_imprimir(const Calendario_t *c)  /* linha: Inicia ou continua a definicao da funcao calendario_imprimir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!c || !c->cabeca) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("  (calendario vazio)\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        return;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    for (cur = c->cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        printf("  t=%5d  %-18s",  /* linha: Mostra texto formatado no ecra para o utilizador. */
               cur->instante, nome_tipo(cur->tipo));  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        if (cur->terminal) printf("  term=#%d", cur->terminal->id);  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (cur->ficha)    printf("  ficha=#%d v=%d/%d",  /* linha: Testa a condicao antes de executar o bloco associado. */
                                  cur->ficha->id,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                                  cur->versao_ficha,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                                  cur->ficha->versao);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        printf("\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */
