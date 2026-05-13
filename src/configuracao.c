#include "configuracao.h"  /* linha: Importa as declaracoes locais de configuracao.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

void configuracao_defeitos(Configuracao_t *c)  /* linha: Inicia ou continua a definicao da funcao configuracao_defeitos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    c->max_espera       = D_MAX_ESPERA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->n_caixas         = D_N_CAIXAS;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->tempo_produto    = D_TEMPO_PRODUTO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->max_preco        = D_MAX_PRECO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->max_fila         = D_MAX_FILA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->min_fila         = D_MIN_FILA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->tick_verificacao = D_TICK_VERIFICACAO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int configuracao_valida(const Configuracao_t *c)  /* linha: Inicia ou continua a definicao da funcao configuracao_valida. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c)                          return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->max_espera       < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->n_caixas         < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->tempo_produto    < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->max_preco        < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->min_fila         < 0)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->max_fila         < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->min_fila         >= c->max_fila) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->tick_verificacao < 1)     return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void configuracao_mostrar(const Configuracao_t *c)  /* linha: Inicia ou continua a definicao da funcao configuracao_mostrar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n+-- Configuracao do Supermercado -----------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| MAX_ESPERA      : %5d s                       |\n", c->max_espera);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| N_CAIXAS        : %5d                         |\n", c->n_caixas);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| TEMPO_PRODUTO   : %5d s/produto               |\n", c->tempo_produto);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| MAX_PRECO       : %5d EUR                     |\n", c->max_preco);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| MAX_FILA        : %5d clientes                |\n", c->max_fila);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| MIN_FILA        : %5d clientes                |\n", c->min_fila);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| TICK_VERIF.     : %5d ticks                   |\n", c->tick_verificacao);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("+-------------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
