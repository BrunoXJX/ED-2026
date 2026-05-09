#include "leitor_dados.h"
#include "configuracao.h"
#include "supermercado.h"
#include "ficha_cliente.h"
#include "terminal_caixa.h"
#include "elenco_terminais.h"
#include "fila_atendimento.h"
#include "calendario_eventos.h"
#include "indice_pesquisa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---------------------------------------------------------------
 * Helpers de parsing
 * --------------------------------------------------------------- */

/* Remove espacos no inicio e no fim. */
static char *aparar(char *s)
{
    char *p, *q;
    if (!s) return s;
    p = s;
    while (*p && isspace((unsigned char) *p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    q = s + strlen(s);
    while (q > s && isspace((unsigned char) q[-1])) { q--; *q = '\0'; }
    return s;
}

/* Remove comentarios // e # de uma linha. */
static void remover_comentarios(char *s)
{
    char *p;
    if (!s) return;
    p = strstr(s, "//");
    if (p) *p = '\0';
    p = strchr(s, '#');
    if (p) *p = '\0';
}


/* Le a proxima linha util do ficheiro (ignora vazias e
   comentarios, remove comentarios inline). */
static int proxima_linha_util(FILE *f, char *dest, int max)
{
    while (fgets(dest, max, f)) {
        remover_comentarios(dest);
        aparar(dest);
        if (*dest != '\0') return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------
 * Configuracao
 * --------------------------------------------------------------- */

int leitor_carregar_configuracao(const char *caminho, Configuracao_t *c)
{
    FILE *f;
    char  linha[LINHA_BRUTA_MAX];
    char *sep;
    char *chave, *valor;
    int   reconhecidas = 0;

    if (!c) return 0;
    configuracao_defeitos(c);
    if (!caminho) return 0;

    f = fopen(caminho, "r");
    if (!f) return 0;

    while (fgets(linha, sizeof(linha), f)) {
        remover_comentarios(linha);
        aparar(linha);
        if (*linha == '\0') continue;

        /* tenta separador '=' */
        sep = strchr(linha, '=');
        if (sep) {
            *sep  = '\0';
            chave = aparar(linha);
            valor = aparar(sep + 1);
        } else {
            /* formato "CHAVE VALOR" (espacos como separador) */
            chave = linha;
            sep   = chave;
            while (*sep && !isspace((unsigned char) *sep)) sep++;
            if (!*sep) continue;
            *sep  = '\0';
            valor = aparar(sep + 1);
            chave = aparar(chave);
        }

        if (!chave || !valor || !*chave || !*valor) continue;

        if      (strcmp(chave, "MAX_ESPERA")       == 0) { c->max_espera       = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "N_CAIXAS")         == 0) { c->n_caixas         = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "TEMPO_PRODUTO")    == 0) { c->tempo_produto    = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "TEMPO_ATENDIMENTO_PRODUTO") == 0) { c->tempo_produto = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "MAX_PRECO")        == 0) { c->max_preco        = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "MAX_FILA")         == 0) { c->max_fila         = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "MIN_FILA")         == 0) { c->min_fila         = atoi(valor); reconhecidas++; }
        else if (strcmp(chave, "TICK_VERIFICACAO") == 0) { c->tick_verificacao = atoi(valor); reconhecidas++; }
    }
    fclose(f);
    return reconhecidas;
}

/* ---------------------------------------------------------------
 * Dados — formato legado: "nome;quantidade", uma linha por cliente
 * --------------------------------------------------------------- */

static int carregar_dados_legado(FILE *f, const char *primeira_linha,
                                 Supermercado_t *s)
{
    char            linha[LINHA_BRUTA_MAX];
    char           *sep;
    char           *nome, *qtd_txt;
    int             qtd;
    int             carregadas = 0;
    FichaCliente_t *ficha;

    /* processar a primeira linha (ja lida) */
    strncpy(linha, primeira_linha, sizeof(linha) - 1);
    linha[sizeof(linha) - 1] = '\0';
    goto processar_linha;

    while (fgets(linha, sizeof(linha), f)) {
        remover_comentarios(linha);
        aparar(linha);
        if (*linha == '\0') continue;

processar_linha:
        sep = strchr(linha, ';');
        if (!sep) {
            sep = strchr(linha, '\t');
            if (!sep) sep = strchr(linha, ',');
        }
        if (!sep) continue;
        *sep    = '\0';
        nome    = aparar(linha);
        qtd_txt = aparar(sep + 1);
        if (!*nome || !*qtd_txt) continue;
        qtd = atoi(qtd_txt);
        if (qtd <= 0) continue;

        ficha = ficha_criar(s->proximo_id_ficha,
                            nome,
                            qtd,
                            s->config.tempo_produto,
                            s->config.max_preco);
        if (!ficha) continue;

        if (!supermercado_adicionar_ficha(s, ficha)) {
            ficha_destruir(ficha);
            continue;
        }
        s->proximo_id_ficha++;
        carregadas++;
    }
    return carregadas;
}

