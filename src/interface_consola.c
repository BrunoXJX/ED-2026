#include "interface_consola.h"
#include "supermercado.h"
#include "elenco_terminais.h"
#include "terminal_caixa.h"
#include "fila_atendimento.h"
#include "indice_pesquisa.h"
#include "calendario_eventos.h"
#include "ficha_cliente.h"
#include "motor_simulacao.h"
#include "painel_metricas.h"
#include "cronista.h"
#include "vigilante_memoria.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Le uma linha do stdin. Devolve 1 se ok. */
static int ler_linha(char *destino, int max)
{
    if (!fgets(destino, max, stdin)) return 0;
    /* aparar \n no fim */
    {
        int n = (int) strlen(destino);
        while (n > 0 && (destino[n-1] == '\n' || destino[n-1] == '\r')) {
            destino[--n] = '\0';
        }
    }
    return 1;
}

static int ler_inteiro(const char *prompt, int *destino)
{
    char buf[64];
    printf("%s", prompt);
    fflush(stdout);
    if (!ler_linha(buf, sizeof(buf))) return 0;
    *destino = atoi(buf);
    return 1;
}

static void ler_texto(const char *prompt, char *destino, int max)
{
    printf("%s", prompt);
    fflush(stdout);
    if (!ler_linha(destino, max)) destino[0] = '\0';
}

/* ----------------------------------------------------------
 * Mostradores de menu
 * ---------------------------------------------------------- */

static void mostrar_menu_principal(const Supermercado_t *s)
{
    printf("\n+======================================================+\n");
    printf("|  GESTAO DE CAIXAS — Compra Aqui Lda.                 |\n");
    printf("|  tick=%-6d  terminais=%-2d  abertos=%-2d  espera=%-3d   |\n",
           s->tick_actual,
           elenco_total(&s->elenco),
           elenco_total_abertos(&s->elenco),
           supermercado_total_clientes_em_espera(s));
    printf("+======================================================+\n");
    printf("|  [A] OPERACAO          [B] SIMULACAO                |\n");
    printf("|  [C] CONSULTA          [D] METRICAS                 |\n");
    printf("|  [E] SISTEMA                                         |\n");
    printf("+======================================================+\n");
    printf("Escolha (ex: A1, B3, ...): ");
    fflush(stdout);
}

static void mostrar_submenu_A(void)
{
    printf("\n--- [A] OPERACAO ---\n");
    printf(" A1  abrir terminal\n");
    printf(" A2  drenar (fechar gentilmente) terminal\n");
    printf(" A3  mover ficha entre terminais\n");
    printf(" A4  redistribuir clientes de um terminal\n");
    printf(" A5  fechar terminal e redistribuir fila\n");
    printf(" A0  voltar\n");
}

static void mostrar_submenu_B(void)
{
    printf("\n--- [B] SIMULACAO ---\n");
    printf(" B1  avancar 1 tick\n");
    printf(" B2  avancar N ticks\n");
    printf(" B3  correr ate estabilizar\n");
    printf(" B4  forcar verificacao de balanco\n");
    printf(" B0  voltar\n");
}

static void mostrar_submenu_C(void)
{
    printf("\n--- [C] CONSULTA ---\n");
    printf(" C1  pesquisar ficha por nome\n");
    printf(" C2  ver terminal por id\n");
    printf(" C3  listar todos os terminais\n");
    printf(" C4  imprimir calendario de eventos\n");
    printf(" C5  listar atendidos por terminal\n");
    printf(" C0  voltar\n");
}

static void mostrar_submenu_D(void)
{
    printf("\n--- [D] METRICAS ---\n");
    printf(" D1  snapshot rapido\n");
    printf(" D2  relatorio por terminal\n");
    printf(" D3  relatorio global\n");
    printf(" D4  estatistica do indice de pesquisa\n");
    printf(" D5  vigilante de memoria\n");
    printf(" D6  exportar resumo final para .txt\n");
    printf(" D0  voltar\n");
}

