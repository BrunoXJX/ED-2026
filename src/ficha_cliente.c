#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "catalogo_artigos.h"  /* linha: Importa as declaracoes locais de catalogo_artigos.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

FichaCliente_t *ficha_criar(int         id,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            const char *nome,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            int         qtd_artigos,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                            int         tempo_produto,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                            int         max_preco_int)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    FichaCliente_t *f;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    f = (FichaCliente_t *) vigia_pedir_z(sizeof(FichaCliente_t));  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!f) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    f->id = id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (nome) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        strncpy(f->nome, nome, NOME_FICHA_MAX - 1);  /* linha: Copia texto para um buffer respeitando o tamanho indicado. */
        f->nome[NOME_FICHA_MAX - 1] = '\0';  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    } else {  /* linha: Fecha o ramo anterior e abre o caminho alternativo. */
        snprintf(f->nome, NOME_FICHA_MAX, "anonimo_%d", id);  /* linha: Formata texto com limite de tamanho para evitar ultrapassar o buffer. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    f->quantidade_artigos = qtd_artigos;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->cesto              = catalogo_construir_cesto(f->nome, qtd_artigos, max_preco_int);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->total_pagar        = catalogo_total_cesto(f->cesto, qtd_artigos);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    f->tempo_servico_estimado  = catalogo_tempo_servico(f->nome, qtd_artigos, tempo_produto);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->instante_chegada        = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->instante_inicio_servico = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->instante_saida          = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    f->id_terminal_actual = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->estado             = EST_FICHA_AGUARDA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    f->recebeu_oferta = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->indice_oferta  = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->valor_oferta   = 0.0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->versao         = 1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    return f;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void ficha_destruir(FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao ficha_destruir. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (f->cesto) catalogo_libertar_cesto(f->cesto, f->quantidade_artigos);  /* linha: Testa a condicao antes de executar o bloco associado. */
    vigia_devolver(f);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
}  /* linha: Fecha o bloco de instrucoes atual. */

const char *ficha_nome_estado(EstadoFicha_t e)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    switch (e) {  /* linha: Escolhe um ramo de execucao conforme o valor analisado. */
        case EST_FICHA_AGUARDA:     return "AGUARDA";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EST_FICHA_ATENDIMENTO: return "ATENDIMENTO";  /* linha: Define um caso especifico dentro da escolha multipla. */
        case EST_FICHA_PROCESSADA:  return "PROCESSADA";  /* linha: Define um caso especifico dentro da escolha multipla. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return "?";  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void ficha_imprimir_detalhe(const FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao ficha_imprimir_detalhe. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("\n+-- Ficha #%d ----------------------------------+\n", f->id);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| nome              : %-28s |\n", f->nome);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| estado            : %-28s |\n", ficha_nome_estado(f->estado));  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| terminal actual   : %-28d |\n", f->id_terminal_actual);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| artigos no cesto  : %-28d |\n", f->quantidade_artigos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| total a pagar     : %23.2f EUR |\n", f->total_pagar);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. servico est.   : %23d s |\n", f->tempo_servico_estimado);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. chegada        : %23d s |\n", f->instante_chegada);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. inicio serv.   : %23d s |\n", f->instante_inicio_servico);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| t. saida          : %23d s |\n", f->instante_saida);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| recebeu oferta    : %-28s |\n", f->recebeu_oferta ? "sim" : "nao");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    if (f->recebeu_oferta && f->indice_oferta >= 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("| artigo oferecido  : %-22s %.2f EUR |\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
               f->cesto[f->indice_oferta].designacao,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
               f->cesto[f->indice_oferta].preco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    printf("+-----------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void ficha_imprimir_linha(const FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao ficha_imprimir_linha. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!f) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    printf("  #%-4d %-18s art=%2d  t.serv=%4ds  estado=%-11s term=%2d\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           f->id, f->nome, f->quantidade_artigos,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           f->tempo_servico_estimado, ficha_nome_estado(f->estado),  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
           f->id_terminal_actual);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void ficha_marcar_oferta(FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao ficha_marcar_oferta. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int idx;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!f || f->recebeu_oferta) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    idx = catalogo_indice_mais_caro(f->cesto, f->quantidade_artigos);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (idx < 0) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    f->recebeu_oferta = 1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->indice_oferta  = idx;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->valor_oferta   = f->cesto[idx].preco;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */
