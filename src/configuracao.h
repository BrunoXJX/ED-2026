/*
 * configuracao.h
 * --------------
 * Tabela de parametros do supermercado. E carregada do
 * ficheiro Configuracao.txt no arranque ou preenchida com
 * os valores por defeito.
 *
 * Os nomes seguem o enunciado: MAX_ESPERA, N_CAIXAS, etc.
 */
#ifndef CONFIGURACAO_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho CONFIGURACAO_H. */
#define CONFIGURACAO_H  /* linha: Define a macro ou constante simbolica CONFIGURACAO_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

/* Valores por defeito vindos do enunciado. */
#define D_MAX_ESPERA       120  /* linha: Define a macro ou constante simbolica D_MAX_ESPERA. */
#define D_N_CAIXAS         6  /* linha: Define a macro ou constante simbolica D_N_CAIXAS. */
#define D_TEMPO_PRODUTO    6  /* linha: Define a macro ou constante simbolica D_TEMPO_PRODUTO. */
#define D_MAX_PRECO        40  /* linha: Define a macro ou constante simbolica D_MAX_PRECO. */
#define D_MAX_FILA         7  /* linha: Define a macro ou constante simbolica D_MAX_FILA. */
#define D_MIN_FILA         3  /* linha: Define a macro ou constante simbolica D_MIN_FILA. */
#define D_TICK_VERIFICACAO 5      /* periodicidade da verificacao automatica */  /* linha: Define a macro ou constante simbolica D_TICK_VERIFICACAO. */

struct configuracao_s {  /* linha: Comeca a estrutura configuracao_s e agrupa os seus campos. */
    int max_espera;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int n_caixas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int tempo_produto;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int max_preco;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int max_fila;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int min_fila;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int tick_verificacao;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
};  /* linha: Fecha a declaracao composta atual. */

/* Preenche com os valores por defeito. */
void configuracao_defeitos(Configuracao_t *c);  /* linha: Declara a funcao configuracao_defeitos para uso por outros modulos. */

/* Verifica se a tabela e coerente. Devolve 1 se ok, 0 se nao. */
int  configuracao_valida (const Configuracao_t *c);  /* linha: Declara a funcao configuracao_valida para uso por outros modulos. */

/* Mostra os valores no ecra. */
void configuracao_mostrar(const Configuracao_t *c);  /* linha: Declara a funcao configuracao_mostrar para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
