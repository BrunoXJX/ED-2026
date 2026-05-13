/*
 * motor_simulacao.h
 * -----------------
 * Motor que avanca a simulacao por ticks consultando o
 * calendario de eventos.
 *
 * Estrategia (a defender oralmente):
 *   1. Cada chamada a motor_passo() avanca um tick.
 *   2. Antes de incrementar o tick, processa todos os eventos
 *      do calendario cujo instante <= tick_actual.
 *   3. Para cada terminal aberto sem ficha em atendimento mas
 *      com fila, o motor "puxa" a primeira ficha para
 *      atendimento e agenda o seu fim.
 *   4. Periodicamente (de acordo com config.tick_verificacao),
 *      verifica abertura/fecho automatico de terminais.
 *   5. motor_correr_ate_estabilizar() avanca enquanto houver
 *      eventos pendentes ou clientes para servir.
 */
#ifndef MOTOR_SIMULACAO_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho MOTOR_SIMULACAO_H. */
#define MOTOR_SIMULACAO_H  /* linha: Define a macro ou constante simbolica MOTOR_SIMULACAO_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

/* Avanca exactamente um tick. */
void motor_passo(Supermercado_t *s);  /* linha: Declara a funcao motor_passo para uso por outros modulos. */

/* Avanca @n_ticks. */
void motor_correr_n_ticks(Supermercado_t *s, int n);  /* linha: Declara a funcao motor_correr_n_ticks para uso por outros modulos. */

/* Avanca enquanto houver eventos pendentes ou clientes em fila. */
void motor_correr_ate_estabilizar(Supermercado_t *s);  /* linha: Declara a funcao motor_correr_ate_estabilizar para uso por outros modulos. */

/* Verifica abertura/fecho automatico de terminais. Pode ser
   chamada manualmente atraves do menu. */
void motor_verificar_balanco(Supermercado_t *s);  /* linha: Declara a funcao motor_verificar_balanco para uso por outros modulos. */

/* Politica manual: abre o primeiro terminal fechado. */
int  motor_abrir_terminal_manual(Supermercado_t *s, int id_ou_zero);  /* linha: Declara a funcao motor_abrir_terminal_manual para uso por outros modulos. */

/* Politica manual: drena (fecha gentilmente) o terminal indicado. */
int  motor_drenar_terminal_manual(Supermercado_t *s, int id);  /* linha: Declara a funcao motor_drenar_terminal_manual para uso por outros modulos. */

/* Politica manual: move uma ficha de um terminal para outro. */
int  motor_mover_ficha(Supermercado_t *s, int id_ficha, int id_destino);  /* linha: Declara a funcao motor_mover_ficha para uso por outros modulos. */

/* Distribui clientes do terminal @id_origem por outros abertos.
   Util quando se quer despachar manualmente. */
int  motor_redistribuir(Supermercado_t *s, int id_origem);  /* linha: Declara a funcao motor_redistribuir para uso por outros modulos. */

/* Fecha uma caixa em modo imediato: a fila de espera e
   redistribuida pelas restantes caixas abertas. Se existir
   cliente em atendimento, a caixa fica a drenar ate terminar.
   Devolve o numero de fichas movidas, ou -1 se falhar. */
int  motor_fechar_redistribuindo(Supermercado_t *s, int id_terminal);  /* linha: Declara a funcao motor_fechar_redistribuindo para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