static void mostrar_submenu_E(void)
{
    printf("\n--- [E] SISTEMA ---\n");
    printf(" E1  ver configuracao actual\n");
    printf(" E2  diagnostico de consistencia\n");
    printf(" E3  sair\n");
    printf(" E0  voltar\n");
}

/* ----------------------------------------------------------
 * Accoes
 * ---------------------------------------------------------- */

static void agir_A(Supermercado_t *s, int op)
{
    int  id, id2;
    char buf[64];
    int  n;
    switch (op) {
        case 1:
            ler_inteiro(" id (0 = primeiro fechado disponivel): ", &id);
            n = motor_abrir_terminal_manual(s, id);
            if (n > 0) {
                snprintf(buf, sizeof(buf), "abriu terminal #%d", n);
                cronista_acao(&s->cronista, s->tick_actual, "ABRIR", buf);
                printf(" >> terminal #%d aberto.\n", n);
            } else {
                printf(" >> nao foi possivel abrir.\n");
            }
            break;
        case 2:
            ler_inteiro(" id do terminal a drenar: ", &id);
            if (motor_drenar_terminal_manual(s, id)) {
                snprintf(buf, sizeof(buf), "drenou terminal #%d", id);
                cronista_acao(&s->cronista, s->tick_actual, "DRENAR", buf);
                printf(" >> terminal #%d marcado como DRENANDO.\n", id);
            } else {
                printf(" >> terminal nao encontrado ou nao esta aberto.\n");
            }
            break;
        case 3:
            ler_inteiro(" id da ficha: ", &id);
            ler_inteiro(" id do terminal de destino: ", &id2);
            if (motor_mover_ficha(s, id, id2)) {
                snprintf(buf, sizeof(buf), "ficha #%d -> term #%d", id, id2);
                cronista_acao(&s->cronista, s->tick_actual, "MOVER", buf);
                printf(" >> ficha movida.\n");
            } else {
                printf(" >> nao foi possivel mover (verifique ids).\n");
            }
            break;
        case 4:
            ler_inteiro(" id do terminal de origem: ", &id);
            n = motor_redistribuir(s, id);
            snprintf(buf, sizeof(buf), "redistribui term #%d (%d movidos)", id, n);
            cronista_acao(&s->cronista, s->tick_actual, "REDISTRIB", buf);
            printf(" >> %d fichas movidas.\n", n);
            break;
        case 5:
            ler_inteiro(" id do terminal a fechar: ", &id);
            n = motor_fechar_redistribuindo(s, id);
            if (n >= 0) {
                snprintf(buf, sizeof(buf), "fechou term #%d (%d movidos)", id, n);
                cronista_acao(&s->cronista, s->tick_actual, "FECHAR", buf);
                printf(" >> terminal #%d em fecho; %d fichas redistribuidas.\n", id, n);
            } else {
                printf(" >> nao foi possivel fechar (verifique se ha outra caixa aberta).\n");
            }
            break;
        default: break;
    }
}

static void agir_B(Supermercado_t *s, int op)
{
    int  n;
    char buf[64];
    switch (op) {
        case 1:
            motor_passo(s);
            cronista_acao(&s->cronista, s->tick_actual, "SIM", "+1 tick");
            printf(" >> tick=%d\n", s->tick_actual);
            break;
        case 2:
            ler_inteiro(" quantos ticks? ", &n);
            motor_correr_n_ticks(s, n);
            snprintf(buf, sizeof(buf), "+%d ticks", n);
            cronista_acao(&s->cronista, s->tick_actual, "SIM", buf);
            printf(" >> tick=%d\n", s->tick_actual);
            break;
        case 3:
            motor_correr_ate_estabilizar(s);
            cronista_acao(&s->cronista, s->tick_actual, "SIM", "estabilizar");
            printf(" >> simulacao estabilizada no tick %d.\n", s->tick_actual);
            break;
        case 4:
            motor_verificar_balanco(s);
            cronista_acao(&s->cronista, s->tick_actual, "BALANCO", "manual");
            printf(" >> verificacao de balanco executada.\n");
            break;
        default: break;
    }
}

