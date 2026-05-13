#include "interface_consola.h"  /* linha: Importa as declaracoes locais de interface_consola.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */
#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "motor_simulacao.h"  /* linha: Importa as declaracoes locais de motor_simulacao.h usadas neste modulo. */
#include "painel_metricas.h"  /* linha: Importa as declaracoes locais de painel_metricas.h usadas neste modulo. */
#include "cronista.h"  /* linha: Importa as declaracoes locais de cronista.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <stdlib.h>  /* linha: Importa a biblioteca padrao stdlib.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */
#include <ctype.h>  /* linha: Importa a biblioteca padrao ctype.h necessaria neste modulo. */

/* Le uma linha do stdin. Devolve 1 se ok. */
static int ler_linha(char *destino, int max)  /* linha: Inicia ou continua a definicao da funcao ler_linha. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!fgets(destino, max, stdin)) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    /* aparar \n no fim */
    {  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
        int n = (int) strlen(destino);  /* linha: Calcula o comprimento da string usada nesta instrucao. */
        while (n > 0 && (destino[n-1] == '\n' || destino[n-1] == '\r')) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
            destino[--n] = '\0';  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static int ler_inteiro(const char *prompt, int *destino)  /* linha: Inicia ou continua a definicao da funcao ler_inteiro. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char buf[64];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    printf("%s", prompt);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    fflush(stdout);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
    if (!ler_linha(buf, sizeof(buf))) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    *destino = atoi(buf);
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void ler_texto(const char *prompt, char *destino, int max)  /* linha: Inicia ou continua a definicao da funcao ler_texto. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("%s", prompt);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    fflush(stdout);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
    if (!ler_linha(destino, max)) destino[0] = '\0';  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * Mostradores de menu
 * ---------------------------------------------------------- */

static void mostrar_menu_principal(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao mostrar_menu_principal. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n+======================================================+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("|  GESTAO DE CAIXAS — Compra Aqui Lda.                 |\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("|  tick=%-6d  terminais=%-2d  abertos=%-2d  espera=%-3d   |\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           s->tick_actual,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           elenco_total(&s->elenco),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           elenco_total_abertos(&s->elenco),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           supermercado_total_clientes_em_espera(s));  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    printf("+======================================================+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("|  [A] OPERACAO          [B] SIMULACAO                |\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("|  [C] CONSULTA          [D] METRICAS                 |\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("|  [E] SISTEMA                                         |\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("+======================================================+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("Escolha (ex: A1, B3, ...): ");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    fflush(stdout);  /* linha: Forca a escrita imediata dos dados pendentes no ficheiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void mostrar_submenu_A(void)  /* linha: Inicia ou continua a definicao da funcao mostrar_submenu_A. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n--- [A] OPERACAO ---\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A1  abrir terminal\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A2  drenar (fechar gentilmente) terminal\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A3  mover ficha entre terminais\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A4  redistribuir clientes de um terminal\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A5  fechar terminal e redistribuir fila\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" A0  voltar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void mostrar_submenu_B(void)  /* linha: Inicia ou continua a definicao da funcao mostrar_submenu_B. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n--- [B] SIMULACAO ---\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" B1  avancar 1 tick\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" B2  avancar N ticks\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" B3  correr ate estabilizar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" B4  forcar verificacao de balanco\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" B0  voltar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void mostrar_submenu_C(void)  /* linha: Inicia ou continua a definicao da funcao mostrar_submenu_C. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n--- [C] CONSULTA ---\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C1  pesquisar ficha por nome\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C2  ver terminal por id\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C3  listar todos os terminais\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C4  imprimir calendario de eventos\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C5  listar atendidos por terminal\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" C0  voltar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void mostrar_submenu_D(void)  /* linha: Inicia ou continua a definicao da funcao mostrar_submenu_D. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n--- [D] METRICAS ---\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D1  snapshot rapido\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D2  relatorio por terminal\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D3  relatorio global\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D4  estatistica do indice de pesquisa\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D5  vigilante de memoria\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D6  exportar resumo final para .txt\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" D0  voltar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void mostrar_submenu_E(void)  /* linha: Inicia ou continua a definicao da funcao mostrar_submenu_E. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n--- [E] SISTEMA ---\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" E1  ver configuracao actual\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" E2  diagnostico de consistencia\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" E3  sair\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf(" E0  voltar\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * Accoes
 * ---------------------------------------------------------- */

static void agir_A(Supermercado_t *s, int op)  /* linha: Inicia ou continua a definicao da funcao agir_A. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int  id, id2;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char buf[64];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int  n;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    switch (op) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case 1:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id (0 = primeiro fechado disponivel): ", &id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            n = motor_abrir_terminal_manual(s, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (n > 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                snprintf(buf, sizeof(buf), "abriu terminal #%d", n);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "ABRIR", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                printf(" >> terminal #%d aberto.\n", n);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> nao foi possivel abrir.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 2:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id do terminal a drenar: ", &id);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            if (motor_drenar_terminal_manual(s, id)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                snprintf(buf, sizeof(buf), "drenou terminal #%d", id);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "DRENAR", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                printf(" >> terminal #%d marcado como DRENANDO.\n", id);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> terminal nao encontrado ou nao esta aberto.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 3:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id da ficha: ", &id);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            ler_inteiro(" id do terminal de destino: ", &id2);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            if (motor_mover_ficha(s, id, id2)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                snprintf(buf, sizeof(buf), "ficha #%d -> term #%d", id, id2);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "MOVER", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                printf(" >> ficha movida.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> nao foi possivel mover (verifique ids).\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 4:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id do terminal de origem: ", &id);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            n = motor_redistribuir(s, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            snprintf(buf, sizeof(buf), "redistribui term #%d (%d movidos)", id, n);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
            cronista_acao(&s->cronista, s->tick_actual, "REDISTRIB", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            printf(" >> %d fichas movidas.\n", n);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 5:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id do terminal a fechar: ", &id);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            n = motor_fechar_redistribuindo(s, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (n >= 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                snprintf(buf, sizeof(buf), "fechou term #%d (%d movidos)", id, n);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "FECHAR", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                printf(" >> terminal #%d em fecho; %d fichas redistribuidas.\n", id, n);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> nao foi possivel fechar (verifique se ha outra caixa aberta).\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        default: break;  /* linha: Define o comportamento por defeito da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void agir_B(Supermercado_t *s, int op)  /* linha: Inicia ou continua a definicao da funcao agir_B. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int  n;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char buf[64];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    switch (op) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case 1:  /* linha: Define um caso especifico dentro da escolha multipla. */
            motor_passo(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            cronista_acao(&s->cronista, s->tick_actual, "SIM", "+1 tick");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            printf(" >> tick=%d\n", s->tick_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 2:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" quantos ticks? ", &n);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            motor_correr_n_ticks(s, n);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            snprintf(buf, sizeof(buf), "+%d ticks", n);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
            cronista_acao(&s->cronista, s->tick_actual, "SIM", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            printf(" >> tick=%d\n", s->tick_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 3:  /* linha: Define um caso especifico dentro da escolha multipla. */
            motor_correr_ate_estabilizar(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            cronista_acao(&s->cronista, s->tick_actual, "SIM", "estabilizar");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            printf(" >> simulacao estabilizada no tick %d.\n", s->tick_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 4:  /* linha: Define um caso especifico dentro da escolha multipla. */
            motor_verificar_balanco(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            cronista_acao(&s->cronista, s->tick_actual, "BALANCO", "manual");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            printf(" >> verificacao de balanco executada.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        default: break;  /* linha: Define o comportamento por defeito da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void agir_C(Supermercado_t *s, int op)  /* linha: Inicia ou continua a definicao da funcao agir_C. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char            nome[NOME_FICHA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char            buf[64];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             id;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    Terminal_t     *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    NoTerminal_t   *cur_t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    switch (op) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case 1:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_texto(" nome da ficha: ", nome, sizeof(nome));  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            f = indice_procurar(&s->indice, nome);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (f) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                ficha_imprimir_detalhe(f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
                snprintf(buf, sizeof(buf), "encontrou %s", nome);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "PESQUISA", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> nao encontrada.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
                snprintf(buf, sizeof(buf), "nao encontrou %s", nome);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
                cronista_acao(&s->cronista, s->tick_actual, "PESQUISA", buf);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 2:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id do terminal: ", &id);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            t = elenco_obter_por_id(&s->elenco, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (t) terminal_imprimir_detalhe(t);  /* linha: Testa a condicao antes de executar o bloco associado. */
            else   printf(" >> terminal nao encontrado.\n");  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 3:  /* linha: Define um caso especifico dentro da escolha multipla. */
            elenco_imprimir(&s->elenco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 4:  /* linha: Define um caso especifico dentro da escolha multipla. */
            calendario_imprimir(&s->calendario);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        case 5:  /* linha: Define um caso especifico dentro da escolha multipla. */
            ler_inteiro(" id do terminal (0 = todos): ", &id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (id <= 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                for (cur_t = s->elenco.cabeca; cur_t; cur_t = cur_t->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
                    if (cur_t->terminal) terminal_imprimir_atendidos(cur_t->terminal);  /* linha: Testa a condicao antes de executar o bloco associado. */
                }  /* linha: Fecha o bloco de instrucoes atual. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                t = elenco_obter_por_id(&s->elenco, id);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
                if (t) terminal_imprimir_atendidos(t);  /* linha: Testa a condicao antes de executar o bloco associado. */
                else   printf(" >> terminal nao encontrado.\n");  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        default: break;  /* linha: Define o comportamento por defeito da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static void agir_D(Supermercado_t *s, int op)  /* linha: Inicia ou continua a definicao da funcao agir_D. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int rc;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    switch (op) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case 1: painel_snapshot(s);             break;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 2: painel_relatorio_terminais(s);  break;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 3: painel_relatorio_global(s);     break;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 4: indice_imprimir_estatistica(&s->indice); break;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 5: vigia_relatorio();              break;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 6:  /* linha: Define um caso especifico dentro da escolha multipla. */
            rc = painel_exportar_resumo(s, "logs/resumo_final.txt");  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (rc) {  /* linha: Testa a condicao antes de executar o bloco associado. */
                printf(" >> resumo escrito em logs/resumo_final.txt\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
                cronista_acao(&s->cronista, s->tick_actual, "EXPORT", "resumo_final.txt");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
                printf(" >> falhou ao exportar.\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
            }  /* linha: Fecha o bloco de instrucoes atual. */
            break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        default: break;  /* linha: Define o comportamento por defeito da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

static int agir_E(Supermercado_t *s, int op)  /* linha: Inicia ou continua a definicao da funcao agir_E. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    switch (op) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case 1: configuracao_mostrar(&s->config); return 0;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 2: painel_diagnostico(s);            return 0;  /* linha: Define um caso especifico dentro da escolha multipla. */
        case 3:  /* linha: Define um caso especifico dentro da escolha multipla. */
            cronista_acao(&s->cronista, s->tick_actual, "SAIR", "fim de sessao");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
        default: return 0;  /* linha: Define o comportamento por defeito da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* ----------------------------------------------------------
 * Loop principal
 * ---------------------------------------------------------- */

void interface_loop(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao interface_loop. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    char linha[16];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    char cat;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int  op;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    for (;;) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        mostrar_menu_principal(s);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        if (!ler_linha(linha, sizeof(linha))) break;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (!linha[0]) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */

        cat = (char) toupper((unsigned char) linha[0]);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        op  = (linha[1]) ? atoi(linha + 1) : 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

        switch (cat) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
            case 'A':  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (op == 0) { mostrar_submenu_A(); }  /* linha: Testa a condicao antes de executar o bloco associado. */
                else         { agir_A(s, op); }  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
            case 'B':  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (op == 0) { mostrar_submenu_B(); }  /* linha: Testa a condicao antes de executar o bloco associado. */
                else         { agir_B(s, op); }  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
            case 'C':  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (op == 0) { mostrar_submenu_C(); }  /* linha: Testa a condicao antes de executar o bloco associado. */
                else         { agir_C(s, op); }  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
            case 'D':  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (op == 0) { mostrar_submenu_D(); }  /* linha: Testa a condicao antes de executar o bloco associado. */
                else         { agir_D(s, op); }  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
            case 'E':  /* linha: Define um caso especifico dentro da escolha multipla. */
                if (op == 0) { mostrar_submenu_E(); }  /* linha: Testa a condicao antes de executar o bloco associado. */
                else if (agir_E(s, op)) return;  /* linha: Testa uma condicao alternativa quando a anterior falhou. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
            default:  /* linha: Define o comportamento por defeito da escolha multipla. */
                printf(" >> categoria desconhecida (use A..E).\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
                break;  /* linha: Interrompe o ciclo ou a escolha multipla atual. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */
