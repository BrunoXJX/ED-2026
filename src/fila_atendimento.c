#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

void fila_iniciar(FilaAtendimento_t *f)  /* linha: Inicia ou continua a definicao da funcao fila_iniciar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    f->frente = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->tras   = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int fila_tamanho(const FilaAtendimento_t *f)  /* linha: Inicia ou continua a definicao da funcao fila_tamanho. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return (f) ? f->total : 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int fila_inserir_atras(FilaAtendimento_t *f, FichaCliente_t *ficha)  /* linha: Inicia ou continua a definicao da funcao fila_inserir_atras. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t *no;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!f || !ficha) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no = (NoFila_t *) vigia_pedir(sizeof(NoFila_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!no) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no->ficha    = ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->seguinte = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (f->tras) f->tras->seguinte = no;  /* linha: Testa a condicao antes de executar o bloco associado. */
    else         f->frente         = no;  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
    f->tras = no;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->total++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int fila_inserir_frente(FilaAtendimento_t *f, FichaCliente_t *ficha)  /* linha: Inicia ou continua a definicao da funcao fila_inserir_frente. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t *no;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!f || !ficha) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no = (NoFila_t *) vigia_pedir(sizeof(NoFila_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!no) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no->ficha    = ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    no->seguinte = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    f->frente = no;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!f->tras) f->tras = no;  /* linha: Testa a condicao antes de executar o bloco associado. */
    f->total++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

FichaCliente_t *fila_extrair_frente(FilaAtendimento_t *f)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t       *no;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    if (!f || !f->frente) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    no       = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    ficha    = no->ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->frente = no->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!f->frente) f->tras = NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    vigia_devolver(no);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    f->total--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    return ficha;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

FichaCliente_t *fila_espreitar_frente(const FilaAtendimento_t *f)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!f || !f->frente) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    return f->frente->ficha;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

FichaCliente_t *fila_remover_por_id(FilaAtendimento_t *f, int id_ficha)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t       *cur, *ant;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    if (!f || !f->frente) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    cur = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    ant = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        if (cur->ficha && cur->ficha->id == id_ficha) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            if (ant) ant->seguinte = cur->seguinte;  /* linha: Testa a condicao antes de executar o bloco associado. */
            else     f->frente     = cur->seguinte;  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */
            if (cur == f->tras) f->tras = ant;  /* linha: Testa a condicao antes de executar o bloco associado. */

            ficha = cur->ficha;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            f->total--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
            return ficha;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        ant = cur;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return NULL;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

long fila_tempo_estimado_total(const FilaAtendimento_t *f)  /* linha: Inicia ou continua a definicao da funcao fila_tempo_estimado_total. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    long      soma = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoFila_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    cur = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        if (cur->ficha) soma += cur->ficha->tempo_servico_estimado;  /* linha: Testa a condicao antes de executar o bloco associado. */
        cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return soma;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void fila_limpar(FilaAtendimento_t *f)  /* linha: Inicia ou continua a definicao da funcao fila_limpar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    cur = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
        cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    f->frente = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->tras   = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->total  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void fila_imprimir(const FilaAtendimento_t *f)  /* linha: Inicia ou continua a definicao da funcao fila_imprimir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoFila_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int       pos = 1;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (!f->frente) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("    (fila vazia)\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        return;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    cur = f->frente;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
        if (cur->ficha) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            printf("    [%2d] ", pos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
            ficha_imprimir_linha(cur->ficha);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        pos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        cur = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */
