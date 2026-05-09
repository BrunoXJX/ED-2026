#include "motor_simulacao.h"
#include "supermercado.h"
#include "elenco_terminais.h"
#include "terminal_caixa.h"
#include "fila_atendimento.h"
#include "calendario_eventos.h"
#include "indice_pesquisa.h"
#include "ficha_cliente.h"
#include "catalogo_artigos.h"
#include "cronista.h"
#include "configuracao.h"
#include "vigilante_memoria.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ----------------------------------------------------------
 * Helpers locais
 * ---------------------------------------------------------- */

static void registar_evento_motor(Supermercado_t *s,
                                  const char *tipo,
                                  const char *fmt, ...)
{
    char    buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    cronista_evento(&s->cronista, s->tick_actual, tipo, buffer);
}

/* Agenda a oferta usando sempre o prazo absoluto da entrada do
   cliente no sistema. Ao mover uma ficha de fila, o relogio de
   espera nao deve voltar a zero. */
static void agendar_oferta_no_prazo_original(Supermercado_t *s,
                                             Terminal_t *t,
                                             FichaCliente_t *f)
{
    int instante_oferta;

    if (!s || !f || f->recebeu_oferta) return;
    if (f->estado != EST_FICHA_AGUARDA) return;

    instante_oferta = f->instante_chegada + s->config.max_espera;
    if (instante_oferta < s->tick_actual) {
        instante_oferta = s->tick_actual;
    }

    calendario_agendar(&s->calendario,
                       instante_oferta,
                       EVT_OFERTA_DEVIDA,
                       t,
                       f);
}

/* Escolhe a melhor caixa aberta, excluindo uma origem que esta
   a ser esvaziada. A metrica continua a ser o menor tempo de
   espera estimado, nao apenas o menor tamanho de fila. */
static Terminal_t *melhor_aberto_excluindo(const Supermercado_t *s,
                                           const Terminal_t *excluir)
{
    NoTerminal_t *cur;
    Terminal_t   *melhor = NULL;
    long          melhor_tempo = -1;

    if (!s) return NULL;

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        Terminal_t *t = cur->terminal;
        long tempo;

        if (!t || t == excluir || t->estado != EST_TERMINAL_ABERTO) continue;

        tempo = terminal_tempo_estimado_total(t);
        if (!melhor || tempo < melhor_tempo) {
            melhor = t;
            melhor_tempo = tempo;
        }
    }

    return melhor;
}

/* Calcula a media de clientes em espera por caixa aberta, tal
   como o enunciado pede para a decisao automatica de abrir ou
   fechar caixas. Clientes ja em atendimento nao contam como
   espera na fila. */
static double media_clientes_filas_abertas(const Supermercado_t *s,
                                           int *abertas_out)
{
    NoTerminal_t *cur;
    int           abertas = 0;
    int           clientes = 0;

    if (!s) {
        if (abertas_out) *abertas_out = 0;
        return 0.0;
    }

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        Terminal_t *t = cur->terminal;
        if (t && t->estado == EST_TERMINAL_ABERTO) {
            abertas++;
            clientes += fila_tamanho(&t->fila);
        }
    }

    if (abertas_out) *abertas_out = abertas;
    return (abertas > 0) ? (double) clientes / (double) abertas : 0.0;
}

/* Localiza a caixa aberta com menos pessoas no total. Esta e a
   escolhida para drenar quando a media das filas fica abaixo
   de MIN_FILA. */
static Terminal_t *terminal_aberto_menos_pessoas(const Supermercado_t *s)
{
    NoTerminal_t *cur;
    Terminal_t   *melhor = NULL;
    int           melhor_total = 0;

    if (!s) return NULL;

    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        Terminal_t *t = cur->terminal;
        int total;

        if (!t || t->estado != EST_TERMINAL_ABERTO) continue;

        total = terminal_total_pessoas(t);
        if (!melhor || total < melhor_total) {
            melhor = t;
            melhor_total = total;
        }
    }

    return melhor;
}


/* Quando uma ficha termina o atendimento. Actualiza o terminal,
   actualiza o estado da ficha e as metricas globais. */
static void finalizar_atendimento(Supermercado_t *s, Terminal_t *t)
{
    FichaCliente_t *f;
    long            tempo_servico;

    if (!s || !t || !t->em_atendimento) return;

    f                  = t->em_atendimento;
    f->instante_saida  = s->tick_actual;
    f->estado          = EST_FICHA_PROCESSADA;
    f->versao++;  /* invalida quaisquer eventos pendentes para esta ficha */

    tempo_servico = f->instante_saida - f->instante_inicio_servico;
    if (tempo_servico < 0) tempo_servico = 0;

    t->total_atendidos++;
    t->total_artigos       += f->quantidade_artigos;
    t->tempo_total_servico += tempo_servico;

    s->total_clientes_atendidos++;
    s->soma_tempos_servico += tempo_servico;
    if (s->total_clientes_no_sistema > 0) s->total_clientes_no_sistema--;

    registar_evento_motor(s, "ATEND_FIM",
        "ficha=%d nome=%s terminal=%d tempo_servico=%lds",
        f->id, f->nome, t->id, tempo_servico);

    terminal_registar_atendido(t, f);

    t->em_atendimento = NULL;
    f->id_terminal_actual = -1;
}

