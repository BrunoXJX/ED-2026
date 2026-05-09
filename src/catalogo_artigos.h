/*
 * catalogo_artigos.h
 * ------------------
 * Cesto de artigos de um cliente. O ficheiro de dados so traz
 * a quantidade de artigos por cliente — o enunciado pede que
 * cada artigo tenha nome e preco. Resolvemos a ambiguidade
 * gerando os artigos deterministicamente:
 *
 *   1. semente = hash(nome do cliente)
 *   2. um xorshift32 produz a sequencia de pseudo-aleatorios
 *   3. cada artigo recebe um nome no formato "art_NN" e um
 *      preco entre 1 e MAX_PRECO em centimos
 *
 * Determinismo importa: uma reexecucao com os mesmos dados
 * produz exactamente os mesmos cestos. Isto facilita testes
 * e a defesa oral.
 */
#ifndef CATALOGO_ARTIGOS_H
#define CATALOGO_ARTIGOS_H

#include "nucleo_tipos.h"

struct artigo_s {
    char   designacao[DESIGNACAO_MAX];
    double preco;
};

/* Cria um cesto com @quantidade artigos. A semente vem do nome
   do cliente. */
Artigo_t *catalogo_construir_cesto(const char *semente_texto,
                                   int         quantidade,
                                   int         preco_max_int);

/* Liberta o cesto. */
void      catalogo_libertar_cesto (Artigo_t *cesto, int quantidade);

/* Soma o valor total dos artigos do cesto. */
double    catalogo_total_cesto    (const Artigo_t *cesto, int quantidade);

/* Devolve o indice do artigo mais caro (ou -1 se vazio). */
int       catalogo_indice_mais_caro(const Artigo_t *cesto, int quantidade);

/* Imprime o cesto no ecra. */
void      catalogo_imprimir_cesto (const Artigo_t *cesto, int quantidade);

/* Hash sdbm — usada para a semente do cesto. Util tambem
   para o indice de pesquisa. */
unsigned long catalogo_hash_texto(const char *texto);

/* Calcula o tempo de servico total, com tempo por produto
   aleatorio no intervalo [2, tempo_produto_max].
   Deterministico: depende apenas do nome. */
int catalogo_tempo_servico(const char *nome, int qtd, int tempo_produto_max);

#endif
