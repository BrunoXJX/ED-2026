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
#ifndef NUCLEO_TIPOS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho NUCLEO_TIPOS_H. */
#define NUCLEO_TIPOS_H  /* linha: Define a macro ou constante simbolica NUCLEO_TIPOS_H. */

/* Tamanhos fixos partilhados pelos varios modulos. */
#define NOME_OPERADOR_MAX  32  /* linha: Define a macro ou constante simbolica NOME_OPERADOR_MAX. */
#define NOME_FICHA_MAX     32  /* linha: Define a macro ou constante simbolica NOME_FICHA_MAX. */
#define DESIGNACAO_MAX     24  /* linha: Define a macro ou constante simbolica DESIGNACAO_MAX. */
#define LINHA_BRUTA_MAX    256  /* linha: Define a macro ou constante simbolica LINHA_BRUTA_MAX. */
#define INDICE_HASH_TAM    31     /* numero primo proximo de N esperado */  /* linha: Define a macro ou constante simbolica INDICE_HASH_TAM. */

/* Estado de um terminal de caixa.
   ABERTO   - aceita novos clientes
   DRENANDO - nao aceita mais, mas atende os que ja la estavam
              (encerramento "natural" pedido pelo utilizador ou
              automatico)
   FECHADO  - nem aceita nem atende, mas a memoria do terminal
              continua viva (e contabilizada como desperdicada). */
typedef enum estado_terminal_e {  /* linha: Comeca a definicao de uma enumeracao usada como tipo do dominio. */
    EST_TERMINAL_FECHADO  = 0,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EST_TERMINAL_ABERTO   = 1,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EST_TERMINAL_DRENANDO = 2  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
} EstadoTerminal_t;  /* linha: Fecha a definicao e disponibiliza o tipo EstadoTerminal_t. */

/* Estado da ficha de um cliente. */
typedef enum estado_ficha_e {  /* linha: Comeca a definicao de uma enumeracao usada como tipo do dominio. */
    EST_FICHA_AGUARDA      = 0,  /* na fila, ainda nao chamado */  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EST_FICHA_ATENDIMENTO  = 1,  /* a ser atendido neste momento */  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EST_FICHA_PROCESSADA   = 2   /* ja saiu do supermercado */  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
} EstadoFicha_t;  /* linha: Fecha a definicao e disponibiliza o tipo EstadoFicha_t. */

/* Tipos de eventos que o calendario sabe agendar.
   FIM_ATENDIMENTO - termo previsto da operacao no terminal
   OFERTA_DEVIDA   - cliente esperou MAX_ESPERA, deve receber
                     produto oferecido (se ainda nao foi servido)
   VERIFICAR_BALANCO - lembrete periodico para o motor verificar
                       se ha terminais a abrir ou fechar */
typedef enum tipo_evento_e {  /* linha: Comeca a definicao de uma enumeracao usada como tipo do dominio. */
    EVT_FIM_ATENDIMENTO   = 0,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EVT_OFERTA_DEVIDA     = 1,  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    EVT_VERIFICAR_BALANCO = 2  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
} TipoEvento_t;  /* linha: Fecha a definicao e disponibiliza o tipo TipoEvento_t. */

/* Forward declarations. As estruturas reais sao definidas
   nos modulos que as possuem. */
typedef struct configuracao_s     Configuracao_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct artigo_s           Artigo_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct ficha_cliente_s    FichaCliente_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct no_fila_s          NoFila_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct fila_atendimento_s FilaAtendimento_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct terminal_s         Terminal_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct no_terminal_s      NoTerminal_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct elenco_terminais_s ElencoTerminais_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct no_indice_s        NoIndice_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct indice_pesquisa_s  IndicePesquisa_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct evento_s           Evento_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct calendario_s       Calendario_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct cronista_s         Cronista_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */
typedef struct supermercado_s     Supermercado_t;  /* linha: Comeca a definicao de uma estrutura com alias typedef. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