/* Tenta puxar uma ficha da fila do terminal @t para
   atendimento. Devolve 1 se comecou a atender. */
static int iniciar_proximo_atendimento(Supermercado_t *s, Terminal_t *t)
{
    FichaCliente_t *f;
    long            espera;

    if (!s || !t) return 0;
    if (t->estado == EST_TERMINAL_FECHADO) return 0;
    if (t->em_atendimento) return 0;
    if (fila_tamanho(&t->fila) == 0) return 0;

    f = fila_extrair_frente(&t->fila);
    if (!f) return 0;

    t->em_atendimento          = f;
    f->estado                  = EST_FICHA_ATENDIMENTO;
    f->instante_inicio_servico = s->tick_actual;
    f->id_terminal_actual      = t->id;
    f->versao++;  /* mudou de estado, invalida ofertas pendentes */

    espera = f->instante_inicio_servico - f->instante_chegada;
    if (espera < 0) espera = 0;

    t->tempo_total_espera += espera;
    s->soma_tempos_espera += espera;

    /* agendar fim do atendimento */
    calendario_agendar(&s->calendario,
                       s->tick_actual + f->tempo_servico_estimado,
                       EVT_FIM_ATENDIMENTO,
                       t, f);

    registar_evento_motor(s, "ATEND_INICIO",
        "ficha=%d nome=%s terminal=%d tempo_estimado=%ds espera=%lds",
        f->id, f->nome, t->id, f->tempo_servico_estimado, espera);
    return 1;
}

/* Processa eventos que ja estao "vencidos" (instante <= tick_actual). */
static void processar_eventos_vencidos(Supermercado_t *s)
{
    Evento_t *e;
    int       prox;

    if (!s) return;

    while ((prox = calendario_proximo_instante(&s->calendario)) >= 0
           && prox <= s->tick_actual)
    {
        e = calendario_extrair_proximo(&s->calendario);
        if (!e) break;

        /* lazy deletion: ficha mudou de estado, evento obsoleto */
        if (e->ficha && e->ficha->versao != e->versao_ficha) {
            calendario_libertar_evento(e);
            continue;
        }

        switch (e->tipo) {
            case EVT_FIM_ATENDIMENTO:
                if (e->terminal && e->terminal->em_atendimento == e->ficha) {
                    finalizar_atendimento(s, e->terminal);
                }
                break;

            case EVT_OFERTA_DEVIDA:
                if (e->ficha && e->ficha->estado == EST_FICHA_AGUARDA
                    && !e->ficha->recebeu_oferta)
                {
                    ficha_marcar_oferta(e->ficha);
                    s->total_ofertas_concedidas++;
                    s->prejuizo_acumulado += e->ficha->valor_oferta;
                    if (e->terminal) {
                        e->terminal->total_ofertas++;
                        e->terminal->prejuizo_total += e->ficha->valor_oferta;
                    }
                    registar_evento_motor(s, "OFERTA",
                        "ficha=%d nome=%s artigo=%s valor=%.2f",
                        e->ficha->id, e->ficha->nome,
                        e->ficha->cesto[e->ficha->indice_oferta].designacao,
                        e->ficha->valor_oferta);
                }
                break;

            case EVT_VERIFICAR_BALANCO:
                motor_verificar_balanco(s);
                break;
        }
        calendario_libertar_evento(e);
    }
}

/* Para cada terminal aberto/drenando sem ficha em atendimento
   mas com fila, puxa o proximo. */
static void puxar_proximos_atendimentos(Supermercado_t *s)
{
    NoTerminal_t *cur;
    if (!s) return;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal) iniciar_proximo_atendimento(s, cur->terminal);
    }
}

/* Para cada terminal em DRENANDO com fila vazia e sem
   atendimento corrente, fecha-o de vez. */
static void fechar_terminais_drenados(Supermercado_t *s)
{
    NoTerminal_t *cur;
    Terminal_t   *t;
    if (!s) return;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        t = cur->terminal;
        if (t && t->estado == EST_TERMINAL_DRENANDO
              && t->em_atendimento == NULL
              && fila_tamanho(&t->fila) == 0)
        {
            terminal_fechar(t);
            registar_evento_motor(s, "FECHO_AUTO",
                "terminal=%d (drenagem completa)", t->id);
        }
    }
}

