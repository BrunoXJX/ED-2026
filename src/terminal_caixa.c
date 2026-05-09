#include "terminal_caixa.h"
#include "ficha_cliente.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

/*
 * Catalogo de nomes de operadores. Sao escolhidos de forma
 * deterministica a partir do id, para que reexecutoes
 * coerentes do programa produzam sempre os mesmos nomes.
 */
static const char *NOMES_OPERADORES[] = {
    "Ana Pereira",   "Bruno Sousa",  "Carla Lopes",   "Diogo Reis",
    "Eva Castro",    "Fabio Mendes", "Gisela Tavares","Hugo Marques",
    "Ines Cardoso",  "Joao Vidal",   "Karina Neves",  "Luis Rocha",
    "Mariana Pinto", "Nuno Garcia",  "Olga Antunes",  "Pedro Costa"
};
static const int N_NOMES_OPERADORES = sizeof(NOMES_OPERADORES) / sizeof(NOMES_OPERADORES[0]);

void terminal_gerar_nome_operador(int id, char *destino, int max)
{
    if (!destino || max <= 0) return;
    if (id < 0) id = -id;
    strncpy(destino, NOMES_OPERADORES[id % N_NOMES_OPERADORES], (size_t) (max - 1));
    destino[max - 1] = '\0';
}

Terminal_t *terminal_criar(int id)
{
    Terminal_t *t;

    t = (Terminal_t *) vigia_pedir_z(sizeof(Terminal_t));
    if (!t) return NULL;

    t->id     = id;
    t->estado = EST_TERMINAL_FECHADO;
    fila_iniciar(&t->fila);
    terminal_gerar_nome_operador(id, t->nome_operador, NOME_OPERADOR_MAX);
    return t;
}

void terminal_destruir(Terminal_t *t)
{
    NoAtendido_t *cur, *prox;
    if (!t) return;
    fila_limpar(&t->fila);
    /* libertar lista de atendidos */
    cur = t->lista_atendidos;
    while (cur) {
        prox = cur->seguinte;
        vigia_devolver(cur);
        cur = prox;
    }
    vigia_devolver(t);
}

void terminal_abrir(Terminal_t *t)
{
    if (!t) return;
    t->estado = EST_TERMINAL_ABERTO;
}

void terminal_drenar(Terminal_t *t)
{
    if (!t) return;
    if (t->estado == EST_TERMINAL_ABERTO) t->estado = EST_TERMINAL_DRENANDO;
}

void terminal_fechar(Terminal_t *t)
{
    if (!t) return;
    t->estado = EST_TERMINAL_FECHADO;
}

int terminal_total_pessoas(const Terminal_t *t)
{
    int n;
    if (!t) return 0;
    n = fila_tamanho(&t->fila);
    if (t->em_atendimento) n++;
    return n;
}

long terminal_tempo_estimado_total(const Terminal_t *t)
{
    long s;
    if (!t) return 0;
    s = fila_tempo_estimado_total(&t->fila);
    if (t->em_atendimento) s += t->em_atendimento->tempo_servico_estimado;
    return s;
}

const char *terminal_nome_estado(const Terminal_t *t)
{
    if (!t) return "?";
    switch (t->estado) {
        case EST_TERMINAL_FECHADO:  return "FECHADO";
        case EST_TERMINAL_ABERTO:   return "ABERTO";
        case EST_TERMINAL_DRENANDO: return "DRENANDO";
    }
    return "?";
}

void terminal_imprimir_resumo(const Terminal_t *t)
{
    if (!t) return;
    printf("  Terminal #%-2d  %-9s  oper=%-15s pessoas=%2d  est.servico=%4lds  atend=%3d  art=%4ld  ofertas=%2d\n",
           t->id,
           terminal_nome_estado(t),
           t->nome_operador,
           terminal_total_pessoas(t),
           terminal_tempo_estimado_total(t),
           t->total_atendidos,
           t->total_artigos,
           t->total_ofertas);
}

void terminal_imprimir_detalhe(const Terminal_t *t)
{
    if (!t) return;
    printf("\n+-- Terminal #%d ----------------------------+\n", t->id);
    printf("| operador          : %-22s |\n", t->nome_operador);
    printf("| estado            : %-22s |\n", terminal_nome_estado(t));
    printf("| pessoas (fila+at) : %-22d |\n", terminal_total_pessoas(t));
    printf("| tempo est. total  : %18ld s |\n", terminal_tempo_estimado_total(t));
    printf("| atendidos         : %-22d |\n", t->total_atendidos);
    printf("| artigos process.  : %-22ld |\n", t->total_artigos);
    printf("| t. tot. espera    : %18ld s |\n", t->tempo_total_espera);
    printf("| t. tot. servico   : %18ld s |\n", t->tempo_total_servico);
    printf("| ofertas concedid. : %-22d |\n", t->total_ofertas);
    printf("| prejuizo total    : %18.2f EUR |\n", t->prejuizo_total);
    printf("+--------------------------------------------+\n");
    if (t->em_atendimento) {
        printf("  >> em atendimento agora:\n");
        ficha_imprimir_linha(t->em_atendimento);
    }
    printf("  >> fila:\n");
    fila_imprimir(&t->fila);
}

void terminal_registar_atendido(Terminal_t *t, const FichaCliente_t *f)
{
    NoAtendido_t *no;
    if (!t || !f) return;

    no = (NoAtendido_t *) vigia_pedir(sizeof(NoAtendido_t));
    if (!no) return;

    no->id                 = f->id;
    strncpy(no->nome, f->nome, NOME_FICHA_MAX - 1);
    no->nome[NOME_FICHA_MAX - 1] = '\0';
    no->quantidade_artigos = f->quantidade_artigos;
    no->total_pago         = f->total_pagar;
    no->recebeu_oferta     = f->recebeu_oferta;

    /* insercao na cabeca (O(1)) */
    no->seguinte          = t->lista_atendidos;
    t->lista_atendidos    = no;
    t->total_na_lista_atendidos++;
}

void terminal_imprimir_atendidos(const Terminal_t *t)
{
    NoAtendido_t *cur;
    int           pos = 1;
    if (!t) return;
    printf("\n+-- Atendidos pelo Terminal #%d (%s) ----+\n",
           t->id, t->nome_operador);
    if (!t->lista_atendidos) {
        printf("| (nenhum cliente atendido ainda)           |\n");
    } else {
        for (cur = t->lista_atendidos; cur; cur = cur->seguinte) {
            printf("  %3d. #%-4d %-18s art=%2d  total=%7.2f EUR%s\n",
                   pos, cur->id, cur->nome,
                   cur->quantidade_artigos, cur->total_pago,
                   cur->recebeu_oferta ? "  [OFERTA]" : "");
            pos++;
        }
    }
    printf("+-------------------------------------------+\n");
}
