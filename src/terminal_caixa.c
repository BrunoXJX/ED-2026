#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

/*
 * Catalogo de nomes de operadores. Sao escolhidos de forma
 * deterministica a partir do id, para que reexecutoes
 * coerentes do programa produzam sempre os mesmos nomes.
 */
static const char *NOMES_OPERADORES[] = {  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    "Ana Pereira",   "Bruno Sousa",  "Carla Lopes",   "Diogo Reis",  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
    "Eva Castro",    "Fabio Mendes", "Gisela Tavares","Hugo Marques",  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
    "Ines Cardoso",  "Joao Vidal",   "Karina Neves",  "Luis Rocha",  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
    "Mariana Pinto", "Nuno Garcia",  "Olga Antunes",  "Pedro Costa"  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
};  /* linha: Fecha a declaracao composta atual. */
static const int N_NOMES_OPERADORES = sizeof(NOMES_OPERADORES) / sizeof(NOMES_OPERADORES[0]);  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

void terminal_gerar_nome_operador(int id, char *destino, int max)  /* linha: Inicia ou continua a definicao da funcao terminal_gerar_nome_operador. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!destino || max <= 0) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (id < 0) id = -id;  /* linha: Testa a condicao antes de executar o bloco associado. */
    strncpy(destino, NOMES_OPERADORES[id % N_NOMES_OPERADORES], (size_t) (max - 1));  /* linha: Copia texto para um buffer respeitando o tamanho indicado. */
    destino[max - 1] = '\0';  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

Terminal_t *terminal_criar(int id)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    t = (Terminal_t *) vigia_pedir_z(sizeof(Terminal_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!t) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    t->id     = id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t->estado = EST_TERMINAL_FECHADO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    fila_iniciar(&t->fila);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    terminal_gerar_nome_operador(id, t->nome_operador, NOME_OPERADOR_MAX);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    return t;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_destruir(Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_destruir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoAtendido_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    fila_limpar(&t->fila);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    /* libertar lista de atendidos */
    cur = t->lista_atendidos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    vigia_devolver(t);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_abrir(Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_abrir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    t->estado = EST_TERMINAL_ABERTO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_drenar(Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_drenar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (t->estado == EST_TERMINAL_ABERTO) t->estado = EST_TERMINAL_DRENANDO;  /* linha: Testa a condicao antes de executar o bloco associado. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_fechar(Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_fechar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    t->estado = EST_TERMINAL_FECHADO;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int terminal_total_pessoas(const Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_total_pessoas. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int n;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!t) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    n = fila_tamanho(&t->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (t->em_atendimento) n++;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return n;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

long terminal_tempo_estimado_total(const Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_tempo_estimado_total. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    long s;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!t) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    s = fila_tempo_estimado_total(&t->fila);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (t->em_atendimento) s += t->em_atendimento->tempo_servico_estimado;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return s;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

const char *terminal_nome_estado(const Terminal_t *t)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return "?";  /* linha: Testa a condicao antes de executar o bloco associado. */
    switch (t->estado) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case EST_TERMINAL_FECHADO:  return "FECHADO";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EST_TERMINAL_ABERTO:   return "ABERTO";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EST_TERMINAL_DRENANDO: return "DRENANDO";  /* linha: Define um caso especifico dentro da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return "?";  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_imprimir_resumo(const Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_imprimir_resumo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("  Terminal #%-2d  %-9s  oper=%-15s pessoas=%2d  est.servico=%4lds  atend=%3d  art=%4ld  ofertas=%2d\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           t->id,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           terminal_nome_estado(t),  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
           t->nome_operador,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           terminal_total_pessoas(t),  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
           terminal_tempo_estimado_total(t),  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
           t->total_atendidos,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           t->total_artigos,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           t->total_ofertas);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_imprimir_detalhe(const Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_imprimir_detalhe. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n+-- Terminal #%d ----------------------------+\n", t->id);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| operador          : %-22s |\n", t->nome_operador);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| estado            : %-22s |\n", terminal_nome_estado(t));  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| pessoas (fila+at) : %-22d |\n", terminal_total_pessoas(t));  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| tempo est. total  : %18ld s |\n", terminal_tempo_estimado_total(t));  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| atendidos         : %-22d |\n", t->total_atendidos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| artigos process.  : %-22ld |\n", t->total_artigos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. tot. espera    : %18ld s |\n", t->tempo_total_espera);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. tot. servico   : %18ld s |\n", t->tempo_total_servico);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| ofertas concedid. : %-22d |\n", t->total_ofertas);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| prejuizo total    : %18.2f EUR |\n", t->prejuizo_total);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("+--------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    if (t->em_atendimento) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("  >> em atendimento agora:\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        ficha_imprimir_linha(t->em_atendimento);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    printf("  >> fila:\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    fila_imprimir(&t->fila);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_registar_atendido(Terminal_t *t, const FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao terminal_registar_atendido. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoAtendido_t *no;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!t || !f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no = (NoAtendido_t *) vigia_pedir(sizeof(NoAtendido_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!no) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no->id                 = f->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    strncpy(no->nome, f->nome, NOME_FICHA_MAX - 1);  /* linha: Copia texto para um buffer respeitando o tamanho indicado. */
    no->nome[NOME_FICHA_MAX - 1] = '\0';  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->quantidade_artigos = f->quantidade_artigos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->total_pago         = f->total_pagar;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->recebeu_oferta     = f->recebeu_oferta;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    /* insercao na cabeca (O(1)) */
    no->seguinte          = t->lista_atendidos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t->lista_atendidos    = no;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    t->total_na_lista_atendidos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void terminal_imprimir_atendidos(const Terminal_t *t)  /* linha: Inicia ou continua a definicao da funcao terminal_imprimir_atendidos. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoAtendido_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           pos = 1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!t) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n+-- Atendidos pelo Terminal #%d (%s) ----+\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           t->id, t->nome_operador);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    if (!t->lista_atendidos) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("| (nenhum cliente atendido ainda)           |\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
        for (cur = t->lista_atendidos; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
            printf("  %3d. #%-4d %-18s art=%2d  total=%7.2f EUR%s\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
                   pos, cur->id, cur->nome,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                   cur->quantidade_artigos, cur->total_pago,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                   cur->recebeu_oferta ? "  [OFERTA]" : "");  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
            pos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    printf("+-------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