/* ----------------------------------------------------------
 * API publica
 * ---------------------------------------------------------- */

void motor_passo(Supermercado_t *s)
{
    if (!s) return;

    s->tick_actual++;

    processar_eventos_vencidos (s);
    puxar_proximos_atendimentos(s);
    fechar_terminais_drenados  (s);

    if (s->config.tick_verificacao > 0
        && (s->tick_actual % s->config.tick_verificacao) == 0)
    {
        motor_verificar_balanco(s);
    }

    cronista_snapshot(&s->cronista, s);
}

void motor_correr_n_ticks(Supermercado_t *s, int n)
{
    int i;
    if (!s || n <= 0) return;
    for (i = 0; i < n; i++) motor_passo(s);
}

void motor_correr_ate_estabilizar(Supermercado_t *s)
{
    int           seguranca = 1000000;
    NoTerminal_t *cur;
    int           pendentes;

    if (!s) return;

    while (seguranca-- > 0) {
        pendentes = 0;
        for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
            if (!cur->terminal) continue;
            pendentes += fila_tamanho(&cur->terminal->fila);
            if (cur->terminal->em_atendimento) pendentes++;
        }
        if (pendentes == 0) break;
        motor_passo(s);
    }
}

/* ----------------------------------------------------------
 * Verificacao automatica de balanco
 * ---------------------------------------------------------- */

void motor_verificar_balanco(Supermercado_t *s)
{
    Terminal_t   *fechado;
    int           abertos;
    double        media;

    if (!s) return;

    media = media_clientes_filas_abertas(s, &abertos);
    if (abertos <= 0) return;

    /* Abertura automatica pedida no enunciado: a decisao usa
       a media de clientes em espera por caixa aberta. */
    if (media > (double) s->config.max_fila) {
        fechado = elenco_primeiro_fechado(&s->elenco);
        if (fechado) {
            terminal_abrir(fechado);
            registar_evento_motor(s, "ABERTURA_AUTO",
                "terminal=%d aberto por media=%.2f", fechado->id, media);
            motor_redistribuir(s, -1);  /* rebalanceia globalmente */
            return;
        }
    }

    /* Fecho automatico: quando a media baixa, drena-se apenas
       a caixa aberta com menos pessoas, deixando-a terminar o
       trabalho que ainda tem. */
    media = media_clientes_filas_abertas(s, &abertos);
    if (abertos > 1 && media < (double) s->config.min_fila) {
        Terminal_t *menos = terminal_aberto_menos_pessoas(s);
        if (menos) {
            terminal_drenar(menos);
            registar_evento_motor(s, "DRENAR_AUTO",
                "terminal=%d drenado por media=%.2f", menos->id, media);
        }
    }
}

/* ----------------------------------------------------------
 * Operacoes manuais (vindas do menu)
 * ---------------------------------------------------------- */

int motor_abrir_terminal_manual(Supermercado_t *s, int id_ou_zero)
{
    Terminal_t *t;
    if (!s) return 0;
    if (id_ou_zero <= 0) {
        t = elenco_primeiro_fechado(&s->elenco);
    } else {
        t = elenco_obter_por_id(&s->elenco, id_ou_zero);
        if (t && t->estado != EST_TERMINAL_FECHADO) return 0;
    }
    if (!t) return 0;
    terminal_abrir(t);
    registar_evento_motor(s, "ABERTURA_MANUAL", "terminal=%d", t->id);
    return t->id;
}

int motor_drenar_terminal_manual(Supermercado_t *s, int id)
{
    Terminal_t *t;
    if (!s || id <= 0) return 0;
    t = elenco_obter_por_id(&s->elenco, id);
    if (!t || t->estado != EST_TERMINAL_ABERTO) return 0;
    terminal_drenar(t);
    registar_evento_motor(s, "DRENAR_MANUAL", "terminal=%d", id);
    return 1;
}

int motor_mover_ficha(Supermercado_t *s, int id_ficha, int id_destino)
{
    NoTerminal_t   *cur;
    Terminal_t     *origem  = NULL;
    Terminal_t     *destino;
    FichaCliente_t *f       = NULL;

    if (!s || id_ficha <= 0 || id_destino <= 0) return 0;

    /* localizar a ficha em todas as filas */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (!cur->terminal) continue;
        f = fila_remover_por_id(&cur->terminal->fila, id_ficha);
        if (f) {
            origem = cur->terminal;
            break;
        }
    }
    if (!f) return 0;

    destino = elenco_obter_por_id(&s->elenco, id_destino);
    if (!destino || destino->estado != EST_TERMINAL_ABERTO) {
        /* nao existe destino valido — devolve a ficha a origem */
        fila_inserir_atras(&origem->fila, f);
        return 0;
    }

    if (!fila_inserir_atras(&destino->fila, f)) {
        fila_inserir_atras(&origem->fila, f);
        return 0;
    }
    f->id_terminal_actual = destino->id;
    f->versao++;  /* invalida ofertas pendentes anteriores */

    agendar_oferta_no_prazo_original(s, destino, f);
    registar_evento_motor(s, "MOVE_FICHA",
        "ficha=%d nome=%s origem=%d destino=%d",
        f->id, f->nome, origem->id, destino->id);
    return 1;
}

