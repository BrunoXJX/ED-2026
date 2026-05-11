#include "calendario_eventos.h"
#include "ficha_cliente.h"
#include "terminal_caixa.h"
#include "vigilante_memoria.h"

#include <stdio.h>

void calendario_iniciar(Calendario_t *c)
{
    if (!c) return;
    c->cabeca = NULL;
    c->total  = 0;
}

void calendario_libertar(Calendario_t *c)
{
    Evento_t *cur, *prox;
    if (!c) return;
    cur = c->cabeca;
    while (cur) {
        prox = cur->seguinte;
        vigia_devolver(cur);
        cur = prox;
    }
    c->cabeca = NULL;
    c->total  = 0;
}

int calendario_agendar(Calendario_t   *c,
                       int             instante,
                       TipoEvento_t    tipo,
                       Terminal_t     *terminal,
                       FichaCliente_t *ficha)
{
    Evento_t *novo, *cur, *ant;
    if (!c) return 0;

    novo = (Evento_t *) vigia_pedir(sizeof(Evento_t));
    if (!novo) return 0;

    novo->instante     = instante;
    novo->tipo         = tipo;
    novo->terminal     = terminal;
    novo->ficha        = ficha;
    novo->versao_ficha = (ficha) ? ficha->versao : 0;
    novo->seguinte     = NULL;

   /* Inserção ordenada por instante crescente.
     *
     * Caso especial: lista vazia ou novo instante é o mais pequeno. */
    if (!c->cabeca || c->cabeca->instante > instante) {
        novo->seguinte = c->cabeca;
        c->cabeca      = novo;
        c->total++;
        return 1;
    }

    /* Caso geral: avança até encontrar o ponto de inserção.
     * Garantia: o loop executa pelo menos uma vez (c->cabeca->instante
     * <= instante), por isso ant != NULL no final.                    */
    cur = c->cabeca;
    ant = NULL;
    while (cur && cur->instante <= instante) {
        ant = cur;
        cur = cur->seguinte;
    }
    /* ant != NULL aqui — o assert documenta a invariante. */
    assert(ant != NULL);
    novo->seguinte = cur;
    ant->seguinte  = novo;

    c->total++;
    return 1;
}


int calendario_proximo_instante(const Calendario_t *c)
{
    if (!c || !c->cabeca) return -1;
    return c->cabeca->instante;
}

Evento_t *calendario_extrair_proximo(Calendario_t *c)
{
    Evento_t *e;
    if (!c || !c->cabeca) return NULL;
    e = c->cabeca;
    c->cabeca = e->seguinte;
    e->seguinte = NULL;
    c->total--;
    return e;
}

void calendario_libertar_evento(Evento_t *e)
{
    if (e) vigia_devolver(e);
}

int calendario_total(const Calendario_t *c)
{
    return (c) ? c->total : 0;
}

int calendario_total_obsoletos(const Calendario_t *c)
{
    Evento_t *cur;
    int       n = 0;
    if (!c) return 0;
    for (cur = c->cabeca; cur; cur = cur->seguinte) {
        if (cur->ficha && cur->ficha->versao != cur->versao_ficha) n++;
    }
    return n;
}

static const char *nome_tipo(TipoEvento_t t)
{
    switch (t) {
        case EVT_FIM_ATENDIMENTO:   return "FIM_ATENDIMENTO";
        case EVT_OFERTA_DEVIDA:     return "OFERTA_DEVIDA";
        case EVT_VERIFICAR_BALANCO: return "VERIFICAR_BALANCO";
    }
    return "?";
}

void calendario_imprimir(const Calendario_t *c)
{
    Evento_t *cur;
    if (!c || !c->cabeca) {
        printf("  (calendario vazio)\n");
        return;
    }
    for (cur = c->cabeca; cur; cur = cur->seguinte) {
        printf("  t=%5d  %-18s",
               cur->instante, nome_tipo(cur->tipo));
        if (cur->terminal) printf("  term=#%d", cur->terminal->id);
        if (cur->ficha)    printf("  ficha=#%d v=%d/%d",
                                  cur->ficha->id,
                                  cur->versao_ficha,
                                  cur->ficha->versao);
        printf("\n");
    }
}
