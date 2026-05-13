#include "catalogo_artigos.h"  /* linha: Importa as declaracoes locais de catalogo_artigos.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

/*
 * sdbm — hash de strings simples e bem distribuida.
 */
unsigned long catalogo_hash_texto(const char *texto)  /* linha: Inicia ou continua a definicao da funcao catalogo_hash_texto. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    unsigned long h = 0UL;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int           c;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!texto) return 1UL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    while ((c = (unsigned char) *texto++) != 0) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        h = (unsigned long) c + (h << 6) + (h << 16) - h;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return h ? h : 1UL;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/*
 * xorshift32 — gerador de pseudo-aleatorios pequenissimo,
 * suficiente para gerar designacoes e precos. NAO se usa rand()
 * para nao depender do estado global da libc nem da seed do
 * sistema.
 */
static unsigned long xorshift_passo(unsigned long *estado)  /* linha: Inicia ou continua a definicao da funcao xorshift_passo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    unsigned long x = *estado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    x ^= (x << 13) & 0xFFFFFFFFUL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    x ^= (x >> 17);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    x ^= (x << 5)  & 0xFFFFFFFFUL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (x == 0UL) x = 0x9E3779B9UL;  /* recuperacao se a seed degenerar */  /* linha: Testa a condicao antes de executar o bloco associado. */
    *estado = x;
    return x;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Artigo_t *catalogo_construir_cesto(const char *semente_texto,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                   int         quantidade,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                                   int         preco_max_int)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Artigo_t      *cesto;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    unsigned long  estado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            preco_centimos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (quantidade <= 0)         return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (preco_max_int <= 0)      preco_max_int = 1;  /* linha: Testa a condicao antes de executar o bloco associado. */

    cesto = (Artigo_t *) vigia_pedir((size_t) quantidade * sizeof(Artigo_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!cesto) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    estado = catalogo_hash_texto(semente_texto);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    for (i = 0; i < quantidade; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        /* nome no formato art_NN, com NN em base 36 reduzida */
        unsigned long n = xorshift_passo(&estado) % 9999UL;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
        snprintf(cesto[i].designacao, DESIGNACAO_MAX, "art_%04lu", n);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */

        /* preco entre 1 cent e preco_max_int * 100 cents */
        preco_centimos = (int) (xorshift_passo(&estado) % (unsigned long) (preco_max_int * 100)) + 1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        cesto[i].preco = (double) preco_centimos / 100.0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return cesto;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void catalogo_libertar_cesto(Artigo_t *cesto, int quantidade)  /* linha: Inicia ou continua a definicao da funcao catalogo_libertar_cesto. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    (void) quantidade;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (cesto) vigia_devolver(cesto);  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */

double catalogo_total_cesto(const Artigo_t *cesto, int quantidade)  /* linha: Inicia ou continua a definicao da funcao catalogo_total_cesto. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    double soma = 0.0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int    i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!cesto) return 0.0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (i = 0; i < quantidade; i++) soma += cesto[i].preco;  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
    return soma;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int catalogo_indice_mais_caro(const Artigo_t *cesto, int quantidade)  /* linha: Inicia ou continua a definicao da funcao catalogo_indice_mais_caro. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int    i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int    melhor = -1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double mp     = -1.0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!cesto || quantidade <= 0) return -1;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (i = 0; i < quantidade; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cesto[i].preco > mp) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            mp     = cesto[i].preco;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            melhor = i;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void catalogo_imprimir_cesto(const Artigo_t *cesto, int quantidade)  /* linha: Inicia ou continua a definicao da funcao catalogo_imprimir_cesto. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!cesto || quantidade <= 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("    (cesto vazio)\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        return;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    for (i = 0; i < quantidade; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        printf("    [%2d] %-22s %7.2f EUR\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
               i + 1, cesto[i].designacao, cesto[i].preco);  /* linha: Acede a um campo de uma estrutura local. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int catalogo_tempo_servico(const char *nome, int qtd, int tempo_produto_max)  /* linha: Inicia ou continua a definicao da funcao catalogo_tempo_servico. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    unsigned long estado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int           i, soma = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int           intervalo;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (qtd <= 0) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (tempo_produto_max < 2) tempo_produto_max = 2;  /* linha: Testa a condicao antes de executar o bloco associado. */
    intervalo = tempo_produto_max - 2 + 1;  /* [2, tempo_produto_max] */  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    /* semente diferente da do cesto para nao correlacionar */
    estado = catalogo_hash_texto(nome);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    estado ^= 0xDEADBEEFUL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (estado == 0UL) estado = 1UL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (i = 0; i < qtd; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        soma += 2 + (int) (xorshift_passo(&estado) % (unsigned long) intervalo);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return soma;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
