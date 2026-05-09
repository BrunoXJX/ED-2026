#include "supermercado.h"
#include "terminal_caixa.h"
#include "fila_atendimento.h"
#include "ficha_cliente.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

void supermercado_iniciar(Supermercado_t *s, const Configuracao_t *c)
{
    if (!s) return;

    if (c) s->config = *c;
    else   configuracao_defeitos(&s->config);

    elenco_iniciar    (&s->elenco);
    indice_iniciar    (&s->indice);
    calendario_iniciar(&s->calendario);
    s->cronista.acoes     = NULL;
    s->cronista.motor     = NULL;
    s->cronista.historico = NULL;

    s->tick_actual                = 0;
    s->proximo_id_ficha           = 1;
    s->total_clientes_atendidos   = 0;
    s->total_clientes_no_sistema  = 0;
    s->soma_tempos_espera         = 0;
    s->soma_tempos_servico        = 0;
    s->total_ofertas_concedidas   = 0;
    s->prejuizo_acumulado         = 0.0;
}

/* Liberta todas as fichas indexadas (e o que la esta dentro). */
static void libertar_fichas_via_indice(IndicePesquisa_t *i)
{
    int         k;
    NoIndice_t *cur, *prox;
    if (!i) return;
    for (k = 0; k < INDICE_HASH_TAM; k++) {
        cur = i->baldes[k];
        while (cur) {
            prox = cur->seguinte;
            if (cur->ficha) ficha_destruir(cur->ficha);
            vigia_devolver(cur);
            cur = prox;
        }
        i->baldes[k] = NULL;
    }
    i->total = 0;
}

void supermercado_libertar(Supermercado_t *s)
{
    if (!s) return;
    /* libertar fichas primeiro (atraves do indice, que e a
       referencia mestre); os terminais perdem entao referencias
       para fichas — o que e ok, porque vamos destruir os
       terminais a seguir. */
    libertar_fichas_via_indice(&s->indice);
    elenco_libertar    (&s->elenco);
    calendario_libertar(&s->calendario);
    cronista_fechar    (&s->cronista);
}

void supermercado_inicializar_terminais(Supermercado_t *s, int n_caixas)
{
    int         i;
    int         abrir_inicialmente;
    Terminal_t *t;
    if (!s || n_caixas <= 0) return;

    abrir_inicialmente = (n_caixas < 3) ? n_caixas : 3;

    for (i = 1; i <= n_caixas; i++) {
        t = terminal_criar(i);
        if (!t) continue;
        if (i <= abrir_inicialmente) terminal_abrir(t);
        elenco_adicionar(&s->elenco, t);
    }
}

int supermercado_adicionar_ficha(Supermercado_t *s, FichaCliente_t *f)
{
    Terminal_t *escolhido;
    if (!s || !f) return 0;

    /* tempo de chegada = tick actual */
    f->instante_chegada = s->tick_actual;

    /* escolher terminal aberto com menor tempo estimado */
    escolhido = elenco_melhor_para_novo(&s->elenco);
    if (!escolhido) {
        /* nenhum terminal aberto — abre o primeiro fechado */
        escolhido = elenco_primeiro_fechado(&s->elenco);
        if (escolhido) terminal_abrir(escolhido);
    }
    if (!escolhido) return 0;

    /* A ficha so entra no indice depois de sabermos que ha uma
       caixa valida para a receber. Isto evita referencias
       penduradas se a insercao falhar a meio. */
    if (!indice_inserir(&s->indice, f)) return 0;

    /* enfileirar */
    if (!fila_inserir_atras(&escolhido->fila, f)) {
        indice_remover(&s->indice, f->nome);
        return 0;
    }
    f->id_terminal_actual = escolhido->id;
    f->estado             = EST_FICHA_AGUARDA;
    s->total_clientes_no_sistema++;

    /* agendar oferta devida */
    if (!calendario_agendar(&s->calendario,
                            f->instante_chegada + s->config.max_espera,
                            EVT_OFERTA_DEVIDA,
                            escolhido,
                            f)) {
        fila_remover_por_id(&escolhido->fila, f->id);
        indice_remover(&s->indice, f->nome);
        s->total_clientes_no_sistema--;
        f->id_terminal_actual = -1;
        return 0;
    }
    return 1;
}

size_t supermercado_memoria_desperdicada(const Supermercado_t *s)
{
    size_t        total = 0;
    NoTerminal_t *cur;
    int           obsoletos;

    if (!s) return 0;

    /* Terminais FECHADOS — o struct e o cabecalho da fila
       continuam vivos. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_FECHADO) {
            total += sizeof(Terminal_t);
        }
    }

    /* Eventos obsoletos no calendario (lazy deletion ainda nao
       limpou). */
    obsoletos = calendario_total_obsoletos(&s->calendario);
    total    += (size_t) obsoletos * sizeof(Evento_t);

    return total;
}

int supermercado_total_terminais(const Supermercado_t *s)
{
    return (s) ? elenco_total(&s->elenco) : 0;
}

int supermercado_total_clientes_em_espera(const Supermercado_t *s)
{
    NoTerminal_t *cur;
    int           n = 0;
    if (!s) return 0;
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal) n += fila_tamanho(&cur->terminal->fila);
    }
    return n;
}