/* ---------------------------------------------------------------
 * Dados — formato do enunciado:
 *   N                      (numero de caixas)
 *   NomeCaixa : 0|1        (nome : estado activo/inactivo)
 *   M                      (numero de clientes nesta caixa)
 *   NomeCliente : artigos  (para cada cliente)
 * --------------------------------------------------------------- */

static int carregar_dados_enunciado(FILE *f, int n_caixas,
                                    Supermercado_t *s)
{
    char            linha[LINHA_BRUTA_MAX];
    char           *sep;
    int             estado_caixa, n_clientes;
    int             qtd;
    int             carregadas = 0;
    int             i, j;
    Terminal_t     *t;
    FichaCliente_t *ficha;

    for (i = 0; i < n_caixas; i++) {
        /* linha "NomeCaixa : estado" */
        if (!proxima_linha_util(f, linha, sizeof(linha))) break;
        sep = strchr(linha, ':');
        if (!sep) continue;
        *sep = '\0';
        /* aparar(linha) seria o nome da caixa (nao usado — geramos
           nomes de operador deterministicamente) */
        estado_caixa = atoi(aparar(sep + 1));

        /* criar terminal */
        t = terminal_criar(i + 1);
        if (t) {
            if (estado_caixa) terminal_abrir(t);
            if (!elenco_adicionar(&s->elenco, t)) {
                terminal_destruir(t);
                t = NULL;
            }
        }

        /* linha com numero de clientes */
        if (!proxima_linha_util(f, linha, sizeof(linha))) break;
        n_clientes = atoi(linha);

        /* cada cliente */
        for (j = 0; j < n_clientes; j++) {
            if (!proxima_linha_util(f, linha, sizeof(linha))) break;
            sep = strchr(linha, ':');
            if (!sep) continue;
            *sep = '\0';
            qtd  = atoi(aparar(sep + 1));
            aparar(linha);
            if (!*linha || qtd <= 0) continue;

            ficha = ficha_criar(s->proximo_id_ficha,
                                linha,
                                qtd,
                                s->config.tempo_produto,
                                s->config.max_preco);
            if (!ficha) continue;
            if (!t) {
                ficha_destruir(ficha);
                continue;
            }

            ficha->instante_chegada   = s->tick_actual;
            ficha->id_terminal_actual = t->id;
            ficha->estado             = EST_FICHA_AGUARDA;

            if (!indice_inserir(&s->indice, ficha)) {
                ficha_destruir(ficha);
                continue;
            }

            if (!fila_inserir_atras(&t->fila, ficha)) {
                indice_remover(&s->indice, ficha->nome);
                ficha_destruir(ficha);
                continue;
            }
            s->total_clientes_no_sistema++;

            /* agendar oferta devida */
            if (!calendario_agendar(&s->calendario,
                                    ficha->instante_chegada + s->config.max_espera,
                                    EVT_OFERTA_DEVIDA, t, ficha)) {
                fila_remover_por_id(&t->fila, ficha->id);
                indice_remover(&s->indice, ficha->nome);
                ficha_destruir(ficha);
                s->total_clientes_no_sistema--;
                continue;
            }

            s->proximo_id_ficha++;
            carregadas++;
        }
    }
    return carregadas;
}

/* ---------------------------------------------------------------
 * Funcao publica: auto-detecta o formato
 * --------------------------------------------------------------- */

int leitor_carregar_dados(const char *caminho, Supermercado_t *s)
{
    FILE *f;
    char  linha[LINHA_BRUTA_MAX];
    int   n_caixas;
    int   carregadas;

    if (!s || !caminho) return 0;
    f = fopen(caminho, "r");
    if (!f) return 0;

    /* ler primeira linha util para decidir o formato */
    if (!proxima_linha_util(f, linha, sizeof(linha))) {
        fclose(f);
        return 0;
    }

    /* se cont tem ';' -> formato legado (nome;quantidade) */
    if (strchr(linha, ';')) {
        if (elenco_total(&s->elenco) == 0) {
            supermercado_inicializar_terminais(s, s->config.n_caixas);
        }
        carregadas = carregar_dados_legado(f, linha, s);
        fclose(f);
        return carregadas;
    }

    /* caso contrario, formato do enunciado: primeira linha e o
       numero de caixas */
    n_caixas = atoi(linha);
    if (n_caixas <= 0) { fclose(f); return 0; }
    carregadas = carregar_dados_enunciado(f, n_caixas, s);
    fclose(f);
    return carregadas;
}
