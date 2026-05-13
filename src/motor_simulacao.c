#include "motor_simulacao.h"  /* linha: Importa as declaracoes locais de motor_simulacao.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "catalogo_artigos.h"  /* linha: Importa as declaracoes locais de catalogo_artigos.h usadas neste modulo. */
#include "cronista.h"  /* linha: Importa as declaracoes locais de cronista.h usadas neste modulo. */
#include "configuracao.h"  /* linha: Importa as declaracoes locais de configuracao.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdarg.h>  /* linha: Importa a biblioteca padrao stdarg.h necessaria neste modulo. */
#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

/* ----------------------------------------------------------
 * Helpers locais
 * ---------------------------------------------------------- */

static void registar_evento_motor(Supermercado_t *s,  /* linha: Inicia ou continua a definicao da funcao registar_evento_motor. */
                                  const char *tipo,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                  const char *fmt, ...)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char    buffer[256];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    va_list args;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    va_start(args, fmt);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    vsnprintf(buffer, sizeof(buffer), fmt, args);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    va_end(args);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    cronista_evento(&s->cronista, s->tick_actual, tipo, buffer);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Agenda a oferta usando sempre o prazo absoluto da entrada do
   cliente no sistema. Ao mover uma ficha de fila, o relogio de
   espera nao deve voltar a zero. */
static void agendar_oferta_no_prazo_original(Supermercado_t *s,  /* linha: Inicia ou continua a definicao da funcao agendar_oferta_no_prazo_original. */
                                             Terminal_t *t,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                             FichaCliente_t *f)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int instante_oferta;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s || !f || f->recebeu_oferta) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (f->estado != EST_FICHA_AGUARDA) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    instante_oferta = f->instante_chegada + s->config.max_espera;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (instante_oferta < s->tick_actual) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        instante_oferta = s->tick_actual;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    calendario_agendar(&s->calendario,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                       instante_oferta,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                       EVT_OFERTA_DEVIDA,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                       t,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                       f);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Escolhe a melhor caixa aberta, excluindo uma origem que esta
   a ser esvaziada. A metrica continua a ser o menor tempo de
   espera estimado, nao apenas o menor tamanho de fila. */
static Terminal_t *melhor_aberto_excluindo(const Supermercado_t *s,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                           const Terminal_t *excluir)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t   *melhor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    long          melhor_tempo = -1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        Terminal_t *t = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        long tempo;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

        if (!t || t == excluir || t->estado != EST_TERMINAL_ABERTO) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        tempo = terminal_tempo_estimado_total(t);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!melhor || tempo < melhor_tempo) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            melhor = t;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            melhor_tempo = tempo;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Calcula a media de clientes em espera por caixa aberta, tal
   como o enunciado pede para a decisao automatica de abrir ou
   fechar caixas. Clientes ja em atendimento nao contam como
   espera na fila. */
