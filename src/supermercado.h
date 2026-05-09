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
#ifndef SUPERMERCADO_H
#define SUPERMERCADO_H

#include "nucleo_tipos.h"
#include "configuracao.h"
#include "elenco_terminais.h"
#include "indice_pesquisa.h"
#include "calendario_eventos.h"
#include "cronista.h"

struct supermercado_s {
    Configuracao_t    config;
    ElencoTerminais_t elenco;
    IndicePesquisa_t  indice;
    Calendario_t      calendario;
    Cronista_t        cronista;

    int               tick_actual;
    int               proximo_id_ficha;

    /* Metricas incrementais. */
    int   total_clientes_atendidos;
    int   total_clientes_no_sistema;
    long  soma_tempos_espera;
    long  soma_tempos_servico;
    int   total_ofertas_concedidas;
    double prejuizo_acumulado;
};

/* Inicializa um supermercado vazio com a config dada. */
void supermercado_iniciar(Supermercado_t *s, const Configuracao_t *c);

/* Liberta toda a memoria associada (terminais, fichas no
   indice, calendario, fecha o cronista). */
void supermercado_libertar(Supermercado_t *s);

/* Cria @n_caixas terminais e abre os primeiros min(n,3). */
void supermercado_inicializar_terminais(Supermercado_t *s, int n_caixas);

/* Adiciona uma ficha vinda do leitor de dados ao sistema:
   - inscreve no indice
   - distribui para o melhor terminal aberto
   - agenda OFERTA_DEVIDA na ficha
   - se for o primeiro a ser atendido naquele terminal, o
     motor_simulacao e que decide quando comecar (proximo tick) */
int  supermercado_adicionar_ficha(Supermercado_t *s, FichaCliente_t *f);

/* Memoria desperdicada calculada dinamicamente:
   - tamanho dos terminais FECHADOS (mais a sua fila/header)
   - eventos obsoletos no calendario
   Devolve em bytes. */
size_t supermercado_memoria_desperdicada(const Supermercado_t *s);

/* Helpers. */
int  supermercado_total_terminais     (const Supermercado_t *s);
int  supermercado_total_clientes_em_espera(const Supermercado_t *s);

#endif
