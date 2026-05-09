#include "cronista.h"
#include "supermercado.h"
#include "elenco_terminais.h"
#include "terminal_caixa.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(p) mkdir((p), 0755)
#endif

static void garantir_pasta_logs(void)
{
    /* tenta criar; se ja existir, ignora-se o erro */
    MKDIR("logs");
}

int cronista_abrir(Cronista_t *c)
{
    if (!c) return 0;
    garantir_pasta_logs();

    c->acoes     = NULL;
    c->motor     = NULL;
    c->historico = NULL;

    c->acoes = fopen("logs/acoes_utilizador.csv", "w");
    if (!c->acoes) return 0;
    fprintf(c->acoes, "tick;categoria;descricao\n");

    c->motor = fopen("logs/eventos_motor.csv", "w");
    if (!c->motor) {
        fclose(c->acoes);
        c->acoes = NULL;
        return 0;
    }
    fprintf(c->motor, "tick;tipo;descricao\n");

    c->historico = fopen("logs/historico_simulacao.csv", "w");
    if (!c->historico) {
        fclose(c->motor);
        fclose(c->acoes);
        c->motor = NULL;
        c->acoes = NULL;
        return 0;
    }
    fprintf(c->historico,
            "tick;caixas_abertas;clientes_espera;clientes_no_sistema;"
            "clientes_atendidos;produtos_processados;media_espera;"
            "media_servico;ofertas;prejuizo;memoria_em_uso;"
            "memoria_desperdicada\n");
    return 1;
}

void cronista_fechar(Cronista_t *c)
{
    if (!c) return;
    if (c->acoes) { fclose(c->acoes); c->acoes = NULL; }
    if (c->motor) { fclose(c->motor); c->motor = NULL; }
    if (c->historico) { fclose(c->historico); c->historico = NULL; }
}

/* Escreve um campo em CSV escapando ; e quebras de linha. */
static void escreve_campo(FILE *f, const char *texto)
{
    int                  i;
    const unsigned char *p;
    int                  precisa = 0;
    if (!texto) { fputs("\"\"", f); return; }

    for (p = (const unsigned char *) texto; *p; p++) {
        if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') { precisa = 1; break; }
    }
    if (!precisa) { fputs(texto, f); return; }

    fputc('"', f);
    for (i = 0; texto[i]; i++) {
        if (texto[i] == '"') fputc('"', f);
        fputc(texto[i], f);
    }
    fputc('"', f);
}

void cronista_acao(Cronista_t *c, int tick,
                   const char *categoria, const char *descricao)
{
    if (!c || !c->acoes) return;
    fprintf(c->acoes, "%d;", tick);
    escreve_campo(c->acoes, categoria ? categoria : "");
    fputc(';', c->acoes);
    escreve_campo(c->acoes, descricao ? descricao : "");
    fputc('\n', c->acoes);
    fflush(c->acoes);
}

void cronista_evento(Cronista_t *c, int tick,
                     const char *tipo, const char *descricao)
{
    if (!c || !c->motor) return;
    fprintf(c->motor, "%d;", tick);
    escreve_campo(c->motor, tipo ? tipo : "");
    fputc(';', c->motor);
    escreve_campo(c->motor, descricao ? descricao : "");
    fputc('\n', c->motor);
    fflush(c->motor);
}

void cronista_snapshot(Cronista_t *c, const Supermercado_t *s)
{
    NoTerminal_t *cur;
    long          produtos = 0;
    double        media_espera = 0.0;
    double        media_servico = 0.0;

    if (!c || !c->historico || !s) return;

    /* O historico resume o estado no fim de cada tick. As somas
       por terminal sao percorridas aqui para nao duplicar esse
       contador no estado global. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {
        if (cur->terminal) produtos += cur->terminal->total_artigos;
    }

    if (s->total_clientes_atendidos > 0) {
        media_espera = (double) s->soma_tempos_espera /
                       (double) s->total_clientes_atendidos;
        media_servico = (double) s->soma_tempos_servico /
                        (double) s->total_clientes_atendidos;
    }

    fprintf(c->historico,
            "%d;%d;%d;%d;%d;%ld;%.2f;%.2f;%d;%.2f;%lu;%lu\n",
            s->tick_actual,
            elenco_total_abertos(&s->elenco),
            supermercado_total_clientes_em_espera(s),
            s->total_clientes_no_sistema,
            s->total_clientes_atendidos,
            produtos,
            media_espera,
            media_servico,
            s->total_ofertas_concedidas,
            s->prejuizo_acumulado,
            (unsigned long) vigia_em_uso(),
            (unsigned long) supermercado_memoria_desperdicada(s));
    fflush(c->historico);
}
