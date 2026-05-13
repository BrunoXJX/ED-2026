#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */
#include <stdlib.h>  /* linha: Importa a biblioteca padrao stdlib.h necessaria neste modulo. */
#include <string.h>  /* linha: Importa a biblioteca padrao string.h necessaria neste modulo. */

/*
 * Cabecalho escondido antes de cada bloco. O union forca um
 * alinhamento conservador para que o ponteiro entregue ao
 * programa fique alinhado para qualquer tipo basico.
 */
typedef union cab_bloco_u {  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
    struct {  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
        size_t bytes_uteis;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    } meta;  /* linha: Fecha a definicao e disponibiliza o tipo meta. */
    long double  alinha_a;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    void        *alinha_b;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    long long    alinha_c;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
} CabBloco_t;  /* linha: Fecha a definicao e disponibiliza o tipo CabBloco_t. */

/* Contadores estaticos. So este modulo lhes mexe. */
static size_t v_em_uso         = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
static size_t v_pico            = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
static size_t v_blocos          = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
static size_t v_total_pedido    = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
static size_t v_total_devolvido = 0;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

/*
 * Funcao interna que recebe um numero de bytes uteis e devolve
 * um ponteiro pronto a usar (ja a frente do cabecalho).
 */
static void *pedir_interno(size_t bytes)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    CabBloco_t *cab;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    if (bytes == 0) bytes = 1;  /* nunca devolver NULL para alocacao zero */  /* linha: Testa a condicao antes de executar o bloco associado. */

    cab = (CabBloco_t *) malloc(sizeof(CabBloco_t) + bytes);  /* linha: Pede memoria dinamica ao sistema. */
    if (!cab) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */

    cab->meta.bytes_uteis = bytes;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    v_em_uso       += bytes;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    v_total_pedido += bytes;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    v_blocos++;  /* linha: Atualiza o contador ou ponteiro usado pela logica desta rotina. */
    if (v_em_uso > v_pico) v_pico = v_em_uso;  /* linha: Testa a condicao antes de executar o bloco associado. */

    return (void *) (cab + 1);  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void *vigia_pedir(size_t bytes)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    return pedir_interno(bytes);  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void *vigia_pedir_z(size_t bytes)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    void *ptr = pedir_interno(bytes);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (ptr) memset(ptr, 0, bytes == 0 ? 1 : bytes);  /* linha: Testa a condicao antes de executar o bloco associado. */
    return ptr;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

void vigia_devolver(void *ptr)  /* linha: Inicia ou continua a definicao da funcao vigia_devolver. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    CabBloco_t *cab;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    size_t      bytes;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */

    if (!ptr) return;  /* linha: Testa a condicao antes de executar o bloco associado. */

    cab   = ((CabBloco_t *) ptr) - 1;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    bytes = cab->meta.bytes_uteis;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (bytes <= v_em_uso) v_em_uso -= bytes;  /* linha: Testa a condicao antes de executar o bloco associado. */
    else                   v_em_uso  = 0;  /* linha: Executa o ramo alternativo quando a condicao anterior falhou. */

    v_total_devolvido += bytes;  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (v_blocos) v_blocos--;  /* linha: Testa a condicao antes de executar o bloco associado. */

    free(cab);  /* linha: Devolve memoria dinamica ao sistema. */
}  /* linha: Fecha o bloco de instrucoes atual. */

char *vigia_dup(const char *texto)  /* linha: Mantem a estrutura sintatica ou os dados necessarios neste ponto do codigo. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    size_t  n;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    char   *novo;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    if (!texto) return NULL;  /* linha: Testa a condicao antes de executar o bloco associado. */
    n    = strlen(texto) + 1;  /* linha: Calcula o comprimento da string usada nesta instrucao. */
    novo = (char *) pedir_interno(n);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    if (novo) memcpy(novo, texto, n);  /* linha: Testa a condicao antes de executar o bloco associado. */
    return novo;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */

size_t vigia_em_uso         (void) { return v_em_uso;         }  /* linha: Inicia ou continua a definicao da funcao vigia_em_uso. */
size_t vigia_pico           (void) { return v_pico;            }  /* linha: Inicia ou continua a definicao da funcao vigia_pico. */
size_t vigia_blocos_vivos   (void) { return v_blocos;          }  /* linha: Inicia ou continua a definicao da funcao vigia_blocos_vivos. */
size_t vigia_total_pedido   (void) { return v_total_pedido;    }  /* linha: Inicia ou continua a definicao da funcao vigia_total_pedido. */
size_t vigia_total_devolvido(void) { return v_total_devolvido; }  /* linha: Inicia ou continua a definicao da funcao vigia_total_devolvido. */

void vigia_relatorio(void)  /* linha: Inicia ou continua a definicao da funcao vigia_relatorio. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    printf("\n+-- Vigilante de Memoria -------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| em uso agora    : %10lu bytes              |\n", (unsigned long) v_em_uso);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| pico atingido   : %10lu bytes              |\n", (unsigned long) v_pico);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| blocos vivos    : %10lu                    |\n", (unsigned long) v_blocos);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| total pedido    : %10lu bytes              |\n", (unsigned long) v_total_pedido);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("| total devolvido : %10lu bytes              |\n", (unsigned long) v_total_devolvido);  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("+-------------------------------------------------+\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
