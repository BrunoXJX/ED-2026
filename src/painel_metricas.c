#include "painel_metricas.h"
#include "supermercado.h"
#include "elenco_terminais.h"
#include "terminal_caixa.h"
#include "fila_atendimento.h"
#include "indice_pesquisa.h"
#include "calendario_eventos.h"
#include "ficha_cliente.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

void painel_snapshot(const Supermercado_t *s)
{
    if (!s) return;
    printf("\n=== SNAPSHOT (tick=%d) ============================\n", s->tick_actual);
    printf(" terminais: %d total, %d abertos, %d activos\n",
           elenco_total(&s->elenco),
           elenco_total_abertos(&s->elenco),
           elenco_total_activos(&s->elenco));
    printf(" clientes:  %d em espera, %d ja atendidos\n",
           supermercado_total_clientes_em_espera(s),
           s->total_clientes_atendidos);
    printf(" calendario: %d eventos pendentes (%d obsoletos)\n",
           calendario_total(&s->calendario),
           calendario_total_obsoletos(&s->calendario));
    printf(" ofertas concedidas: %d (prejuizo=%.2f EUR)\n",
           s->total_ofertas_concedidas, s->prejuizo_acumulado);
    printf("---------------------------------------------------\n");
    elenco_imprimir(&s->elenco);
}

void painel_relatorio_terminais(const Supermercado_t *s)
{
    NoTerminal_t *cur;
    if (!s) return;
    printf("\n=== Relatorio por Terminal =========================\n");
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        terminal_imprimir_detalhe(cur->terminal);
    }
}

/* Encontra o terminal que atendeu mais clientes. */
static const Terminal_t *terminal_mais_atendimentos(const Supermercado_t *s)
{
    NoTerminal_t     *cur;
    const Terminal_t *melhor = NULL;
    if (!s) return NULL;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (!cur->terminal) continue;
        if (!melhor || cur->terminal->total_atendidos > melhor->total_atendidos) {
            melhor = cur->terminal;
        }
    }
    return melhor;
}

/* Terminal com mais artigos processados. */
static const Terminal_t *terminal_mais_artigos(const Supermercado_t *s)
{
    NoTerminal_t     *cur;
    const Terminal_t *melhor = NULL;
    if (!s) return NULL;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (!cur->terminal) continue;
        if (!melhor || cur->terminal->total_artigos > melhor->total_artigos) {
            melhor = cur->terminal;
        }
    }
    return melhor;
}

/* Operador menos activo (menor numero de atendimentos). */
static const Terminal_t *terminal_menos_activo(const Supermercado_t *s)
{
    NoTerminal_t     *cur;
    const Terminal_t *pior = NULL;
    if (!s) return NULL;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (!cur->terminal) continue;
        /* exclui terminais que nunca foram abertos para nao
           penalizar quem nem chegou a trabalhar */
        if (cur->terminal->total_atendidos == 0
            && cur->terminal->estado == EST_TERMINAL_FECHADO) continue;
        if (!pior || cur->terminal->total_atendidos < pior->total_atendidos) {
            pior = cur->terminal;
        }
    }
    return pior;
}

void painel_relatorio_global(const Supermercado_t *s)
{
    const Terminal_t *t1, *t2, *t3;
    double            media_espera = 0.0, media_servico = 0.0;
    size_t            mem_uso, mem_pico, mem_des;

    if (!s) return;

    if (s->total_clientes_atendidos > 0) {
        media_espera  = (double) s->soma_tempos_espera  / (double) s->total_clientes_atendidos;
        media_servico = (double) s->soma_tempos_servico / (double) s->total_clientes_atendidos;
    }

    mem_uso  = vigia_em_uso();
    mem_pico = vigia_pico();
    mem_des  = supermercado_memoria_desperdicada(s);

    printf("\n=== Estatisticas Finais ============================\n");
    printf(" tick final                : %d\n", s->tick_actual);
    printf(" terminais (total/aberto)  : %d / %d\n",
           elenco_total(&s->elenco), elenco_total_abertos(&s->elenco));
    printf(" clientes atendidos        : %d\n", s->total_clientes_atendidos);
    printf(" clientes ainda no sistema : %d\n", s->total_clientes_no_sistema);
    printf(" tempo medio de espera     : %.2f s\n", media_espera);
    printf(" tempo medio de servico    : %.2f s\n", media_servico);
    printf(" ofertas concedidas        : %d\n", s->total_ofertas_concedidas);
    printf(" prejuizo total acumulado  : %.2f EUR\n", s->prejuizo_acumulado);
    printf(" ----- memoria -----\n");
    printf(" em uso (real)             : %lu bytes\n", (unsigned long) mem_uso);
    printf(" pico                      : %lu bytes\n", (unsigned long) mem_pico);
    printf(" desperdicada (calculada)  : %lu bytes\n", (unsigned long) mem_des);
    printf(" blocos vivos              : %lu\n",       (unsigned long) vigia_blocos_vivos());

    t1 = terminal_mais_atendimentos(s);
    t2 = terminal_mais_artigos(s);
    t3 = terminal_menos_activo(s);

    printf(" ----- distincoes -----\n");
    if (t1) printf(" mais clientes atendidos   : terminal #%d (%s) — %d clientes\n",
                   t1->id, t1->nome_operador, t1->total_atendidos);
    if (t2) printf(" mais artigos processados  : terminal #%d (%s) — %ld artigos\n",
                   t2->id, t2->nome_operador, t2->total_artigos);
    if (t3) printf(" operador menos activo     : terminal #%d (%s) — %d clientes\n",
                   t3->id, t3->nome_operador, t3->total_atendidos);
    printf("====================================================\n");
}

int painel_diagnostico(const Supermercado_t *s)
{
    NoTerminal_t *cur;
    int           ok = 1;
    int           pessoas = 0;

    if (!s) return 0;

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (!cur->terminal) { ok = 0; printf(" [DIAG] no sem terminal\n"); continue; }
        pessoas += terminal_total_pessoas(cur->terminal);
        if (cur->terminal->em_atendimento
            && cur->terminal->em_atendimento->id_terminal_actual != cur->terminal->id)
        {
            printf(" [DIAG] ficha em atendimento com id_terminal errado (term=%d)\n",
                   cur->terminal->id);
            ok = 0;
        }
    }
    if (s->total_clientes_no_sistema < 0) {
        printf(" [DIAG] total_clientes_no_sistema negativo\n");
        ok = 0;
    }
    printf("\n>> Diagnostico: %s (pessoas vivas no sistema=%d)\n",
           ok ? "OK" : "PROBLEMAS", pessoas);
    return ok;
}

int painel_exportar_resumo(const Supermercado_t *s, const char *caminho)
{
    FILE *f;
    if (!s || !caminho) return 0;
    f = fopen(caminho, "w");
    if (!f) return 0;

    fprintf(f, "Resumo final da simulacao\n");
    fprintf(f, "tick_final;%d\n", s->tick_actual);
    fprintf(f, "atendidos;%d\n", s->total_clientes_atendidos);
    fprintf(f, "no_sistema;%d\n", s->total_clientes_no_sistema);
    fprintf(f, "ofertas;%d\n", s->total_ofertas_concedidas);
    fprintf(f, "prejuizo;%.2f\n", s->prejuizo_acumulado);
    fprintf(f, "memoria_em_uso;%lu\n", (unsigned long) vigia_em_uso());
    fprintf(f, "memoria_pico;%lu\n",   (unsigned long) vigia_pico());
    fprintf(f, "memoria_desperdicada;%lu\n",
            (unsigned long) supermercado_memoria_desperdicada(s));

    fclose(f);
    return 1;
}
