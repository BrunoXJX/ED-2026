/*
 * terminal_caixa.h
 * ----------------
 * Terminal de caixa. Cada terminal tem:
 *
 *   - identidade (id, nome do operador gerado deterministicamente)
 *   - estado (FECHADO, ABERTO, DRENANDO)
 *   - fila de atendimento
 *   - eventual ficha em atendimento
 *   - contadores incrementais (clientes atendidos, artigos
 *     processados, tempo total de espera, tempo total de
 *     servico, ofertas concedidas, prejuizo)
 *
 * O terminal nao manda em si proprio: as transicoes sao feitas
 * pelo motor de simulacao ou pela interface.
 */
#ifndef TERMINAL_CAIXA_H
#define TERMINAL_CAIXA_H

#include "nucleo_tipos.h"
#include "fila_atendimento.h"

/* No da lista de clientes ja atendidos por este terminal. */
typedef struct no_atendido_s {
    int    id;
    char   nome[NOME_FICHA_MAX];
    int    quantidade_artigos;
    double total_pago;
    int    recebeu_oferta;
    struct no_atendido_s *seguinte;
} NoAtendido_t;

struct terminal_s {
    int               id;
    char              nome_operador[NOME_OPERADOR_MAX];
    EstadoTerminal_t  estado;

    FilaAtendimento_t fila;
    FichaCliente_t   *em_atendimento;

    /* Contadores locais (sao incrementais — actualizados
       sempre que o motor processa um evento, nunca recalculados). */
    int               total_atendidos;
    long              total_artigos;
    long              tempo_total_espera;
    long              tempo_total_servico;
    int               total_ofertas;
    double            prejuizo_total;

    /* Lista ligada dos clientes ja atendidos (historico). */
    NoAtendido_t     *lista_atendidos;
    int               total_na_lista_atendidos;
};

/* Cria um novo terminal no heap. Comeca FECHADO. */
Terminal_t *terminal_criar(int id);

/* Liberta um terminal. Liberta tambem a sua fila (mas nao as
   fichas — a propriedade das fichas e do gestor do supermercado). */
void  terminal_destruir(Terminal_t *t);

/* Transicoes de estado. */
void  terminal_abrir   (Terminal_t *t);
void  terminal_drenar  (Terminal_t *t);   /* deixar de aceitar */
void  terminal_fechar  (Terminal_t *t);   /* fechamento total  */

/* Helpers de leitura. */
int   terminal_total_pessoas(const Terminal_t *t); /* fila + em atendimento */
long  terminal_tempo_estimado_total(const Terminal_t *t);
const char *terminal_nome_estado(const Terminal_t *t);

/* Mostra o terminal no ecra. */
void  terminal_imprimir_resumo(const Terminal_t *t);
void  terminal_imprimir_detalhe(const Terminal_t *t);

/* Gera nomes deterministicos de operador. So depende do id. */
void  terminal_gerar_nome_operador(int id, char *destino, int max);

/* Regista uma copia da ficha na lista de atendidos. */
void  terminal_registar_atendido(Terminal_t *t, const FichaCliente_t *f);

/* Imprime a lista de clientes ja atendidos por este terminal. */
void  terminal_imprimir_atendidos(const Terminal_t *t);

#endif
