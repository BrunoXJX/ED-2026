#include "catalogo_artigos.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

/*
 * sdbm — hash de strings simples e bem distribuida.
 */
unsigned long catalogo_hash_texto(const char *texto)
{
    unsigned long h = 0UL;
    int           c;
    if (!texto) return 1UL;
    while ((c = (unsigned char) *texto++) != 0) {
        h = (unsigned long) c + (h << 6) + (h << 16) - h;
    }
    return h ? h : 1UL;
}

/*
 * xorshift32 — gerador de pseudo-aleatorios pequenissimo,
 * suficiente para gerar designacoes e precos. NAO se usa rand()
 * para nao depender do estado global da libc nem da seed do
 * sistema.
 */
static unsigned long xorshift_passo(unsigned long *estado)
{
    unsigned long x = *estado;
    x ^= (x << 13) & 0xFFFFFFFFUL;
    x ^= (x >> 17);
    x ^= (x << 5)  & 0xFFFFFFFFUL;
    if (x == 0UL) x = 0x9E3779B9UL;  /* recuperacao se a seed degenerar */
    *estado = x;
    return x;
}

Artigo_t *catalogo_construir_cesto(const char *semente_texto,
                                   int         quantidade,
                                   int         preco_max_int)
{
    Artigo_t      *cesto;
    unsigned long  estado;
    int            i;
    int            preco_centimos;

    if (quantidade <= 0)         return NULL;
    if (preco_max_int <= 0)      preco_max_int = 1;

    cesto = (Artigo_t *) vigia_pedir((size_t) quantidade * sizeof(Artigo_t));
    if (!cesto) return NULL;

    estado = catalogo_hash_texto(semente_texto);

    for (i = 0; i < quantidade; i++) {
        /* nome no formato art_NN, com NN em base 36 reduzida */
        unsigned long n = xorshift_passo(&estado) % 9999UL;
        snprintf(cesto[i].designacao, DESIGNACAO_MAX, "art_%04lu", n);

        /* preco entre 1 cent e preco_max_int * 100 cents */
        preco_centimos = (int) (xorshift_passo(&estado) % (unsigned long) (preco_max_int * 100)) + 1;
        cesto[i].preco = (double) preco_centimos / 100.0;
    }
    return cesto;
}

void catalogo_libertar_cesto(Artigo_t *cesto, int quantidade)
{
    (void) quantidade;
    if (cesto) vigia_devolver(cesto);
}

double catalogo_total_cesto(const Artigo_t *cesto, int quantidade)
{
    double soma = 0.0;
    int    i;
    if (!cesto) return 0.0;
    for (i = 0; i < quantidade; i++) soma += cesto[i].preco;
    return soma;
}

int catalogo_indice_mais_caro(const Artigo_t *cesto, int quantidade)
{
    int    i;
    int    melhor = -1;
    double mp     = -1.0;
    if (!cesto || quantidade <= 0) return -1;
    for (i = 0; i < quantidade; i++) {
        if (cesto[i].preco > mp) {
            mp     = cesto[i].preco;
            melhor = i;
        }
    }
    return melhor;
}

void catalogo_imprimir_cesto(const Artigo_t *cesto, int quantidade)
{
    int i;
    if (!cesto || quantidade <= 0) {
        printf("    (cesto vazio)\n");
        return;
    }
    for (i = 0; i < quantidade; i++) {
        printf("    [%2d] %-22s %7.2f EUR\n",
               i + 1, cesto[i].designacao, cesto[i].preco);
    }
}

int catalogo_tempo_servico(const char *nome, int qtd, int tempo_produto_max)
{
    unsigned long estado;
    int           i, soma = 0;
    int           intervalo;

    if (qtd <= 0) return 0;
    if (tempo_produto_max < 2) tempo_produto_max = 2;
    intervalo = tempo_produto_max - 2 + 1;  /* [2, tempo_produto_max] */

    /* semente diferente da do cesto para nao correlacionar */
    estado = catalogo_hash_texto(nome);
    estado ^= 0xDEADBEEFUL;
    if (estado == 0UL) estado = 1UL;

    for (i = 0; i < qtd; i++) {
        soma += 2 + (int) (xorshift_passo(&estado) % (unsigned long) intervalo);
    }
    return soma;
}