static void agir_C(Supermercado_t *s, int op)
{
    char            nome[NOME_FICHA_MAX];
    char            buf[64];
    int             id;
    FichaCliente_t *f;
    Terminal_t     *t;
    NoTerminal_t   *cur_t;
    switch (op) {
        case 1:
            ler_texto(" nome da ficha: ", nome, sizeof(nome));
            f = indice_procurar(&s->indice, nome);
            if (f) {
                ficha_imprimir_detalhe(f);
                snprintf(buf, sizeof(buf), "encontrou %s", nome);
                cronista_acao(&s->cronista, s->tick_actual, "PESQUISA", buf);
            } else {
                printf(" >> nao encontrada.\n");
                snprintf(buf, sizeof(buf), "nao encontrou %s", nome);
                cronista_acao(&s->cronista, s->tick_actual, "PESQUISA", buf);
            }
            break;
        case 2:
            ler_inteiro(" id do terminal: ", &id);
            t = elenco_obter_por_id(&s->elenco, id);
            if (t) terminal_imprimir_detalhe(t);
            else   printf(" >> terminal nao encontrado.\n");
            break;
        case 3:
            elenco_imprimir(&s->elenco);
            break;
        case 4:
            calendario_imprimir(&s->calendario);
            break;
        case 5:
            ler_inteiro(" id do terminal (0 = todos): ", &id);
            if (id <= 0) {
                for (cur_t = s->elenco.cabeca; cur_t; cur_t = cur_t->seguinte) {
                    if (cur_t->terminal) terminal_imprimir_atendidos(cur_t->terminal);
                }
            } else {
                t = elenco_obter_por_id(&s->elenco, id);
                if (t) terminal_imprimir_atendidos(t);
                else   printf(" >> terminal nao encontrado.\n");
            }
            break;
        default: break;
    }
}

static void agir_D(Supermercado_t *s, int op)
{
    int rc;
    switch (op) {
        case 1: painel_snapshot(s);             break;
        case 2: painel_relatorio_terminais(s);  break;
        case 3: painel_relatorio_global(s);     break;
        case 4: indice_imprimir_estatistica(&s->indice); break;
        case 5: vigia_relatorio();              break;
        case 6:
            rc = painel_exportar_resumo(s, "logs/resumo_final.txt");
            if (rc) {
                printf(" >> resumo escrito em logs/resumo_final.txt\n");
                cronista_acao(&s->cronista, s->tick_actual, "EXPORT", "resumo_final.txt");
            } else {
                printf(" >> falhou ao exportar.\n");
            }
            break;
        default: break;
    }
}

static int agir_E(Supermercado_t *s, int op)
{
    switch (op) {
        case 1: configuracao_mostrar(&s->config); return 0;
        case 2: painel_diagnostico(s);            return 0;
        case 3:
            cronista_acao(&s->cronista, s->tick_actual, "SAIR", "fim de sessao");
            return 1;
        default: return 0;
    }
}

/* ----------------------------------------------------------
 * Loop principal
 * ---------------------------------------------------------- */

void interface_loop(Supermercado_t *s)
{
    char linha[16];
    char cat;
    int  op;

    if (!s) return;

    for (;;) {
        mostrar_menu_principal(s);
        if (!ler_linha(linha, sizeof(linha))) break;
        if (!linha[0]) continue;

        cat = (char) toupper((unsigned char) linha[0]);
        op  = (linha[1]) ? atoi(linha + 1) : 0;

        switch (cat) {
            case 'A':
                if (op == 0) { mostrar_submenu_A(); }
                else         { agir_A(s, op); }
                break;
            case 'B':
                if (op == 0) { mostrar_submenu_B(); }
                else         { agir_B(s, op); }
                break;
            case 'C':
                if (op == 0) { mostrar_submenu_C(); }
                else         { agir_C(s, op); }
                break;
            case 'D':
                if (op == 0) { mostrar_submenu_D(); }
                else         { agir_D(s, op); }
                break;
            case 'E':
                if (op == 0) { mostrar_submenu_E(); }
                else if (agir_E(s, op)) return;
                break;
            default:
                printf(" >> categoria desconhecida (use A..E).\n");
                break;
        }
    }
}
