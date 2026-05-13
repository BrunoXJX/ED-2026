/*
 * terminal_caixa.h
 * ----------------
 * Terminal de caixa. Cada terminal tem:
 *
 *   - identidade (id, nome do operador gerado deterministicamente)
 *   - estado (FECHADO, ABERTO, DRENANDO)
 *   - fila de atendimento
 *   - eventual ficha em atendimento
 *   - contadores incrementais (clientes atendidos, artigos
 *     processados, tempo total de espera, tempo total de
 *     servico, ofertas concedidas, prejuizo)
 *
 * O terminal nao manda em si proprio: as transicoes sao feitas
 * pelo motor de simulacao ou pela interface.
 */
#ifndef TERMINAL_CAIXA_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho TERMINAL_CAIXA_H. */
#define TERMINAL_CAIXA_H  /* linha: Define a macro ou constante simbolica TERMINAL_CAIXA_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */
#include "fila_atendimento.h"  /* linha: Importa as declaracoes locais de fila_atendimento.h usadas neste modulo. */

/* No da lista de clientes ja atendidos por este terminal. */
typedef struct no_atendido_s {  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
    int    id;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char   nome[NOME_FICHA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int    quantidade_artigos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double total_pago;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int    recebeu_oferta;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    struct no_atendido_s *seguinte;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
} NoAtendido_t;  /* linha: Fecha a definicao e disponibiliza o tipo NoAtendido_t. */

struct terminal_s {  /* linha: Comeca a estrutura terminal_s e agrupa os seus campos. */
    int               id;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char              nome_operador[NOME_OPERADOR_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    EstadoTerminal_t  estado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    FilaAtendimento_t fila;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    FichaCliente_t   *em_atendimento;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    /* Contadores locais (sao incrementais — actualizados
       sempre que o motor processa um evento, nunca recalculados). */
    int               total_atendidos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    long              total_artigos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    long              tempo_total_espera;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    long              tempo_total_servico;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int               total_ofertas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double            prejuizo_total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    /* Lista ligada dos clientes ja atendidos (historico). */
    NoAtendido_t     *lista_atendidos;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int               total_na_lista_atendidos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Cria um novo terminal no heap. Comeca FECHADO. */
Terminal_t *terminal_criar(int id);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Liberta um terminal. Liberta tambem a sua fila (mas nao as
   fichas — a propriedade das fichas e do gestor do supermercado). */
void  terminal_destruir(Terminal_t *t);  /* linha: Declara a funcao terminal_destruir para uso por outros modulos. */

/* Transicoes de estado. */
void  terminal_abrir   (Terminal_t *t);  /* linha: Declara a funcao terminal_abrir para uso por outros modulos. */
void  terminal_drenar  (Terminal_t *t);   /* deixar de aceitar */  /* linha: Inicia ou continua a definicao da funcao terminal_drenar. */
void  terminal_fechar  (Terminal_t *t);   /* fechamento total  */  /* linha: Inicia ou continua a definicao da funcao terminal_fechar. */

/* Helpers de leitura. */
int   terminal_total_pessoas(const Terminal_t *t); /* fila + em atendimento */  /* linha: Inicia ou continua a definicao da funcao terminal_total_pessoas. */
long  terminal_tempo_estimado_total(const Terminal_t *t);  /* linha: Declara a funcao terminal_tempo_estimado_total para uso por outros modulos. */
const char *terminal_nome_estado(const Terminal_t *t);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Mostra o terminal no ecra. */
void  terminal_imprimir_resumo(const Terminal_t *t);  /* linha: Declara a funcao terminal_imprimir_resumo para uso por outros modulos. */
void  terminal_imprimir_detalhe(const Terminal_t *t);  /* linha: Declara a funcao terminal_imprimir_detalhe para uso por outros modulos. */

/* Gera nomes deterministicos de operador. So depende do id. */
void  terminal_gerar_nome_operador(int id, char *destino, int max);  /* linha: Declara a funcao terminal_gerar_nome_operador para uso por outros modulos. */

/* Regista uma copia da ficha na lista de atendidos. */
void  terminal_registar_atendido(Terminal_t *t, const FichaCliente_t *f);  /* linha: Declara a funcao terminal_registar_atendido para uso por outros modulos. */

/* Imprime a lista de clientes ja atendidos por este terminal. */
void  terminal_imprimir_atendidos(const Terminal_t *t);  /* linha: Declara a funcao terminal_imprimir_atendidos para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
