/*
 * ficha_cliente.h
 * ---------------
 * Ficha individual de um cliente. O nome "ficha" e proposital
 * — num supermercado real cada cliente fidelizado tem uma
 * ficha. A estrutura agrupa tudo o que precisamos saber sobre
 * a sua passagem pelo sistema:
 *
 *   - identificacao (id e nome)
 *   - cesto de artigos (gerado pelo modulo catalogo)
 *   - tempos (entrada, inicio de servico, saida)
 *   - terminal onde se encontra
 *   - estado (aguarda, atendimento, processada)
 *   - flag de oferta e indice do produto oferecido
 *
 * O campo "versao" serve a tecnica de lazy deletion no
 * calendario de eventos: cada vez que a ficha muda de fila ou
 * recebe acao directa do utilizador, a versao incrementa, e
 * eventos antigos no calendario com versao diferente sao
 * descartados em silencio.
 */
#ifndef FICHA_CLIENTE_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho FICHA_CLIENTE_H. */
#define FICHA_CLIENTE_H  /* linha: Define a macro ou constante simbolica FICHA_CLIENTE_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct ficha_cliente_s {  /* linha: Comeca a estrutura ficha_cliente_s e agrupa os seus campos. */
    int            id;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char           nome[NOME_FICHA_MAX];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    int            quantidade_artigos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Artigo_t      *cesto;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    double         total_pagar;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    int            instante_chegada;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            instante_inicio_servico;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            instante_saida;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            tempo_servico_estimado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    int            id_terminal_actual;     /* -1 se nenhum  */  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    EstadoFicha_t  estado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    int            recebeu_oferta;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            indice_oferta;          /* -1 se nenhum  */  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double         valor_oferta;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    int            versao;                 /* lazy deletion */  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Cria uma ficha completa, gerando o cesto. */
FichaCliente_t *ficha_criar(int         id,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            const char *nome,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                            int         qtd_artigos,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                            int         tempo_produto,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                            int         max_preco_int);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Liberta uma ficha (incluindo o cesto). */
void ficha_destruir(FichaCliente_t *f);  /* linha: Declara a funcao ficha_destruir para uso por outros modulos. */

/* Mostra detalhes da ficha no ecra. */
void ficha_imprimir_detalhe(const FichaCliente_t *f);  /* linha: Declara a funcao ficha_imprimir_detalhe para uso por outros modulos. */

/* Mostra resumo (uma linha). */
void ficha_imprimir_linha(const FichaCliente_t *f);  /* linha: Declara a funcao ficha_imprimir_linha para uso por outros modulos. */

/* Marca uma oferta para a ficha. Se ja recebeu, nao faz nada. */
void ficha_marcar_oferta(FichaCliente_t *f);  /* linha: Declara a funcao ficha_marcar_oferta para uso por outros modulos. */

/* Helper: nome textual do estado. */
const char *ficha_nome_estado(EstadoFicha_t e);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