static double media_clientes_filas_abertas(const Supermercado_t *s,  /* linha: Inicia ou continua a definicao da funcao media_clientes_filas_abertas. */
                                           int *abertas_out)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           abertas = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int           clientes = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (abertas_out) *abertas_out = 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
        return 0.0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        Terminal_t *t = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (t && t->estado == EST_TERMINAL_ABERTO) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            abertas++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
            clientes += fila_tamanho(&t->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    if (abertas_out) *abertas_out = abertas;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return (abertas > 0) ? (double) clientes / (double) abertas : 0.0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Localiza a caixa aberta com menos pessoas no total. Esta e a
   escolhida para drenar quando a media das filas fica abaixo
   de MIN_FILA. */
static Terminal_t *terminal_aberto_menos_pessoas(const Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t   *melhor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    int           melhor_total = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        Terminal_t *t = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        int total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

        if (!t || t->estado != EST_TERMINAL_ABERTO) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        total = terminal_total_pessoas(t);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!melhor || total < melhor_total) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            melhor = t;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            melhor_total = total;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    return melhor;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */


/* Quando uma ficha termina o atendimento. Actualiza o terminal,
   actualiza o estado da ficha e as metricas globais. */
static void finalizar_atendimento(Supermercado_t *s, Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao finalizar_atendimento. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    long            tempo_servico;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s || !t || !t->em_atendimento) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    f                  = t->em_atendimento;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->instante_saida  = s->tick_actual;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->estado          = EST_FICHA_PROCESSADA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->versao++;  /* invalida quaisquer eventos pendentes para esta ficha */  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

    tempo_servico = f->instante_saida - f->instante_inicio_servico;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (tempo_servico < 0) tempo_servico = 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    t->total_atendidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    t->total_artigos       += f->quantidade_artigos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t->tempo_total_servico += tempo_servico;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    s->total_clientes_atendidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    s->soma_tempos_servico += tempo_servico;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (s->total_clientes_no_sistema > 0) s->total_clientes_no_sistema--;  /* linha: Testa a condicao antes de executar o bloco associado. */

    registar_evento_motor(s, "ATEND_FIM",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        "ficha=%d nome=%s terminal=%d tempo_servico=%lds",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        f->id, f->nome, t->id, tempo_servico);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */

    terminal_registar_atendido(t, f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    t->em_atendimento = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->id_terminal_actual = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Tenta puxar uma ficha da fila do terminal @t para
   atendimento. Devolve 1 se comecou a atender. */
static int iniciar_proximo_atendimento(Supermercado_t *s, Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao iniciar_proximo_atendimento. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    long            espera;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s || !t) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (t->estado == EST_TERMINAL_FECHADO) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (t->em_atendimento) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (fila_tamanho(&t->fila) == 0) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    f = fila_extrair_frente(&t->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    t->em_atendimento          = f;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->estado                  = EST_FICHA_ATENDIMENTO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->instante_inicio_servico = s->tick_actual;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->id_terminal_actual      = t->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->versao++;  /* mudou de estado, invalida ofertas pendentes */  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

    espera = f->instante_inicio_servico - f->instante_chegada;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (espera < 0) espera = 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    t->tempo_total_espera += espera;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->soma_tempos_espera += espera;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    /* agendar fim do atendimento */
    calendario_agendar(&s->calendario,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                       s->tick_actual + f->tempo_servico_estimado,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                       EVT_FIM_ATENDIMENTO,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                       t, f);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    registar_evento_motor(s, "ATEND_INICIO",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        "ficha=%d nome=%s terminal=%d tempo_estimado=%ds espera=%lds",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        f->id, f->nome, t->id, f->tempo_servico_estimado, espera);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Processa eventos que ja estao "vencidos" (instante <= tick_actual). */
static void processar_eventos_vencidos(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao processar_eventos_vencidos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Evento_t *e;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int       prox;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    while ((prox = calendario_proximo_instante(&s->calendario)) >= 0  /* linha: Repete ou controla o bloco com base na condicao indicada. */
           && prox <= s->tick_actual)  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
        e = calendario_extrair_proximo(&s->calendario);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!e) break;  /* linha: Testa a condicao antes de executar o bloco associado. */

        /* lazy deletion: ficha mudou de estado, evento obsoleto */
        if (e->ficha && e->ficha->versao != e->versao_ficha) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            calendario_libertar_evento(e);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */

        switch (e->tipo) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
            case EVT_FIM_ATENDIMENTO:  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (e->terminal && e->terminal->em_atendimento == e->ficha) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                    finalizar_atendimento(s, e->terminal);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                }  /* linha: Fecha o bloco de instrucoes atual. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */

            case EVT_OFERTA_DEVIDA:  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (e->ficha && e->ficha->estado == EST_FICHA_AGUARDA  /* linha: Testa a condicao antes de executar o bloco associado. */
                    && !e->ficha->recebeu_oferta)  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
                    ficha_marcar_oferta(e->ficha);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                    s->total_ofertas_concedidas++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
                    s->prejuizo_acumulado += e->ficha->valor_oferta;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                    if (e->terminal) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                        e->terminal->total_ofertas++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
                        e->terminal->prejuizo_total += e->ficha->valor_oferta;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                    }  /* linha: Fecha o bloco de instrucoes atual. */
                    registar_evento_motor(s, "OFERTA",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                        "ficha=%d nome=%s artigo=%s valor=%.2f",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                        e->ficha->id, e->ficha->nome,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                        e->ficha->cesto[e->ficha->indice_oferta].designacao,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                        e->ficha->valor_oferta);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                }  /* linha: Fecha o bloco de instrucoes atual. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */

            case EVT_VERIFICAR_BALANCO:  /* linha: Define um caso especifico dentro da escolha multipla. */
                motor_verificar_balanco(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        calendario_libertar_evento(e);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Para cada terminal aberto/drenando sem ficha em atendimento
   mas com fila, puxa o proximo. */
static void puxar_proximos_atendimentos(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao puxar_proximos_atendimentos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal) iniciar_proximo_atendimento(s, cur->terminal);  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Para cada terminal em DRENANDO com fila vazia e sem
   atendimento corrente, fecha-o de vez. */
static void fechar_terminais_drenados(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao fechar_terminais_drenados. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t   *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        t = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (t && t->estado == EST_TERMINAL_DRENANDO  /* linha: Testa a condicao antes de executar o bloco associado. */
              && t->em_atendimento == NULL  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
              && fila_tamanho(&t->fila) == 0)  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
            terminal_fechar(t);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            registar_evento_motor(s, "FECHO_AUTO",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                "terminal=%d (drenagem completa)", t->id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * API publica
 * ---------------------------------------------------------- */

void motor_passo(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao motor_passo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    s->tick_actual++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

    processar_eventos_vencidos (s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    puxar_proximos_atendimentos(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    fechar_terminais_drenados  (s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    if (s->config.tick_verificacao > 0  /* linha: Testa a condicao antes de executar o bloco associado. */
        && (s->tick_actual % s->config.tick_verificacao) == 0)  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
        motor_verificar_balanco(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    cronista_snapshot(&s->cronista, s);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void motor_correr_n_ticks(Supermercado_t *s, int n)  /* linha: Inicia ou continua a definicao da funcao motor_correr_n_ticks. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!s || n <= 0) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (i = 0; i < n; i++) motor_passo(s);  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void motor_correr_ate_estabilizar(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao motor_correr_ate_estabilizar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int           seguranca = 1000000;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           pendentes;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    while (seguranca-- > 0) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        pendentes = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
            if (!cur->terminal) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
            pendentes += fila_tamanho(&cur->terminal->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (cur->terminal->em_atendimento) pendentes++;  /* linha: Testa a condicao antes de executar o bloco associado. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        if (pendentes == 0) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        motor_passo(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * Verificacao automatica de balanco
 * ---------------------------------------------------------- */

void motor_verificar_balanco(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao motor_verificar_balanco. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t   *fechado;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           abertos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double        media;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    media = media_clientes_filas_abertas(s, &abertos);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (abertos <= 0) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* Abertura automatica pedida no enunciado: a decisao usa
       a media de clientes em espera por caixa aberta. */
    if (media > (double) s->config.max_fila) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        fechado = elenco_primeiro_fechado(&s->elenco);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (fechado) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            terminal_abrir(fechado);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            registar_evento_motor(s, "ABERTURA_AUTO",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                "terminal=%d aberto por media=%.2f", fechado->id, media);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            motor_redistribuir(s, -1);  /* rebalanceia globalmente */  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            return;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    /* Fecho automatico: quando a media baixa, drena-se apenas
       a caixa aberta com menos pessoas, deixando-a terminar o
       trabalho que ainda tem. */
    media = media_clientes_filas_abertas(s, &abertos);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (abertos > 1 && media < (double) s->config.min_fila) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        Terminal_t *menos = terminal_aberto_menos_pessoas(s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (menos) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            terminal_drenar(menos);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            registar_evento_motor(s, "DRENAR_AUTO",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                "terminal=%d drenado por media=%.2f", menos->id, media);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * Operacoes manuais (vindas do menu)
 * ---------------------------------------------------------- */

int motor_abrir_terminal_manual(Supermercado_t *s, int id_ou_zero)  /* linha: Inicia ou continua a definicao da funcao motor_abrir_terminal_manual. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (id_ou_zero <= 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        t = elenco_primeiro_fechado(&s->elenco);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
        t = elenco_obter_por_id(&s->elenco, id_ou_zero);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (t && t->estado != EST_TERMINAL_FECHADO) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    if (!t) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    terminal_abrir(t);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    registar_evento_motor(s, "ABERTURA_MANUAL", "terminal=%d", t->id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    return t->id;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int motor_drenar_terminal_manual(Supermercado_t *s, int id)  /* linha: Inicia ou continua a definicao da funcao motor_drenar_terminal_manual. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s || id <= 0) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    t = elenco_obter_por_id(&s->elenco, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!t || t->estado != EST_TERMINAL_ABERTO) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    terminal_drenar(t);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    registar_evento_motor(s, "DRENAR_MANUAL", "terminal=%d", id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int motor_mover_ficha(Supermercado_t *s, int id_ficha, int id_destino)  /* linha: Inicia ou continua a definicao da funcao motor_mover_ficha. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t   *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t     *origem  = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    Terminal_t     *destino;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *f       = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (!s || id_ficha <= 0 || id_destino <= 0) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* localizar a ficha em todas as filas */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (!cur->terminal) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        f = fila_remover_por_id(&cur->terminal->fila, id_ficha);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (f) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            origem = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    destino = elenco_obter_por_id(&s->elenco, id_destino);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!destino || destino->estado != EST_TERMINAL_ABERTO) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        /* nao existe destino valido — devolve a ficha a origem */
        fila_inserir_atras(&origem->fila, f);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    if (!fila_inserir_atras(&destino->fila, f)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        fila_inserir_atras(&origem->fila, f);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    f->id_terminal_actual = destino->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->versao++;  /* invalida ofertas pendentes anteriores */  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

    agendar_oferta_no_prazo_original(s, destino, f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    registar_evento_motor(s, "MOVE_FICHA",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        "ficha=%d nome=%s origem=%d destino=%d",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        f->id, f->nome, origem->id, destino->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int motor_redistribuir(Supermercado_t *s, int id_origem)  /* linha: Inicia ou continua a definicao da funcao motor_redistribuir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t   *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t     *origem;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t     *destino;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             movidos = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* id_origem = -1 significa "redistribui todas as filas
       sobrelotadas" — usado pela verificacao automatica. */
    if (id_origem < 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
            origem = cur->terminal;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (!origem || origem->estado != EST_TERMINAL_ABERTO) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
            while (fila_tamanho(&origem->fila) > s->config.max_fila) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
                destino = melhor_aberto_excluindo(s, origem);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                if (!destino) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
                /* tirar o ultimo da fila do origem */
                f = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
                    NoFila_t *no, *ant;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
                    no = origem->fila.frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                    ant = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                    while (no && no->seguinte) { ant = no; no = no->seguinte; }  /* linha: Repete ou controla o bloco com base na condicao indicada. */
                    if (no) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                        f = no->ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                        if (ant) ant->seguinte = NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
                        else     origem->fila.frente = NULL;  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
                        origem->fila.tras = ant;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                        origem->fila.total--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
                        vigia_devolver(no);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                    }  /* linha: Fecha o bloco de instrucoes atual. */
                }  /* linha: Fecha o bloco de instrucoes atual. */
                if (!f) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
                if (!fila_inserir_atras(&destino->fila, f)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                    fila_inserir_atras(&origem->fila, f);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                    break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
                }  /* linha: Fecha o bloco de instrucoes atual. */
                f->id_terminal_actual = destino->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                f->versao++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
                agendar_oferta_no_prazo_original(s, destino, f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                registar_evento_motor(s, "REDISTRIB",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                    "ficha=%d origem=%d destino=%d",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                    f->id, origem->id, destino->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                movidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        return movidos;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    origem = elenco_obter_por_id(&s->elenco, id_origem);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!origem) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    while (fila_tamanho(&origem->fila) > 0) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        destino = melhor_aberto_excluindo(s, origem);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!destino) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        f = fila_extrair_frente(&origem->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!f) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (!fila_inserir_atras(&destino->fila, f)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            fila_inserir_frente(&origem->fila, f);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        f->id_terminal_actual = destino->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        f->versao++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        agendar_oferta_no_prazo_original(s, destino, f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        registar_evento_motor(s, "REDISTRIB",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            "ficha=%d origem=%d destino=%d",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            f->id, id_origem, destino->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        movidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return movidos;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int motor_fechar_redistribuindo(Supermercado_t *s, int id_terminal)  /* linha: Inicia ou continua a definicao da funcao motor_fechar_redistribuindo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t     *origem;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t     *destino;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             movidos = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s || id_terminal <= 0) return -1;  /* linha: Testa a condicao antes de executar o bloco associado. */

    origem = elenco_obter_por_id(&s->elenco, id_terminal);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!origem || origem->estado == EST_TERMINAL_FECHADO) return -1;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* Fecho imediato pedido pelo enunciado: quem esta em fila
       sai para outras caixas abertas; quem ja esta a ser
       atendido termina antes de a caixa fechar. */
    while (fila_tamanho(&origem->fila) > 0) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        destino = melhor_aberto_excluindo(s, origem);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!destino) return -1;  /* linha: Testa a condicao antes de executar o bloco associado. */

        f = fila_extrair_frente(&origem->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!f) break;  /* linha: Testa a condicao antes de executar o bloco associado. */

        if (!fila_inserir_atras(&destino->fila, f)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            fila_inserir_frente(&origem->fila, f);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            return -1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
        }  /* linha: Fecha o bloco de instrucoes atual. */

        f->id_terminal_actual = destino->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        f->versao++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        agendar_oferta_no_prazo_original(s, destino, f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

        registar_evento_motor(s, "REDISTRIB",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            "ficha=%d origem=%d destino=%d (fecho imediato)",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            f->id, origem->id, destino->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        movidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    if (origem->em_atendimento) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        terminal_drenar(origem);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        registar_evento_motor(s, "DRENAR_MANUAL",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            "terminal=%d apos redistribuicao (%d movidos)",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            origem->id, movidos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
        terminal_fechar(origem);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        registar_evento_motor(s, "FECHO_MANUAL",  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            "terminal=%d fechado apos redistribuicao (%d movidos)",  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            origem->id, movidos);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    return movidos;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
