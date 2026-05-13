/*
 * supermercado.h
 * --------------
 * Estado global do supermercado. Agrega:
 *   - configuracao
 *   - elenco de terminais
 *   - indice de pesquisa
 *   - calendario de eventos
 *   - cronista (logs)
 *   - tick actual da simulacao
 *   - metricas globais incrementais
 *
 * Esta estrutura nao tem logica propria — ela e o "estado" e
 * o motor_simulacao e a interface_consola e que mexem nela.
 */
#ifndef SUPERMERCADO_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho SUPERMERCADO_H. */
#define SUPERMERCADO_H  /* linha: Define a macro ou constante simbolica SUPERMERCADO_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */
#include "configuracao.h"  /* linha: Importa as declaracoes locais de configuracao.h usadas neste modulo. */
#include "elenco_terminais.h"  /* linha: Importa as declaracoes locais de elenco_terminais.h usadas neste modulo. */
#include "indice_pesquisa.h"  /* linha: Importa as declaracoes locais de indice_pesquisa.h usadas neste modulo. */
#include "calendario_eventos.h"  /* linha: Importa as declaracoes locais de calendario_eventos.h usadas neste modulo. */
#include "cronista.h"  /* linha: Importa as declaracoes locais de cronista.h usadas neste modulo. */

struct supermercado_s {  /* linha: Comeca a estrutura supermercado_s e agrupa os seus campos. */
    Configuracao_t    config;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    ElencoTerminais_t elenco;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    IndicePesquisa_t  indice;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Calendario_t      calendario;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Cronista_t        cronista;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    int               tick_actual;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int               proximo_id_ficha;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    /* Metricas incrementais. */
    int   total_clientes_atendidos;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int   total_clientes_no_sistema;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    long  soma_tempos_espera;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    long  soma_tempos_servico;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int   total_ofertas_concedidas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    double prejuizo_acumulado;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Inicializa um supermercado vazio com a config dada. */
void supermercado_iniciar(Supermercado_t *s, const Configuracao_t *c);  /* linha: Declara a funcao supermercado_iniciar para uso por outros modulos. */

/* Liberta toda a memoria associada (terminais, fichas no
   indice, calendario, fecha o cronista). */
void supermercado_libertar(Supermercado_t *s);  /* linha: Declara a funcao supermercado_libertar para uso por outros modulos. */

/* Cria @n_caixas terminais e abre os primeiros min(n,3). */
void supermercado_inicializar_terminais(Supermercado_t *s, int n_caixas);  /* linha: Declara a funcao supermercado_inicializar_terminais para uso por outros modulos. */

/* Adiciona uma ficha vinda do leitor de dados ao sistema:
   - inscreve no indice
   - distribui para o melhor terminal aberto
   - agenda OFERTA_DEVIDA na ficha
   - se for o primeiro a ser atendido naquele terminal, o
     motor_simulacao e que decide quando comecar (proximo tick) */
int  supermercado_adicionar_ficha(Supermercado_t *s, FichaCliente_t *f);  /* linha: Declara a funcao supermercado_adicionar_ficha para uso por outros modulos. */

/* Memoria desperdicada calculada dinamicamente:
   - tamanho dos terminais FECHADOS (mais a sua fila/header)
   - eventos obsoletos no calendario
   Devolve em bytes. */
size_t supermercado_memoria_desperdicada(const Supermercado_t *s);  /* linha: Declara a funcao supermercado_memoria_desperdicada para uso por outros modulos. */

/* Helpers. */
int  supermercado_total_terminais     (const Supermercado_t *s);  /* linha: Declara a funcao supermercado_total_terminais para uso por outros modulos. */
int  supermercado_total_clientes_em_espera(const Supermercado_t *s);  /* linha: Declara a funcao supermercado_total_clientes_em_espera para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
