#include "fila_atendimento.h"
#include "ficha_cliente.h"
#include "vigilante_memoria.h"

#include <stdio.h>

void fila_iniciar(FilaAtendimento_t *f)
{
    if (!f) return;
    f->frente = NULL;
    f->tras   = NULL;
    f->total  = 0;
}

int fila_tamanho(const FilaAtendimento_t *f)
{
    return (f) ? f->total : 0;
}

int fila_inserir_atras(FilaAtendimento_t *f, FichaCliente_t *ficha)
{
    NoFila_t *no;
    if (!f || !ficha) return 0;

    no = (NoFila_t *) vigia_pedir(sizeof(NoFila_t));
    if (!no) return 0;

    no->ficha    = ficha;
    no->seguinte = NULL;

    if (f->tras) f->tras->seguinte = no;
    else         f->frente         = no;
    f->tras = no;
    f->total++;
    return 1;
}

int fila_inserir_frente(FilaAtendimento_t *f, FichaCliente_t *ficha)
{
    NoFila_t *no;
    if (!f || !ficha) return 0;

    no = (NoFila_t *) vigia_pedir(sizeof(NoFila_t));
    if (!no) return 0;

    no->ficha    = ficha;
    no->seguinte = f->frente;

    f->frente = no;
    if (!f->tras) f->tras = no;
    f->total++;
    return 1;
}

FichaCliente_t *fila_extrair_frente(FilaAtendimento_t *f)
{
    NoFila_t       *no;
    FichaCliente_t *ficha;

    if (!f || !f->frente) return NULL;

    no       = f->frente;
    ficha    = no->ficha;
    f->frente = no->seguinte;
    if (!f->frente) f->tras = NULL;

    vigia_devolver(no);
    f->total--;
    return ficha;
}

FichaCliente_t *fila_espreitar_frente(const FilaAtendimento_t *f)
{
    if (!f || !f->frente) return NULL;
    return f->frente->ficha;
}

FichaCliente_t *fila_remover_por_id(FilaAtendimento_t *f, int id_ficha)
{
    NoFila_t       *cur, *ant;
    FichaCliente_t *ficha;

    if (!f || !f->frente) return NULL;

    cur = f->frente;
    ant = NULL;
    while (cur) {
        if (cur->ficha && cur->ficha->id == id_ficha) {
            if (ant) ant->seguinte = cur->seguinte;
            else     f->frente     = cur->seguinte;
            if (cur == f->tras) f->tras = ant;

            ficha = cur->ficha;
            vigia_devolver(cur);
            f->total--;
            return ficha;
        }
        ant = cur;
        cur = cur->seguinte;
    }
    return NULL;
}

long fila_tempo_estimado_total(const FilaAtendimento_t *f)
{
    long      soma = 0;
    NoFila_t *cur;
    if (!f) return 0;
    cur = f->frente;
    while (cur) {
        if (cur->ficha) soma += cur->ficha->tempo_servico_estimado;
        cur = cur->seguinte;
    }
    return soma;
}

void fila_limpar(FilaAtendimento_t *f)
{
    NoFila_t *cur, *prox;
    if (!f) return;
    cur = f->frente;
    while (cur) {
        prox = cur->seguinte;
        vigia_devolver(cur);
        cur = prox;
    }
    f->frente = NULL;
    f->tras   = NULL;
    f->total  = 0;
}

void fila_imprimir(const FilaAtendimento_t *f)
{
    NoFila_t *cur;
    int       pos = 1;
    if (!f) return;
    if (!f->frente) {
        printf("    (fila vazia)\n");
        return;
    }
    cur = f->frente;
    while (cur) {
        if (cur->ficha) {
            printf("    [%2d] ", pos);
            ficha_imprimir_linha(cur->ficha);
        }
        pos++;
        cur = cur->seguinte;
    }
}
