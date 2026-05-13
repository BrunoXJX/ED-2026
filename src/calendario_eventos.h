/*
 * calendario_eventos.h
 * --------------------
 * Calendario de eventos pendentes da simulacao. E uma lista
 * ligada simples mantida ORDENADA por instante crescente.
 *
 * Cada evento contem um instante (em ticks de simulacao), um
 * tipo, e referencias para o terminal e/ou ficha envolvidos.
 * O campo "versao_ficha" permite a tecnica de lazy deletion:
 * o motor compara a versao do evento com a versao actual da
 * ficha; se forem diferentes, o evento esta obsoleto e e
 * descartado em silencio.
 *
 * Esta abordagem evita ter de remover eventos do meio da lista
 * sempre que uma ficha muda de fila ou recebe acao directa.
 */
#ifndef CALENDARIO_EVENTOS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho CALENDARIO_EVENTOS_H. */
#define CALENDARIO_EVENTOS_H  /* linha: Define a macro ou constante simbolica CALENDARIO_EVENTOS_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct evento_s {  /* linha: Comeca a estrutura evento_s e agrupa os seus campos. */
    int             instante;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    TipoEvento_t    tipo;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Terminal_t     *terminal;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int             versao_ficha;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Evento_t       *seguinte;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
};  /* linha: Fecha a declaracao composta atual. */

struct calendario_s {  /* linha: Comeca a estrutura calendario_s e agrupa os seus campos. */
    Evento_t *cabeca;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int       total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

void calendario_iniciar (Calendario_t *c);  /* linha: Declara a funcao calendario_iniciar para uso por outros modulos. */
void calendario_libertar(Calendario_t *c);  /* linha: Declara a funcao calendario_libertar para uso por outros modulos. */

/* Agenda um novo evento. Insere ordenado por instante. */
int  calendario_agendar(Calendario_t   *c,  /* linha: Inicia ou continua a definicao da funcao calendario_agendar. */
                        int             instante,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                        TipoEvento_t    tipo,  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
                        Terminal_t     *terminal,  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
                        FichaCliente_t *ficha);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Devolve o instante do proximo evento (ou -1 se vazio). */
int  calendario_proximo_instante(const Calendario_t *c);  /* linha: Declara a funcao calendario_proximo_instante para uso por outros modulos. */

/* Retira e devolve o evento mais proximo. NULL se vazio. */
Evento_t *calendario_extrair_proximo(Calendario_t *c);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Liberta um evento devolvido por _extrair_proximo. */
void      calendario_libertar_evento(Evento_t *e);  /* linha: Declara a funcao calendario_libertar_evento para uso por outros modulos. */

/* Numero de eventos pendentes (incluindo obsoletos). */
int       calendario_total(const Calendario_t *c);  /* linha: Declara a funcao calendario_total para uso por outros modulos. */

/* Conta quantos eventos estao obsoletos (versao desactualizada).
   Util para a contagem de memoria desperdicada. */
int       calendario_total_obsoletos(const Calendario_t *c);  /* linha: Declara a funcao calendario_total_obsoletos para uso por outros modulos. */

/* Imprime o calendario para depuracao. */
void      calendario_imprimir(const Calendario_t *c);  /* linha: Declara a funcao calendario_imprimir para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
