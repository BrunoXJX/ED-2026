#include "vigilante_memoria.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Cabecalho escondido antes de cada bloco. O union forca um
 * alinhamento conservador para que o ponteiro entregue ao
 * programa fique alinhado para qualquer tipo basico.
 */
typedef union cab_bloco_u {
    struct {
        size_t bytes_uteis;
    } meta;
    long double  alinha_a;
    void        *alinha_b;
    long long    alinha_c;
} CabBloco_t;

/* Contadores estaticos. So este modulo lhes mexe. */
static size_t v_em_uso         = 0;
static size_t v_pico            = 0;
static size_t v_blocos          = 0;
static size_t v_total_pedido    = 0;
static size_t v_total_devolvido = 0;

/*
 * Funcao interna que recebe um numero de bytes uteis e devolve
 * um ponteiro pronto a usar (ja a frente do cabecalho).
 */
static void *pedir_interno(size_t bytes)
{
    CabBloco_t *cab;

    if (bytes == 0) bytes = 1;  /* nunca devolver NULL para alocacao zero */

    cab = (CabBloco_t *) malloc(sizeof(CabBloco_t) + bytes);
    if (!cab) return NULL;

    cab->meta.bytes_uteis = bytes;

    v_em_uso       += bytes;
    v_total_pedido += bytes;
    v_blocos++;
    if (v_em_uso > v_pico) v_pico = v_em_uso;

    return (void *) (cab + 1);
}

void *vigia_pedir(size_t bytes)
{
    return pedir_interno(bytes);
}

void *vigia_pedir_z(size_t bytes)
{
    void *ptr = pedir_interno(bytes);
    if (ptr) memset(ptr, 0, bytes == 0 ? 1 : bytes);
    return ptr;
}

void vigia_devolver(void *ptr)
{
    CabBloco_t *cab;
    size_t      bytes;

    if (!ptr) return;

    cab   = ((CabBloco_t *) ptr) - 1;
    bytes = cab->meta.bytes_uteis;

    if (bytes <= v_em_uso) v_em_uso -= bytes;
    else                   v_em_uso  = 0;

    v_total_devolvido += bytes;
    if (v_blocos) v_blocos--;

    free(cab);
}

char *vigia_dup(const char *texto)
{
    size_t  n;
    char   *novo;

    if (!texto) return NULL;
    n    = strlen(texto) + 1;
    novo = (char *) pedir_interno(n);
    if (novo) memcpy(novo, texto, n);
    return novo;
}

size_t vigia_em_uso         (void) { return v_em_uso;         }
size_t vigia_pico           (void) { return v_pico;            }
size_t vigia_blocos_vivos   (void) { return v_blocos;          }
size_t vigia_total_pedido   (void) { return v_total_pedido;    }
size_t vigia_total_devolvido(void) { return v_total_devolvido; }

void vigia_relatorio(void)
{
    printf("\n+-- Vigilante de Memoria -------------------------+\n");
    printf("| em uso agora    : %10lu bytes              |\n", (unsigned long) v_em_uso);
    printf("| pico atingido   : %10lu bytes              |\n", (unsigned long) v_pico);
    printf("| blocos vivos    : %10lu                    |\n", (unsigned long) v_blocos);
    printf("| total pedido    : %10lu bytes              |\n", (unsigned long) v_total_pedido);
    printf("| total devolvido : %10lu bytes              |\n", (unsigned long) v_total_devolvido);
    printf("+-------------------------------------------------+\n");
}
