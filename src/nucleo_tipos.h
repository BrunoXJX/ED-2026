/*
 * nucleo_tipos.h
 * --------------
 * Tipos partilhados por todo o sistema. So entram aqui as
 * declaracoes "leves": enumeracoes, forward declarations e
 * constantes simbolicas. Os structs reais ficam nos modulos
 * que os possuem.
 *
 * Manter este ficheiro pequeno evita inclusoes circulares e
 * permite que cada modulo peca apenas o que precisa.
 */
#ifndef NUCLEO_TIPOS_H
#define NUCLEO_TIPOS_H

/* Tamanhos fixos partilhados pelos varios modulos. */
#define NOME_OPERADOR_MAX  32
#define NOME_FICHA_MAX     32
#define DESIGNACAO_MAX     24
#define LINHA_BRUTA_MAX    256
#define INDICE_HASH_TAM    31     /* numero primo proximo de N esperado */

/* Estado de um terminal de caixa.
   ABERTO   - aceita novos clientes
   DRENANDO - nao aceita mais, mas atende os que ja la estavam
              (encerramento "natural" pedido pelo utilizador ou
              automatico)
   FECHADO  - nem aceita nem atende, mas a memoria do terminal
              continua viva (e contabilizada como desperdicada). */
typedef enum estado_terminal_e {
    EST_TERMINAL_FECHADO  = 0,
    EST_TERMINAL_ABERTO   = 1,
    EST_TERMINAL_DRENANDO = 2
} EstadoTerminal_t;

/* Estado da ficha de um cliente. */
typedef enum estado_ficha_e {
    EST_FICHA_AGUARDA      = 0,  /* na fila, ainda nao chamado */
    EST_FICHA_ATENDIMENTO  = 1,  /* a ser atendido neste momento */
    EST_FICHA_PROCESSADA   = 2   /* ja saiu do supermercado */
} EstadoFicha_t;

/* Tipos de eventos que o calendario sabe agendar.
   FIM_ATENDIMENTO - termo previsto da operacao no terminal
   OFERTA_DEVIDA   - cliente esperou MAX_ESPERA, deve receber
                     produto oferecido (se ainda nao foi servido)
   VERIFICAR_BALANCO - lembrete periodico para o motor verificar
                       se ha terminais a abrir ou fechar */
typedef enum tipo_evento_e {
    EVT_FIM_ATENDIMENTO   = 0,
    EVT_OFERTA_DEVIDA     = 1,
    EVT_VERIFICAR_BALANCO = 2
} TipoEvento_t;

/* Forward declarations. As estruturas reais sao definidas
   nos modulos que as possuem. */
typedef struct configuracao_s     Configuracao_t;
typedef struct artigo_s           Artigo_t;
typedef struct ficha_cliente_s    FichaCliente_t;
typedef struct no_fila_s          NoFila_t;
typedef struct fila_atendimento_s FilaAtendimento_t;
typedef struct terminal_s         Terminal_t;
typedef struct no_terminal_s      NoTerminal_t;
typedef struct elenco_terminais_s ElencoTerminais_t;
typedef struct no_indice_s        NoIndice_t;
typedef struct indice_pesquisa_s  IndicePesquisa_t;
typedef struct evento_s           Evento_t;
typedef struct calendario_s       Calendario_t;
typedef struct cronista_s         Cronista_t;
typedef struct supermercado_s     Supermercado_t;

#endif
