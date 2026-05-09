#include "configuracao.h"

#include <stdio.h>

void configuracao_defeitos(Configuracao_t *c)
{
    if (!c) return;
    c->max_espera       = D_MAX_ESPERA;
    c->n_caixas         = D_N_CAIXAS;
    c->tempo_produto    = D_TEMPO_PRODUTO;
    c->max_preco        = D_MAX_PRECO;
    c->max_fila         = D_MAX_FILA;
    c->min_fila         = D_MIN_FILA;
    c->tick_verificacao = D_TICK_VERIFICACAO;
}

int configuracao_valida(const Configuracao_t *c)
{
    if (!c)                          return 0;
    if (c->max_espera       < 1)     return 0;
    if (c->n_caixas         < 1)     return 0;
    if (c->tempo_produto    < 1)     return 0;
    if (c->max_preco        < 1)     return 0;
    if (c->min_fila         < 0)     return 0;
    if (c->max_fila         < 1)     return 0;
    if (c->min_fila         >= c->max_fila) return 0;
    if (c->tick_verificacao < 1)     return 0;
    return 1;
}

void configuracao_mostrar(const Configuracao_t *c)
{
    if (!c) return;
    printf("\n+-- Configuracao do Supermercado -----------------+\n");
    printf("| MAX_ESPERA      : %5d s                       |\n", c->max_espera);
    printf("| N_CAIXAS        : %5d                         |\n", c->n_caixas);
    printf("| TEMPO_PRODUTO   : %5d s/produto               |\n", c->tempo_produto);
    printf("| MAX_PRECO       : %5d EUR                     |\n", c->max_preco);
    printf("| MAX_FILA        : %5d clientes                |\n", c->max_fila);
    printf("| MIN_FILA        : %5d clientes                |\n", c->min_fila);
    printf("| TICK_VERIF.     : %5d ticks                   |\n", c->tick_verificacao);
    printf("+-------------------------------------------------+\n");
}
