#include "ficha_cliente.h"
#include "catalogo_artigos.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

FichaCliente_t *ficha_criar(int         id,
                            const char *nome,
                            int         qtd_artigos,
                            int         tempo_produto,
                            int         max_preco_int)
{
    FichaCliente_t *f;

    f = (FichaCliente_t *) vigia_pedir_z(sizeof(FichaCliente_t));
    if (!f) return NULL;

    f->id = id;

    if (nome) {
        strncpy(f->nome, nome, NOME_FICHA_MAX - 1);
        f->nome[NOME_FICHA_MAX - 1] = '\0';
    } else {
        snprintf(f->nome, NOME_FICHA_MAX, "anonimo_%d", id);
    }

    f->quantidade_artigos = qtd_artigos;
    f->cesto              = catalogo_construir_cesto(f->nome, qtd_artigos, max_preco_int);
    f->total_pagar        = catalogo_total_cesto(f->cesto, qtd_artigos);

    f->tempo_servico_estimado  = catalogo_tempo_servico(f->nome, qtd_artigos, tempo_produto);
    f->instante_chegada        = 0;
    f->instante_inicio_servico = -1;
    f->instante_saida          = -1;

    f->id_terminal_actual = -1;
    f->estado             = EST_FICHA_AGUARDA;

    f->recebeu_oferta = 0;
    f->indice_oferta  = -1;
    f->valor_oferta   = 0.0;
    f->versao         = 1;

    return f;
}

void ficha_destruir(FichaCliente_t *f)
{
    if (!f) return;
    if (f->cesto) catalogo_libertar_cesto(f->cesto, f->quantidade_artigos);
    vigia_devolver(f);
}

const char *ficha_nome_estado(EstadoFicha_t e)
{
    switch (e) {
        case EST_FICHA_AGUARDA:     return "AGUARDA";
        case EST_FICHA_ATENDIMENTO: return "ATENDIMENTO";
        case EST_FICHA_PROCESSADA:  return "PROCESSADA";
    }
    return "?";
}

void ficha_imprimir_detalhe(const FichaCliente_t *f)
{
    if (!f) return;
    printf("\n+-- Ficha #%d ----------------------------------+\n", f->id);
    printf("| nome              : %-28s |\n", f->nome);
    printf("| estado            : %-28s |\n", ficha_nome_estado(f->estado));
    printf("| terminal actual   : %-28d |\n", f->id_terminal_actual);
    printf("| artigos no cesto  : %-28d |\n", f->quantidade_artigos);
    printf("| total a pagar     : %23.2f EUR |\n", f->total_pagar);
    printf("| t. servico est.   : %23d s |\n", f->tempo_servico_estimado);
    printf("| t. chegada        : %23d s |\n", f->instante_chegada);
    printf("| t. inicio serv.   : %23d s |\n", f->instante_inicio_servico);
    printf("| t. saida          : %23d s |\n", f->instante_saida);
    printf("| recebeu oferta    : %-28s |\n", f->recebeu_oferta ? "sim" : "nao");
    if (f->recebeu_oferta && f->indice_oferta >= 0) {
        printf("| artigo oferecido  : %-22s %.2f EUR |\n",
               f->cesto[f->indice_oferta].designacao,
               f->cesto[f->indice_oferta].preco);
    }
    printf("+-----------------------------------------------+\n");
}

void ficha_imprimir_linha(const FichaCliente_t *f)
{
    if (!f) return;
    printf("  #%-4d %-18s art=%2d  t.serv=%4ds  estado=%-11s term=%2d\n",
           f->id, f->nome, f->quantidade_artigos,
           f->tempo_servico_estimado, ficha_nome_estado(f->estado),
           f->id_terminal_actual);
}

void ficha_marcar_oferta(FichaCliente_t *f)
{
    int idx;
    if (!f || f->recebeu_oferta) return;
    idx = catalogo_indice_mais_caro(f->cesto, f->quantidade_artigos);
    if (idx < 0) return;
    f->recebeu_oferta = 1;
    f->indice_oferta  = idx;
    f->valor_oferta   = f->cesto[idx].preco;
}
