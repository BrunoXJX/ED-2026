/*
 * fila_atendimento.h
 * ------------------
 * Fila ligada simples de fichas de cliente. Cada terminal de
 * caixa tem a sua fila. Implementacao com cabeca e cauda para
 * que enfileirar fique em O(1) e desenfileirar tambem.
 *
 * Os nos guardam apenas um ponteiro para a FichaCliente_t —
 * a ficha em si nao e propriedade da fila. A fila pode ser
 * "esvaziada" sem libertar as fichas (util quando uma ficha
 * muda de terminal).
 */
#ifndef FILA_ATENDIMENTO_H
#define FILA_ATENDIMENTO_H

#include "nucleo_tipos.h"

struct no_fila_s {
    FichaCliente_t  *ficha;
    NoFila_t        *seguinte;
};

struct fila_atendimento_s {
    NoFila_t *frente;
    NoFila_t *tras;
    int       total;
};

/* Inicializa uma fila vazia (a estrutura ja tem que existir). */
void fila_iniciar(FilaAtendimento_t *f);

/* Numero de fichas na fila. */
int  fila_tamanho(const FilaAtendimento_t *f);

/* Adiciona uma ficha ao fim da fila. Devolve 1 se ok. */
int  fila_inserir_atras(FilaAtendimento_t *f, FichaCliente_t *ficha);

/* Adiciona uma ficha no inicio (usado para reposicao). */
int  fila_inserir_frente(FilaAtendimento_t *f, FichaCliente_t *ficha);

/* Remove e devolve a ficha da frente. NULL se vazia. */
FichaCliente_t *fila_extrair_frente(FilaAtendimento_t *f);

/* Espreita a ficha da frente sem a remover. */
FichaCliente_t *fila_espreitar_frente(const FilaAtendimento_t *f);

/* Remove uma ficha especifica da fila pelo id. Devolve a
   ficha removida ou NULL se nao foi encontrada. Util para
   movimentar entre terminais. */
FichaCliente_t *fila_remover_por_id(FilaAtendimento_t *f, int id_ficha);

/* Soma dos tempos de servico estimados de todas as fichas
   da fila. Util para a politica de balanceamento por menor
   tempo de espera. */
long fila_tempo_estimado_total(const FilaAtendimento_t *f);

/* Liberta todos os nos (mas nao as fichas). */
void fila_limpar(FilaAtendimento_t *f);

/* Imprime resumo da fila. */
void fila_imprimir(const FilaAtendimento_t *f);

#endif
