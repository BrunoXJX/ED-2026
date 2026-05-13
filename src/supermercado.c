#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "terminal_caixa.h"  /* linha: Importa as declaracoes locais de terminal_caixa.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */
#include "ficha_cliente.h"  /* linha: Importa as declaracoes locais de ficha_cliente.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

void supermercado_iniciar(Supermercado_t *s, const Configuracao_t *c)  /* linha: Inicia ou continua a definicao da funcao supermercado_iniciar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    if (c) s->config = *c;  /* linha: Testa a condicao antes de executar o bloco associado. */
    else   configuracao_defeitos(&s->config);  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */

    elenco_iniciar    (&s->elenco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    indice_iniciar    (&s->indice);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    calendario_iniciar(&s->calendario);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    s->cronista.acoes     = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->cronista.motor     = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->cronista.historico = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    s->tick_actual                = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->proximo_id_ficha           = 1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->total_clientes_atendidos   = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->total_clientes_no_sistema  = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->soma_tempos_espera         = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->soma_tempos_servico        = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->total_ofertas_concedidas   = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->prejuizo_acumulado         = 0.0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

/* Liberta todas as fichas indexadas (e o que la esta dentro). */
static void libertar_fichas_via_indice(IndicePesquisa_t *i)  /* linha: Inicia ou continua a definicao da funcao libertar_fichas_via_indice. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int         k;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoIndice_t *cur, *prox;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!i) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (k = 0; k < INDICE_HASH_TAM; k++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        cur = i->baldes[k];  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        while (cur) {  /* linha: Repete ou controla o bloco com base na condicao indicada. */
            prox = cur->seguinte;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
            if (cur->ficha) ficha_destruir(cur->ficha);  /* linha: Testa a condicao antes de executar o bloco associado. */
            vigia_devolver(cur);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
            cur = prox;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
        i->baldes[k] = NULL;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    i->total = 0;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void supermercado_libertar(Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao supermercado_libertar. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    if (!s) return;  /* linha: Testa a condicao antes de executar o bloco associado. */
    /* libertar fichas primeiro (atraves do indice, que e a
       referencia mestre); os terminais perdem entao referencias
       para fichas — o que e ok, porque vamos destruir os
       terminais a seguir. */
    libertar_fichas_via_indice(&s->indice);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    elenco_libertar    (&s->elenco);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    calendario_libertar(&s->calendario);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    cronista_fechar    (&s->cronista);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void supermercado_inicializar_terminais(Supermercado_t *s, int n_caixas)  /* linha: Inicia ou continua a definicao da funcao supermercado_inicializar_terminais. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    int         i;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int         abrir_inicialmente;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Terminal_t *t;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s || n_caixas <= 0) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    abrir_inicialmente = (n_caixas < 3) ? n_caixas : 3;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    for (i = 1; i <= n_caixas; i++) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        t = terminal_criar(i);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (!t) continue;  /* linha: Testa a condicao antes de executar o bloco associado. */
        if (i <= abrir_inicialmente) terminal_abrir(t);  /* linha: Testa a condicao antes de executar o bloco associado. */
        elenco_adicionar(&s->elenco, t);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int supermercado_adicionar_ficha(Supermercado_t *s, FichaCliente_t *f)  /* linha: Inicia ou continua a definicao da funcao supermercado_adicionar_ficha. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Terminal_t *escolhido;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    if (!s || !f) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* tempo de chegada = tick actual */
    f->instante_chegada = s->tick_actual;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    /* escolher terminal aberto com menor tempo estimado */
    escolhido = elenco_melhor_para_novo(&s->elenco);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (!escolhido) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        /* nenhum terminal aberto — abre o primeiro fechado */
        escolhido = elenco_primeiro_fechado(&s->elenco);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        if (escolhido) terminal_abrir(escolhido);  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    if (!escolhido) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* A ficha so entra no indice depois de sabermos que ha uma
       caixa valida para a receber. Isto evita referencias
       penduradas se a insercao falhar a meio. */
    if (!indice_inserir(&s->indice, f)) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* enfileirar */
    if (!fila_inserir_atras(&escolhido->fila, f)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        indice_remover(&s->indice, f->nome);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    f->id_terminal_actual = escolhido->id;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    f->estado             = EST_FICHA_AGUARDA;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    s->total_clientes_no_sistema++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */

    /* agendar oferta devida */
    if (!calendario_agendar(&s->calendario,  /* linha: Testa a condicao antes de executar o bloco associado. */
                            f->instante_chegada + s->config.max_espera,  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
                            EVT_OFERTA_DEVIDA,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            escolhido,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            f)) {  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
        fila_remover_por_id(&escolhido->fila, f->id);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        indice_remover(&s->indice, f->nome);  /* linha: Acede a campos de uma estrutura atraves de ponteiro. */
        s->total_clientes_no_sistema--;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
        f->id_terminal_actual = -1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return 1;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

size_t supermercado_memoria_desperdicada(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao supermercado_memoria_desperdicada. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    size_t        total = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           obsoletos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!s) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */

    /* Terminais FECHADOS — o struct e o cabecalho da fila
       continuam vivos. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal && cur->terminal->estado == EST_TERMINAL_FECHADO) {  /* linha: Testa a condicao antes de executar o bloco associado. */
            total += sizeof(Terminal_t);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
        }  /* linha: Fecha o bloco de instrucoes atual. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    /* Eventos obsoletos no calendario (lazy deletion ainda nao
       limpou). */
    obsoletos = calendario_total_obsoletos(&s->calendario);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    total    += (size_t) obsoletos * sizeof(Evento_t);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    return total;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int supermercado_total_terminais(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao supermercado_total_terminais. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return (s) ? elenco_total(&s->elenco) : 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

int supermercado_total_clientes_em_espera(const Supermercado_t *s)  /* linha: Inicia ou continua a definicao da funcao supermercado_total_clientes_em_espera. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    NoTerminal_t *cur;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int           n = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    if (!s) return 0;  /* linha: Testa a condicao antes de executar o bloco associado. */
    for (cur = s->elenco.cabeca; cur; cur = cur->seguinte) {  /* linha: Percorre uma sequencia repetindo o bloco enquanto a condicao se mantem. */
        if (cur->terminal) n += fila_tamanho(&cur->terminal->fila);  /* linha: Testa a condicao antes de executar o bloco associado. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    return n;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
