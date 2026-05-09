/*
 * ficha_cliente.h
 * ---------------
 * Ficha individual de um cliente. O nome "ficha" e proposital
 * — num supermercado real cada cliente fidelizado tem uma
 * ficha. A estrutura agrupa tudo o que precisamos saber sobre
 * a sua passagem pelo sistema:
 *
 *   - identificacao (id e nome)
 *   - cesto de artigos (gerado pelo modulo catalogo)
 *   - tempos (entrada, inicio de servico, saida)
 *   - terminal onde se encontra
 *   - estado (aguarda, atendimento, processada)
 *   - flag de oferta e indice do produto oferecido
 *
 * O campo "versao" serve a tecnica de lazy deletion no
 * calendario de eventos: cada vez que a ficha muda de fila ou
 * recebe acao directa do utilizador, a versao incrementa, e
 * eventos antigos no calendario com versao diferente sao
 * descartados em silencio.
 */
#ifndef FICHA_CLIENTE_H
#define FICHA_CLIENTE_H

#include "nucleo_tipos.h"

struct ficha_cliente_s {
    int            id;
    char           nome[NOME_FICHA_MAX];

    int            quantidade_artigos;
    Artigo_t      *cesto;
    double         total_pagar;

    int            instante_chegada;
    int            instante_inicio_servico;
    int            instante_saida;
    int            tempo_servico_estimado;

    int            id_terminal_actual;     /* -1 se nenhum  */
    EstadoFicha_t  estado;

    int            recebeu_oferta;
    int            indice_oferta;          /* -1 se nenhum  */
    double         valor_oferta;

    int            versao;                 /* lazy deletion */
};

/* Cria uma ficha completa, gerando o cesto. */
FichaCliente_t *ficha_criar(int         id,
                            const char *nome,
                            int         qtd_artigos,
                            int         tempo_produto,
                            int         max_preco_int);

/* Liberta uma ficha (incluindo o cesto). */
void ficha_destruir(FichaCliente_t *f);

/* Mostra detalhes da ficha no ecra. */
void ficha_imprimir_detalhe(const FichaCliente_t *f);

/* Mostra resumo (uma linha). */
void ficha_imprimir_linha(const FichaCliente_t *f);

/* Marca uma oferta para a ficha. Se ja recebeu, nao faz nada. */
void ficha_marcar_oferta(FichaCliente_t *f);

/* Helper: nome textual do estado. */
const char *ficha_nome_estado(EstadoFicha_t e);

#endif