int motor_redistribuir(Supermercado_t *s, int id_origem)
{
    NoTerminal_t   *cur;
    Terminal_t     *origem;
    Terminal_t     *destino;
    FichaCliente_t *f;
    int             movidos = 0;

    if (!s) return 0;

    /* id_origem = -1 significa "redistribui todas as filas
       sobrelotadas" — usado pela verificacao automatica. */
    if (id_origem < 0) {
        for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
            origem = cur->terminal;
            if (!origem || origem->estado != EST_TERMINAL_ABERTO) continue;
            while (fila_tamanho(&origem->fila) > s->config.max_fila) {
                destino = melhor_aberto_excluindo(s, origem);
                if (!destino) break;
                /* tirar o ultimo da fila do origem */
                f = NULL;
                {
                    NoFila_t *no, *ant;
                    no = origem->fila.frente;
                    ant = NULL;
                    while (no && no->seguinte) { ant = no; no = no->seguinte; }
                    if (no) {
                        f = no->ficha;
                        if (ant) ant->seguinte = NULL;
                        else     origem->fila.frente = NULL;
                        origem->fila.tras = ant;
                        origem->fila.total--;
                        vigia_devolver(no);
                    }
                }
                if (!f) break;
                if (!fila_inserir_atras(&destino->fila, f)) {
                    fila_inserir_atras(&origem->fila, f);
                    break;
                }
                f->id_terminal_actual = destino->id;
                f->versao++;
                agendar_oferta_no_prazo_original(s, destino, f);
                registar_evento_motor(s, "REDISTRIB",
                    "ficha=%d origem=%d destino=%d",
                    f->id, origem->id, destino->id);
                movidos++;
            }
        }
        return movidos;
    }

    origem = elenco_obter_por_id(&s->elenco, id_origem);
    if (!origem) return 0;

    while (fila_tamanho(&origem->fila) > 0) {
        destino = melhor_aberto_excluindo(s, origem);
        if (!destino) break;
        f = fila_extrair_frente(&origem->fila);
        if (!f) break;
        if (!fila_inserir_atras(&destino->fila, f)) {
            fila_inserir_frente(&origem->fila, f);
            break;
        }
        f->id_terminal_actual = destino->id;
        f->versao++;
        agendar_oferta_no_prazo_original(s, destino, f);
        registar_evento_motor(s, "REDISTRIB",
            "ficha=%d origem=%d destino=%d",
            f->id, id_origem, destino->id);
        movidos++;
    }
    return movidos;
}

int motor_fechar_redistribuindo(Supermercado_t *s, int id_terminal)
{
    Terminal_t     *origem;
    Terminal_t     *destino;
    FichaCliente_t *f;
    int             movidos = 0;

    if (!s || id_terminal <= 0) return -1;

    origem = elenco_obter_por_id(&s->elenco, id_terminal);
    if (!origem || origem->estado == EST_TERMINAL_FECHADO) return -1;

    /* Fecho imediato pedido pelo enunciado: quem esta em fila
       sai para outras caixas abertas; quem ja esta a ser
       atendido termina antes de a caixa fechar. */
    while (fila_tamanho(&origem->fila) > 0) {
        destino = melhor_aberto_excluindo(s, origem);
        if (!destino) return -1;

        f = fila_extrair_frente(&origem->fila);
        if (!f) break;

        if (!fila_inserir_atras(&destino->fila, f)) {
            fila_inserir_frente(&origem->fila, f);
            return -1;
        }

        f->id_terminal_actual = destino->id;
        f->versao++;
        agendar_oferta_no_prazo_original(s, destino, f);

        registar_evento_motor(s, "REDISTRIB",
            "ficha=%d origem=%d destino=%d (fecho imediato)",
            f->id, origem->id, destino->id);
        movidos++;
    }

    if (origem->em_atendimento) {
        terminal_drenar(origem);
        registar_evento_motor(s, "DRENAR_MANUAL",
            "terminal=%d apos redistribuicao (%d movidos)",
            origem->id, movidos);
    } else {
        terminal_fechar(origem);
        registar_evento_motor(s, "FECHO_MANUAL",
            "terminal=%d fechado apos redistribuicao (%d movidos)",
            origem->id, movidos);
    }

    return movidos;
}
