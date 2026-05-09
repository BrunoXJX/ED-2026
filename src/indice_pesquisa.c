#include "indice_pesquisa.h"
#include "ficha_cliente.h"
#include "catalogo_artigos.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

void indice_iniciar(IndicePesquisa_t *i)
{
    int k;
    if (!i) return;
    for (k = 0; k < INDICE_HASH_TAM; k++) i->baldes[k] = NULL;
    i->total = 0;
}

void indice_libertar(IndicePesquisa_t *i)
{
    int         k;
    NoIndice_t *cur, *prox;
    if (!i) return;
    for (k = 0; k < INDICE_HASH_TAM; k++) {
        cur = i->baldes[k];
        while (cur) {
            prox = cur->seguinte;
            vigia_devolver(cur);
            cur = prox;
        }
        i->baldes[k] = NULL;
    }
    i->total = 0;
}

int indice_inserir(IndicePesquisa_t *i, FichaCliente_t *f)
{
    NoIndice_t *no;
    int         pos;
    if (!i || !f) return 0;

    /* nao indexamos duplicados pelo mesmo nome */
    if (indice_procurar(i, f->nome)) return 0;

    no = (NoIndice_t *) vigia_pedir(sizeof(NoIndice_t));
    if (!no) return 0;
    no->ficha = f;

    pos = (int) (catalogo_hash_texto(f->nome) % (unsigned long) INDICE_HASH_TAM);
    no->seguinte = i->baldes[pos];
    i->baldes[pos] = no;
    i->total++;
    return 1;
}

int indice_remover(IndicePesquisa_t *i, const char *nome)
{
    NoIndice_t *cur, *ant;
    int         pos;
    if (!i || !nome) return 0;
    pos = (int) (catalogo_hash_texto(nome) % (unsigned long) INDICE_HASH_TAM);
    cur = i->baldes[pos];
    ant = NULL;
    while (cur) {
        if (cur->ficha && strcmp(cur->ficha->nome, nome) == 0) {
            if (ant) ant->seguinte = cur->seguinte;
            else     i->baldes[pos] = cur->seguinte;
            vigia_devolver(cur);
            i->total--;
            return 1;
        }
        ant = cur;
        cur = cur->seguinte;
    }
    return 0;
}

FichaCliente_t *indice_procurar(const IndicePesquisa_t *i, const char *nome)
{
    NoIndice_t *cur;
    int         pos;
    if (!i || !nome) return NULL;
    pos = (int) (catalogo_hash_texto(nome) % (unsigned long) INDICE_HASH_TAM);
    cur = i->baldes[pos];
    while (cur) {
        if (cur->ficha && strcmp(cur->ficha->nome, nome) == 0) return cur->ficha;
        cur = cur->seguinte;
    }
    return NULL;
}

int indice_total(const IndicePesquisa_t *i)
{
    return (i) ? i->total : 0;
}

void indice_imprimir_estatistica(const IndicePesquisa_t *i)
{
    int         k, n;
    int         max_balde = 0;
    int         baldes_ocupados = 0;
    NoIndice_t *cur;
    if (!i) return;

    for (k = 0; k < INDICE_HASH_TAM; k++) {
        n = 0;
        for (cur = i->baldes[k]; cur; cur = cur->seguinte) n++;
        if (n > 0) baldes_ocupados++;
        if (n > max_balde) max_balde = n;
    }
    printf("\n+-- Indice de Pesquisa ---------------------------+\n");
    printf("| total fichas    : %-29d |\n", i->total);
    printf("| baldes da tab.  : %-29d |\n", INDICE_HASH_TAM);
    printf("| baldes ocupados : %-29d |\n", baldes_ocupados);
    printf("| balde mais alto : %-29d |\n", max_balde);
    printf("+-------------------------------------------------+\n");
}
