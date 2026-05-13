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
#ifndef CATALOGO_ARTIGOS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho CATALOGO_ARTIGOS_H. */
#define CATALOGO_ARTIGOS_H  /* linha: Define a macro ou constante simbolica CATALOGO_ARTIGOS_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct artigo_s {  /* linha: Comeca a estrutura artigo_s e agrupa os seus campos. */
    char   designacao[DESIGNACAO_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    double preco;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Cria um cesto com @quantidade artigos. A semente vem do nome
   do cliente. */
Artigo_t *catalogo_construir_cesto(const char *semente_texto,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                   int         quantidade,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                                   int         preco_max_int);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Liberta o cesto. */
void      catalogo_libertar_cesto (Artigo_t *cesto, int quantidade);  /* linha: Declara a funcao catalogo_libertar_cesto para uso por outros modulos. */

/* Soma o valor total dos artigos do cesto. */
double    catalogo_total_cesto    (const Artigo_t *cesto, int quantidade);  /* linha: Declara a funcao catalogo_total_cesto para uso por outros modulos. */

/* Devolve o indice do artigo mais caro (ou -1 se vazio). */
int       catalogo_indice_mais_caro(const Artigo_t *cesto, int quantidade);  /* linha: Declara a funcao catalogo_indice_mais_caro para uso por outros modulos. */

/* Imprime o cesto no ecra. */
void      catalogo_imprimir_cesto (const Artigo_t *cesto, int quantidade);  /* linha: Declara a funcao catalogo_imprimir_cesto para uso por outros modulos. */

/* Hash sdbm — usada para a semente do cesto. Util tambem
   para o indice de pesquisa. */
unsigned long catalogo_hash_texto(const char *texto);  /* linha: Declara a funcao catalogo_hash_texto para uso por outros modulos. */

/* Calcula o tempo de servico total, com tempo por produto
   aleatorio no intervalo [2, tempo_produto_max].
   Deterministico: depende apenas do nome. */
int catalogo_tempo_servico(const char *nome, int qtd, int tempo_produto_max);  /* linha: Declara a funcao catalogo_tempo_servico para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
