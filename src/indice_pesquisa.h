/*
 * indice_pesquisa.h
 * -----------------
 * Tabela hash com encadeamento separado para fichas. Mantida
 * em paralelo com o estado real do supermercado: sempre que
 * uma ficha entra no sistema, e indexada; quando sai (e
 * libertada), e removida.
 *
 * A chave e o nome da ficha. A funcao de hash usada e a sdbm
 * fornecida pelo modulo catalogo_artigos.
 *
 * Tamanho da tabela: INDICE_HASH_TAM (primo).
 */
#ifndef INDICE_PESQUISA_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho INDICE_PESQUISA_H. */
#define INDICE_PESQUISA_H  /* linha: Define a macro ou constante simbolica INDICE_PESQUISA_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

struct no_indice_s {  /* linha: Comeca a estrutura no_indice_s e agrupa os seus campos. */
    FichaCliente_t *ficha;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    NoIndice_t     *seguinte;  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
};  /* linha: Fecha a declaracao composta atual. */

struct indice_pesquisa_s {  /* linha: Comeca a estrutura indice_pesquisa_s e agrupa os seus campos. */
    NoIndice_t *baldes[INDICE_HASH_TAM];  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    int         total;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

void            indice_iniciar (IndicePesquisa_t *i);  /* linha: Declara a funcao indice_iniciar para uso por outros modulos. */
void            indice_libertar(IndicePesquisa_t *i);  /* linha: Declara a funcao indice_libertar para uso por outros modulos. */

/* Insere a ficha. Devolve 1 se ok. */
int             indice_inserir (IndicePesquisa_t *i, FichaCliente_t *f);  /* linha: Declara a funcao indice_inserir para uso por outros modulos. */

/* Remove a ficha pelo nome. Devolve 1 se removeu. */
int             indice_remover (IndicePesquisa_t *i, const char *nome);  /* linha: Declara a funcao indice_remover para uso por outros modulos. */

/* Procura por nome. NULL se nao existir. */
FichaCliente_t *indice_procurar(const IndicePesquisa_t *i, const char *nome);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

/* Numero de fichas indexadas. */
int             indice_total   (const IndicePesquisa_t *i);  /* linha: Declara a funcao indice_total para uso por outros modulos. */

/* Estatistica simples: distribuicao por baldes. */
void            indice_imprimir_estatistica(const IndicePesquisa_t *i);  /* linha: Declara a funcao indice_imprimir_estatistica para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
