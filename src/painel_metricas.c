#include "painel_metricas.h"  /* linha: Importa as declaracoes locais de painel_metricas.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */
#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

void painel_snapshot(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao painel_snapshot. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n=== SNAPSHOT (tick=%d) ============================\n", s->tick_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" terminais: %d total, %d abertos, %d activos\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           elenco_total(&s->elenco),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           elenco_total_abertos(&s->elenco),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           elenco_total_activos(&s->elenco));  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf(" clientes:  %d em espera, %d ja atendidos\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           supermercado_total_clientes_em_espera(s),  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
           s->total_clientes_atendidos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf(" calendario: %d eventos pendentes (%d obsoletos)\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           calendario_total(&s->calendario),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           calendario_total_obsoletos(&s->calendario));  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf(" ofertas concedidas: %d (prejuizo=%.2f EUR)\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           s->total_ofertas_concedidas, s->prejuizo_acumulado);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf("---------------------------------------------------\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    elenco_imprimir(&s->elenco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void painel_relatorio_terminais(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao painel_relatorio_terminais. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n=== Relatorio por Terminal =========================\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        terminal_imprimir_detalhe(cur->terminal);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Encontra o terminal que atendeu mais clientes. */
static const Terminal_t *terminal_mais_atendimentos(const Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t     *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    const Terminal_t *melhor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!s) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (!cur->terminal) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (!melhor || cur->terminal->total_atendidos > melhor->total_atendidos) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            melhor = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Terminal com mais artigos processados. */
static const Terminal_t *terminal_mais_artigos(const Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t     *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    const Terminal_t *melhor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!s) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (!cur->terminal) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (!melhor || cur->terminal->total_artigos > melhor->total_artigos) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            melhor = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Operador menos activo (menor numero de atendimentos). */
static const Terminal_t *terminal_menos_activo(const Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t     *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    const Terminal_t *pior = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!s) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (!cur->terminal) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        /* exclui terminais que nunca foram abertos para nao
           penalizar quem nem chegou a trabalhar */
        if (cur->terminal->total_atendidos == 0  /* linha: Testa a condicao antes de executar o bloco associado. */
            && cur->terminal->estado == EST_TERMINAL_FECHADO) continue;  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        if (!pior || cur->terminal->total_atendidos < pior->total_atendidos) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            pior = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return pior;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void painel_relatorio_global(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao painel_relatorio_global. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    const Terminal_t *t1, *t2, *t3;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    double            media_espera = 0.0, media_servico = 0.0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    size_t            mem_uso, mem_pico, mem_des;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    if (s->total_clientes_atendidos > 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        media_espera  = (double) s->soma_tempos_espera  / (double) s->total_clientes_atendidos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        media_servico = (double) s->soma_tempos_servico / (double) s->total_clientes_atendidos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    mem_uso  = vigia_em_uso();  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    mem_pico = vigia_pico();  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    mem_des  = supermercado_memoria_desperdicada(s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    printf("\n=== Estatisticas Finais ============================\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" tick final                : %d\n", s->tick_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" terminais (total/aberto)  : %d / %d\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           elenco_total(&s->elenco), elenco_total_abertos(&s->elenco));  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf(" clientes atendidos        : %d\n", s->total_clientes_atendidos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" clientes ainda no sistema : %d\n", s->total_clientes_no_sistema);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" tempo medio de espera     : %.2f s\n", media_espera);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" tempo medio de servico    : %.2f s\n", media_servico);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" ofertas concedidas        : %d\n", s->total_ofertas_concedidas);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" prejuizo total acumulado  : %.2f EUR\n", s->prejuizo_acumulado);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" ----- memoria -----\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" em uso (real)             : %lu bytes\n", (unsigned long) mem_uso);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" pico                      : %lu bytes\n", (unsigned long) mem_pico);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" desperdicada (calculada)  : %lu bytes\n", (unsigned long) mem_des);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" blocos vivos              : %lu\n",       (unsigned long) vigia_blocos_vivos());  /* linha: Mostra texto formatado no ecra para o utilizador. */

    t1 = terminal_mais_atendimentos(s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t2 = terminal_mais_artigos(s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t3 = terminal_menos_activo(s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    printf(" ----- distincoes -----\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    if (t1) printf(" mais clientes atendidos   : terminal #%d (%s) — %d clientes\n",  /* linha: Testa a condicao antes de executar o bloco associado. */
                   t1->id, t1->nome_operador, t1->total_atendidos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    if (t2) printf(" mais artigos processados  : terminal #%d (%s) — %ld artigos\n",  /* linha: Testa a condicao antes de executar o bloco associado. */
                   t2->id, t2->nome_operador, t2->total_artigos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    if (t3) printf(" operador menos activo     : terminal #%d (%s) — %d clientes\n",  /* linha: Testa a condicao antes de executar o bloco associado. */
                   t3->id, t3->nome_operador, t3->total_atendidos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    printf("====================================================\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int painel_diagnostico(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao painel_diagnostico. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           ok = 1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int           pessoas = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (!cur->terminal) { ok = 0; printf(" [DIAG] no sem terminal\n"); continue; }  /* linha: Testa a condicao antes de executar o bloco associado. */
        pessoas += terminal_total_pessoas(cur->terminal);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (cur->terminal->em_atendimento  /* linha: Testa a condicao antes de executar o bloco associado. */
            && cur->terminal->em_atendimento->id_terminal_actual != cur->terminal->id)  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
            printf(" [DIAG] ficha em atendimento com id_terminal errado (term=%d)\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
                   cur->terminal->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            ok = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    if (s->total_clientes_no_sistema < 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf(" [DIAG] total_clientes_no_sistema negativo\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        ok = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    printf("\n>> Diagnostico: %s (pessoas vivas no sistema=%d)\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           ok ? "OK" : "PROBLEMAS", pessoas);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    return ok;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int painel_exportar_resumo(const Supermercado_t *s, const char *caminho)  /* linha: Inicia ou continua a definicao da funcao painel_exportar_resumo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FILE *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s || !caminho) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    f = fopen(caminho, "w");  /* linha: Abre um ficheiro no modo indicado. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    fprintf(f, "Resumo final da simulacao\n");  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "tick_final;%d\n", s->tick_actual);  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "atendidos;%d\n", s->total_clientes_atendidos);  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "no_sistema;%d\n", s->total_clientes_no_sistema);  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "ofertas;%d\n", s->total_ofertas_concedidas);  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "prejuizo;%.2f\n", s->prejuizo_acumulado);  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "memoria_em_uso;%lu\n", (unsigned long) vigia_em_uso());  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "memoria_pico;%lu\n",   (unsigned long) vigia_pico());  /* linha: Escreve texto formatado no ficheiro indicado. */
    fprintf(f, "memoria_desperdicada;%lu\n",  /* linha: Escreve texto formatado no ficheiro indicado. */
            (unsigned long) supermercado_memoria_desperdicada(s));  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    fclose(f);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
