/*
 * vigilante_memoria.h
 * -------------------
 * Wrapper de alocacao/libertacao com contadores proprios.
 *
 * A ideia: cada bloco entregue ao programa traz um pequeno
 * "cabecalho escondido" antes do ponteiro visivel. O cabecalho
 * guarda o tamanho pedido. Quando o programa devolve o bloco,
 * o vigilante recua sizeof(CabBloco_t), le o tamanho e
 * actualiza os contadores. Isto evita ter de andar a passar
 * o tamanho a cada chamada de _devolver, ao mesmo tempo que
 * permite contabilizar todos os bytes em uso sem depender da
 * libc para isso.
 *
 * Memoria desperdicada NAO e marcada por aqui — e calculada
 * dinamicamente pelo modulo supermercado, percorrendo o estado
 * (terminais fechados, eventos obsoletos, baldes do indice).
 */
#ifndef VIGILANTE_MEMORIA_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho VIGILANTE_MEMORIA_H. */
#define VIGILANTE_MEMORIA_H  /* linha: Define a macro ou constante simbolica VIGILANTE_MEMORIA_H. */

#include <stddef.h>  /* linha: Importa a biblioteca padrao stddef.h necessaria neste modulo. */

/* Aloca @bytes uteis. Devolve NULL em falha. */
void *vigia_pedir   (size_t bytes);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Aloca @bytes uteis e poe a zero. */
void *vigia_pedir_z (size_t bytes);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Devolve um bloco antes obtido com vigia_pedir/_z/_dup. */
void  vigia_devolver(void *ptr);  /* linha: Declara a funcao vigia_devolver para uso por outros modulos. */

/* Duplica uma string. */
char *vigia_dup     (const char *texto);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Bytes actualmente em uso (entregues e ainda nao devolvidos). */
size_t vigia_em_uso       (void);  /* linha: Declara a funcao vigia_em_uso para uso por outros modulos. */

/* Pico maximo atingido durante a execucao. */
size_t vigia_pico         (void);  /* linha: Declara a funcao vigia_pico para uso por outros modulos. */

/* Numero de blocos vivos. */
size_t vigia_blocos_vivos (void);  /* linha: Declara a funcao vigia_blocos_vivos para uso por outros modulos. */

/* Total de bytes ja pedidos (acumulado, nunca diminui). */
size_t vigia_total_pedido (void);  /* linha: Declara a funcao vigia_total_pedido para uso por outros modulos. */

/* Total de bytes ja libertados (acumulado). */
size_t vigia_total_devolvido (void);  /* linha: Declara a funcao vigia_total_devolvido para uso por outros modulos. */

/* Imprime um pequeno relatorio para o stdout. */
void   vigia_relatorio    (void);  /* linha: Declara a funcao vigia_relatorio para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
