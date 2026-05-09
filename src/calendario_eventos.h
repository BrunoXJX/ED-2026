/*
 * calendario_eventos.h
 * --------------------
 * Calendario de eventos pendentes da simulacao. E uma lista
 * ligada simples mantida ORDENADA por instante crescente.
 *
 * Cada evento contem um instante (em ticks de simulacao), um
 * tipo, e referencias para o terminal e/ou ficha envolvidos.
 * O campo "versao_ficha" permite a tecnica de lazy deletion:
 * o motor compara a versao do evento com a versao actual da
 * ficha; se forem diferentes, o evento esta obsoleto e e
 * descartado em silencio.
 *
 * Esta abordagem evita ter de remover eventos do meio da lista
 * sempre que uma ficha muda de fila ou recebe acao directa.
 */
#ifndef CALENDARIO_EVENTOS_H
#define CALENDARIO_EVENTOS_H

#include "nucleo_tipos.h"

struct evento_s {
    int             instante;
    TipoEvento_t    tipo;
    Terminal_t     *terminal;
    FichaCliente_t *ficha;
    int             versao_ficha;
    Evento_t       *seguinte;
};

struct calendario_s {
    Evento_t *cabeca;
    int       total;
};

void calendario_iniciar (Calendario_t *c);
void calendario_libertar(Calendario_t *c);

/* Agenda um novo evento. Insere ordenado por instante. */
int  calendario_agendar(Calendario_t   *c,
                        int             instante,
                        TipoEvento_t    tipo,
                        Terminal_t     *terminal,
                        FichaCliente_t *ficha);

/* Devolve o instante do proximo evento (ou -1 se vazio). */
int  calendario_proximo_instante(const Calendario_t *c);

/* Retira e devolve o evento mais proximo. NULL se vazio. */
Evento_t *calendario_extrair_proximo(Calendario_t *c);

/* Liberta um evento devolvido por _extrair_proximo. */
void      calendario_libertar_evento(Evento_t *e);

/* Numero de eventos pendentes (incluindo obsoletos). */
int       calendario_total(const Calendario_t *c);

/* Conta quantos eventos estao obsoletos (versao desactualizada).
   Util para a contagem de memoria desperdicada. */
int       calendario_total_obsoletos(const Calendario_t *c);

/* Imprime o calendario para depuracao. */
void      calendario_imprimir(const Calendario_t *c);

#endif
