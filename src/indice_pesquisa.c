#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "catalogo_artigos.h"  /* linha: Importa as declaracoes locais de catalogo_artigos.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

void indice_iniciar(IndicePesquisa_t *i)  /* linha: Inicia ou continua a definicao da funcao indice_iniciar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int k;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!i) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (k = 0; k < INDICE_HASH_TAM; k++) i->baldes[k] = NULL;  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
    i->total = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void indice_libertar(IndicePesquisa_t *i)  /* linha: Inicia ou continua a definicao da funcao indice_libertar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int         k;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoIndice_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!i) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (k = 0; k < INDICE_HASH_TAM; k++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        cur = i->baldes[k];  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
            prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        i->baldes[k] = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    i->total = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int indice_inserir(IndicePesquisa_t *i, FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao indice_inserir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoIndice_t *no;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int         pos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!i || !f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* nao indexamos duplicados pelo mesmo nome */
    if (indice_procurar(i, f->nome)) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no = (NoIndice_t *) vigia_pedir(sizeof(NoIndice_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!no) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    no->ficha = f;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    pos = (int) (catalogo_hash_texto(f->nome) % (unsigned long) INDICE_HASH_TAM);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->seguinte = i->baldes[pos];  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    i->baldes[pos] = no;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    i->total++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int indice_remover(IndicePesquisa_t *i, const char *nome)  /* linha: Inicia ou continua a definicao da funcao indice_remover. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoIndice_t *cur, *ant;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int         pos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!i || !nome) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    pos = (int) (catalogo_hash_texto(nome) % (unsigned long) INDICE_HASH_TAM);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    cur = i->baldes[pos];  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    ant = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        if (cur->ficha && strcmp(cur->ficha->nome, nome) == 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            if (ant) ant->seguinte = cur->seguinte;  /* linha: Testa a condicao antes de executar o bloco associado. */
            else     i->baldes[pos] = cur->seguinte;  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
            vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            i->total--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
            return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        ant = cur;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

FichaCliente_t *indice_procurar(const IndicePesquisa_t *i, const char *nome)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoIndice_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int         pos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!i || !nome) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    pos = (int) (catalogo_hash_texto(nome) % (unsigned long) INDICE_HASH_TAM);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    cur = i->baldes[pos];  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        if (cur->ficha && strcmp(cur->ficha->nome, nome) == 0) return cur->ficha;  /* linha: Testa a condicao antes de executar o bloco associado. */
        cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return NULL;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int indice_total(const IndicePesquisa_t *i)  /* linha: Inicia ou continua a definicao da funcao indice_total. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return (i) ? i->total : 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void indice_imprimir_estatistica(const IndicePesquisa_t *i)  /* linha: Inicia ou continua a definicao da funcao indice_imprimir_estatistica. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int         k, n;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int         max_balde = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int         baldes_ocupados = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoIndice_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!i) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (k = 0; k < INDICE_HASH_TAM; k++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        n = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        for (cur = i->baldes[k]; cur; cur = cur->seguinte) n++;  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (n > 0) baldes_ocupados++;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (n > max_balde) max_balde = n;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    printf("\n+-- Indice de Pesquisa ---------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| total fichas    : %-29d |\n", i->total);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| baldes da tab.  : %-29d |\n", INDICE_HASH_TAM);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| baldes ocupados : %-29d |\n", baldes_ocupados);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| balde mais alto : %-29d |\n", max_balde);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("+-------------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
