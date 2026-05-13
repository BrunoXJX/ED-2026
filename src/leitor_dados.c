#include "leitor_dados.h"  /* linha: Importa as declaracoes locais de leitor_dados.h usadas neste modulo. */
#include "configuracao.h"  /* linha: Importa as declaracoes locais de configuracao.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <stdlib.h>  /* linha: Importa a biblioteca padrao stdlib.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */
#include <ctype.h>  /* linha: Importa a biblioteca padrao ctype.h necessaria neste modulo. */

/* ---------------------------------------------------------------
 * Helpers de parsing
 * --------------------------------------------------------------- */

/* Remove espacos no inicio e no fim. */
static char *aparar(char *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char *p, *q;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return s;  /* linha: Testa a condicao antes de executar o bloco associado. */
    p = s;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (*p && isspace((unsigned char) *p)) p++;  /* linha: Repete ou controla o bloco com base na condicao indicada. */
    if (p != s) memmove(s, p, strlen(p) + 1);  /* linha: Testa a condicao antes de executar o bloco associado. */
    q = s + strlen(s);  /* linha: Calcula o comprimento da string usada nesta instrucao. */
    while (q > s && isspace((unsigned char) q[-1])) { q--; *q = '\0'; }  /* linha: Repete ou controla o bloco com base na condicao indicada. */
    return s;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Remove comentarios // e # de uma linha. */
static void remover_comentarios(char *s)  /* linha: Inicia ou continua a definicao da funcao remover_comentarios. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char *p;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    p = strstr(s, "//");  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (p) *p = '\0';  /* linha: Testa a condicao antes de executar o bloco associado. */
    p = strchr(s, '#');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (p) *p = '\0';  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */


/* Le a proxima linha util do ficheiro (ignora vazias e
   comentarios, remove comentarios inline). */
static int proxima_linha_util(FILE *f, char *dest, int max)  /* linha: Inicia ou continua a definicao da funcao proxima_linha_util. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    while (fgets(dest, max, f)) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        remover_comentarios(dest);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        aparar(dest);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        if (*dest != '\0') return 1;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ---------------------------------------------------------------
 * Configuracao
 * --------------------------------------------------------------- */

int leitor_carregar_configuracao(const char *caminho, Configuracao_t *c)  /* linha: Inicia ou continua a definicao da funcao leitor_carregar_configuracao. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FILE *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char  linha[LINHA_BRUTA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char *sep;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char *chave, *valor;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int   reconhecidas = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!c) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    configuracao_defeitos(c);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    if (!caminho) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    f = fopen(caminho, "r");  /* linha: Abre um ficheiro no modo indicado. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    while (fgets(linha, sizeof(linha), f)) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        remover_comentarios(linha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        aparar(linha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        if (*linha == '\0') continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        /* tenta separador '=' */
        sep = strchr(linha, '=');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (sep) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            *sep  = '\0';
            chave = aparar(linha);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            valor = aparar(sep + 1);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
            /* formato "CHAVE VALOR" (espacos como separador) */
            chave = linha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            sep   = chave;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            while (*sep && !isspace((unsigned char) *sep)) sep++;  /* linha: Repete ou controla o bloco com base na condicao indicada. */
            if (!*sep) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
            *sep  = '\0';
            valor = aparar(sep + 1);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            chave = aparar(chave);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */

        if (!chave || !valor || !*chave || !*valor) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        if      (strcmp(chave, "MAX_ESPERA")       == 0) { c->max_espera       = atoi(valor); reconhecidas++; }  /* linha: Testa a condicao antes de executar o bloco associado. */
        else if (strcmp(chave, "N_CAIXAS")         == 0) { c->n_caixas         = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "TEMPO_PRODUTO")    == 0) { c->tempo_produto    = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "TEMPO_ATENDIMENTO_PRODUTO") == 0) { c->tempo_produto = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "MAX_PRECO")        == 0) { c->max_preco        = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "MAX_FILA")         == 0) { c->max_fila         = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "MIN_FILA")         == 0) { c->min_fila         = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
        else if (strcmp(chave, "TICK_VERIFICACAO") == 0) { c->tick_verificacao = atoi(valor); reconhecidas++; }  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    fclose(f);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
    return reconhecidas;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ---------------------------------------------------------------
 * Dados — formato legado: "nome;quantidade", uma linha por cliente
 * --------------------------------------------------------------- */

static int carregar_dados_legado(FILE *f, const char *primeira_linha,  /* linha: Inicia ou continua a definicao da funcao carregar_dados_legado. */
                                 Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char            linha[LINHA_BRUTA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char           *sep;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char           *nome, *qtd_txt;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             qtd;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int             carregadas = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    /* processar a primeira linha (ja lida) */
    strncpy(linha, primeira_linha, sizeof(linha) - 1);  /* linha: Copia texto para um buffer respeitando o tamanho indicado. */
    linha[sizeof(linha) - 1] = '\0';  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    goto processar_linha;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    while (fgets(linha, sizeof(linha), f)) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        remover_comentarios(linha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        aparar(linha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        if (*linha == '\0') continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

processar_linha:  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
        sep = strchr(linha, ';');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!sep) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            sep = strchr(linha, '\t');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (!sep) sep = strchr(linha, ',');  /* linha: Testa a condicao antes de executar o bloco associado. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        if (!sep) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        *sep    = '\0';
        nome    = aparar(linha);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        qtd_txt = aparar(sep + 1);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!*nome || !*qtd_txt) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        qtd = atoi(qtd_txt);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (qtd <= 0) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        ficha = ficha_criar(s->proximo_id_ficha,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                            nome,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            qtd,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            s->config.tempo_produto,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                            s->config.max_preco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        if (!ficha) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        if (!supermercado_adicionar_ficha(s, ficha)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            ficha_destruir(ficha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        s->proximo_id_ficha++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        carregadas++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return carregadas;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ---------------------------------------------------------------
 * Dados — formato do enunciado:
 *   N                      (numero de caixas)
 *   NomeCaixa : 0|1        (nome : estado activo/inactivo)
 *   M                      (numero de clientes nesta caixa)
 *   NomeCliente : artigos  (para cada cliente)
 * --------------------------------------------------------------- */

static int carregar_dados_enunciado(FILE *f, int n_caixas,  /* linha: Inicia ou continua a definicao da funcao carregar_dados_enunciado. */
                                    Supermercado_t *s)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char            linha[LINHA_BRUTA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char           *sep;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             estado_caixa, n_clientes;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int             qtd;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int             carregadas = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int             i, j;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Terminal_t     *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    for (i = 0; i < n_caixas; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        /* linha "NomeCaixa : estado" */
        if (!proxima_linha_util(f, linha, sizeof(linha))) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        sep = strchr(linha, ':');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!sep) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        *sep = '\0';
        /* aparar(linha) seria o nome da caixa (nao usado — geramos
           nomes de operador deterministicamente) */
        estado_caixa = atoi(aparar(sep + 1));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

        /* criar terminal */
        t = terminal_criar(i + 1);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (t) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            if (estado_caixa) terminal_abrir(t);  /* linha: Testa a condicao antes de executar o bloco associado. */
            if (!elenco_adicionar(&s->elenco, t)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                terminal_destruir(t);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                t = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */

        /* linha com numero de clientes */
        if (!proxima_linha_util(f, linha, sizeof(linha))) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        n_clientes = atoi(linha);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

        /* cada cliente */
        for (j = 0; j < n_clientes; j++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
            if (!proxima_linha_util(f, linha, sizeof(linha))) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
            sep = strchr(linha, ':');  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (!sep) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
            *sep = '\0';
            qtd  = atoi(aparar(sep + 1));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            aparar(linha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            if (!*linha || qtd <= 0) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

            ficha = ficha_criar(s->proximo_id_ficha,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                                linha,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                qtd,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                                s->config.tempo_produto,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                                s->config.max_preco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            if (!ficha) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
            if (!t) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                ficha_destruir(ficha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
            }  /* linha: Fecha o bloco de instrucoes atual. */

            ficha->instante_chegada   = s->tick_actual;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            ficha->id_terminal_actual = t->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            ficha->estado             = EST_FICHA_AGUARDA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

            if (!indice_inserir(&s->indice, ficha)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                ficha_destruir(ficha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
            }  /* linha: Fecha o bloco de instrucoes atual. */

            if (!fila_inserir_atras(&t->fila, ficha)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                indice_remover(&s->indice, ficha->nome);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                ficha_destruir(ficha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            s->total_clientes_no_sistema++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

            /* agendar oferta devida */
            if (!calendario_agendar(&s->calendario,  /* linha: Testa a condicao antes de executar o bloco associado. */
                                    ficha->instante_chegada + s->config.max_espera,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                                    EVT_OFERTA_DEVIDA, t, ficha)) {  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                fila_remover_por_id(&t->fila, ficha->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                indice_remover(&s->indice, ficha->nome);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                ficha_destruir(ficha);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                s->total_clientes_no_sistema--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
                continue;  /* linha: Salta para a proxima iteracao do ciclo atual. */
            }  /* linha: Fecha o bloco de instrucoes atual. */

            s->proximo_id_ficha++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
            carregadas++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return carregadas;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ---------------------------------------------------------------
 * Funcao publica: auto-detecta o formato
 * --------------------------------------------------------------- */

int leitor_carregar_dados(const char *caminho, Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao leitor_carregar_dados. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FILE *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char  linha[LINHA_BRUTA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int   n_caixas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int   carregadas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s || !caminho) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    f = fopen(caminho, "r");  /* linha: Abre um ficheiro no modo indicado. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* ler primeira linha util para decidir o formato */
    if (!proxima_linha_util(f, linha, sizeof(linha))) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        fclose(f);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    /* se cont tem ';' -> formato legado (nome;quantidade) */
    if (strchr(linha, ';')) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (elenco_total(&s->elenco) == 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            supermercado_inicializar_terminais(s, s->config.n_caixas);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        carregadas = carregar_dados_legado(f, linha, s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        fclose(f);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
        return carregadas;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    /* caso contrario, formato do enunciado: primeira linha e o
       numero de caixas */
    n_caixas = atoi(linha);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (n_caixas <= 0) { fclose(f); return 0; }  /* linha: Testa a condicao antes de executar o bloco associado. */
    carregadas = carregar_dados_enunciado(f, n_caixas, s);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    fclose(f);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
    return carregadas;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
