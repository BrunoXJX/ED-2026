#include "cronista.h"  /* linha: Importa as declaracoes locais de cronista.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

#ifdef _WIN32  /* linha: Ativa este bloco apenas quando _WIN32 esta definido. */
#include <direct.h>  /* linha: Importa a biblioteca padrao direct.h necessaria neste modulo. */
#define MKDIR(p) _mkdir(p)  /* linha: Define a macro ou constante simbolica MKDIR. */
#else  /* linha: Seleciona o ramo alternativo da compilacao condicional. */
#include <sys/stat.h>  /* linha: Importa a biblioteca padrao sys/stat.h necessaria neste modulo. */
#include <sys/types.h>  /* linha: Importa a biblioteca padrao sys/types.h necessaria neste modulo. */
#define MKDIR(p) mkdir((p), 0755)  /* linha: Define a macro ou constante simbolica MKDIR. */
#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */

static void garantir_pasta_logs(void)  /* linha: Inicia ou continua a definicao da funcao garantir_pasta_logs. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    /* tenta criar; se ja existir, ignora-se o erro */
    MKDIR("logs");  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int cronista_abrir(Cronista_t *c)  /* linha: Inicia ou continua a definicao da funcao cronista_abrir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    garantir_pasta_logs();  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    c->acoes     = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->motor     = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    c->historico = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    c->acoes = fopen("logs/acoes_utilizador.csv", "w");  /* linha: Abre um ficheiro no modo indicado. */
    if (!c->acoes) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    fprintf(c->acoes, "tick;categoria;descricao\n");  /* linha: Escreve texto formatado no ficheiro indicado. */

    c->motor = fopen("logs/eventos_motor.csv", "w");  /* linha: Abre um ficheiro no modo indicado. */
    if (!c->motor) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        fclose(c->acoes);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
        c->acoes = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    fprintf(c->motor, "tick;tipo;descricao\n");  /* linha: Escreve texto formatado no ficheiro indicado. */

    c->historico = fopen("logs/historico_simulacao.csv", "w");  /* linha: Abre um ficheiro no modo indicado. */
    if (!c->historico) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        fclose(c->motor);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
        fclose(c->acoes);  /* linha: Fecha o ficheiro e liberta o descritor associado. */
        c->motor = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        c->acoes = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    fprintf(c->historico,  /* linha: Escreve texto formatado no ficheiro indicado. */
            "tick;caixas_abertas;clientes_espera;clientes_no_sistema;"  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            "clientes_atendidos;produtos_processados;media_espera;"  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            "media_servico;ofertas;prejuizo;memoria_em_uso;"  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            "memoria_desperdicada\n");  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void cronista_fechar(Cronista_t *c)  /* linha: Inicia ou continua a definicao da funcao cronista_fechar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->acoes) { fclose(c->acoes); c->acoes = NULL; }  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->motor) { fclose(c->motor); c->motor = NULL; }  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (c->historico) { fclose(c->historico); c->historico = NULL; }  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Escreve um campo em CSV escapando ; e quebras de linha. */
static void escreve_campo(FILE *f, const char *texto)  /* linha: Inicia ou continua a definicao da funcao escreve_campo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int                  i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    const unsigned char *p;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int                  precisa = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!texto) { fputs("\"\"", f); return; }  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (p = (const unsigned char *) texto; *p; p++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') { precisa = 1; break; }  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    if (!precisa) { fputs(texto, f); return; }  /* linha: Testa a condicao antes de executar o bloco associado. */

    fputc('"', f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    for (i = 0; texto[i]; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (texto[i] == '"') fputc('"', f);  /* linha: Testa a condicao antes de executar o bloco associado. */
        fputc(texto[i], f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    fputc('"', f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void cronista_acao(Cronista_t *c, int tick,  /* linha: Inicia ou continua a definicao da funcao cronista_acao. */
                   const char *categoria, const char *descricao)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c || !c->acoes) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    fprintf(c->acoes, "%d;", tick);  /* linha: Escreve texto formatado no ficheiro indicado. */
    escreve_campo(c->acoes, categoria ? categoria : "");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fputc(';', c->acoes);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    escreve_campo(c->acoes, descricao ? descricao : "");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fputc('\n', c->acoes);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fflush(c->acoes);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void cronista_evento(Cronista_t *c, int tick,  /* linha: Inicia ou continua a definicao da funcao cronista_evento. */
                     const char *tipo, const char *descricao)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!c || !c->motor) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    fprintf(c->motor, "%d;", tick);  /* linha: Escreve texto formatado no ficheiro indicado. */
    escreve_campo(c->motor, tipo ? tipo : "");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fputc(';', c->motor);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    escreve_campo(c->motor, descricao ? descricao : "");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fputc('\n', c->motor);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    fflush(c->motor);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void cronista_snapshot(Cronista_t *c, const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao cronista_snapshot. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    long          produtos = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double        media_espera = 0.0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double        media_servico = 0.0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!c || !c->historico || !s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* O historico resume o estado no fim de cada tick. As somas
       por terminal sao percorridas aqui para nao duplicar esse
       contador no estado global. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal) produtos += cur->terminal->total_artigos;  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    if (s->total_clientes_atendidos > 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        media_espera = (double) s->soma_tempos_espera /  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                       (double) s->total_clientes_atendidos;  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        media_servico = (double) s->soma_tempos_servico /  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                        (double) s->total_clientes_atendidos;  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    fprintf(c->historico,  /* linha: Escreve texto formatado no ficheiro indicado. */
            "%d;%d;%d;%d;%d;%ld;%.2f;%.2f;%d;%.2f;%lu;%lu\n",  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            s->tick_actual,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            elenco_total_abertos(&s->elenco),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            supermercado_total_clientes_em_espera(s),  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            s->total_clientes_no_sistema,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            s->total_clientes_atendidos,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            produtos,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            media_espera,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            media_servico,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            s->total_ofertas_concedidas,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            s->prejuizo_acumulado,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            (unsigned long) vigia_em_uso(),  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
            (unsigned long) supermercado_memoria_desperdicada(s));  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    fflush(c->historico);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */
