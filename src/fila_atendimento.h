/*
 * fila_atendimento.h
 * ------------------
 * Fila ligada simples de fichas de cliente. Cada terminal de
 * caixa tem a sua fila. Implementacao com cabeca e cauda para
 * que enfileirar fique em O(1) e desenfileirar tambem.
 *
 * Os nos guardam apenas um ponteiro para a FichaCliente_t —
 * a ficha em si nao e propriedade da fila. A fila pode ser
 * "esvaziada" sem libertar as fichas (util quando uma ficha
 * muda de terminal).
 */
#ifndef FILA_ATENDIMENTO_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho FILA_ATENDIMENTO_H. */
#define FILA_ATENDIMENTO_H  /* linha: Define a macro ou constante simbolica FILA_ATENDIMENTO_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct no_fila_s {  /* linha: Comeca a estrutura no_fila_s e agrupa os seus campos. */
    FichaCliente_t  *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    NoFila_t        *seguinte;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
};  /* linha: Fecha a declaracao composta atual. */

struct fila_atendimento_s {  /* linha: Comeca a estrutura fila_atendimento_s e agrupa os seus campos. */
    NoFila_t *frente;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    NoFila_t *tras;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int       total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Inicializa uma fila vazia (a estrutura ja tem que existir). */
void fila_iniciar(FilaAtendimento_t *f);  /* linha: Declara a funcao fila_iniciar para uso por outros modulos. */

/* Numero de fichas na fila. */
int  fila_tamanho(const FilaAtendimento_t *f);  /* linha: Declara a funcao fila_tamanho para uso por outros modulos. */

/* Adiciona uma ficha ao fim da fila. Devolve 1 se ok. */
int  fila_inserir_atras(FilaAtendimento_t *f, FichaCliente_t *ficha);  /* linha: Declara a funcao fila_inserir_atras para uso por outros modulos. */

/* Adiciona uma ficha no inicio (usado para reposicao). */
int  fila_inserir_frente(FilaAtendimento_t *f, FichaCliente_t *ficha);  /* linha: Declara a funcao fila_inserir_frente para uso por outros modulos. */

/* Remove e devolve a ficha da frente. NULL se vazia. */
FichaCliente_t *fila_extrair_frente(FilaAtendimento_t *f);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Espreita a ficha da frente sem a remover. */
FichaCliente_t *fila_espreitar_frente(const FilaAtendimento_t *f);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Remove uma ficha especifica da fila pelo id. Devolve a
   ficha removida ou NULL se nao foi encontrada. Util para
   movimentar entre terminais. */
FichaCliente_t *fila_remover_por_id(FilaAtendimento_t *f, int id_ficha);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Soma dos tempos de servico estimados de todas as fichas
   da fila. Util para a politica de balanceamento por menor
   tempo de espera. */
long fila_tempo_estimado_total(const FilaAtendimento_t *f);  /* linha: Declara a funcao fila_tempo_estimado_total para uso por outros modulos. */

/* Liberta todos os nos (mas nao as fichas). */
void fila_limpar(FilaAtendimento_t *f);  /* linha: Declara a funcao fila_limpar para uso por outros modulos. */

/* Imprime resumo da fila. */
void fila_imprimir(const FilaAtendimento_t *f);  /* linha: Declara a funcao fila_imprimir para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
