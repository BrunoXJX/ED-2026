/*
 * indice_pesquisa.h
 * -----------------
 * Tabela hash com encadeamento separado para fichas. Mantida
 * em paralelo com o estado real do supermercado: sempre que
 * uma ficha entra no sistema, e indexada; quando sai (e
 * libertada), e removida.
 *
 * A chave e o nome da ficha. A funcao de hash usada e a sdbm
 * fornecida pelo modulo catalogo_artigos.
 *
 * Tamanho da tabela: INDICE_HASH_TAM (primo).
 */
#ifndef INDICE_PESQUISA_H
#define INDICE_PESQUISA_H

#include "nucleo_tipos.h"

struct no_indice_s {
    FichaCliente_t *ficha;
    NoIndice_t     *seguinte;
};

struct indice_pesquisa_s {
    NoIndice_t *baldes[INDICE_HASH_TAM];
    int         total;
};

void            indice_iniciar (IndicePesquisa_t *i);
void            indice_libertar(IndicePesquisa_t *i);

/* Insere a ficha. Devolve 1 se ok. */
int             indice_inserir (IndicePesquisa_t *i, FichaCliente_t *f);

/* Remove a ficha pelo nome. Devolve 1 se removeu. */
int             indice_remover (IndicePesquisa_t *i, const char *nome);

/* Procura por nome. NULL se nao existir. */
FichaCliente_t *indice_procurar(const IndicePesquisa_t *i, const char *nome);

/* Numero de fichas indexadas. */
int             indice_total   (const IndicePesquisa_t *i);

/* Estatistica simples: distribuicao por baldes. */
void            indice_imprimir_estatistica(const IndicePesquisa_t *i);

#endif
