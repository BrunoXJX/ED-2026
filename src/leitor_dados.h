/*
 * leitor_dados.h
 * --------------
 * Parser dos ficheiros de entrada.
 *
 * Configuracao.txt:  formato "chave = valor" ou "chave valor",
 *                    uma linha por parametro. Comentarios
 *                    comecam por # ou //.
 *                    Chaves reconhecidas:
 *                      MAX_ESPERA, N_CAIXAS,
 *                      TEMPO_ATENDIMENTO_PRODUTO (ou TEMPO_PRODUTO),
 *                      MAX_PRECO, MAX_FILA, MIN_FILA,
 *                      TICK_VERIFICACAO
 *
 * Dados.txt:         formato do enunciado (caixas com clientes):
 *                      N                      // numero de caixas
 *                      NomeCaixa : 0|1        // nome : estado
 *                      M                      // numero de clientes
 *                      NomeCliente : artigos  // por cliente
 *                    Comentarios com // ou #. Linhas vazias
 *                    ignoradas.
 *
 *                    Formato alternativo (legado):
 *                      nome;quantidade_artigos
 *                    Auto-detectado pela presenca de ';' na
 *                    primeira linha util.
 */
#ifndef LEITOR_DADOS_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho LEITOR_DADOS_H. */
#define LEITOR_DADOS_H  /* linha: Define a macro ou constante simbolica LEITOR_DADOS_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

/* Carrega Configuracao.txt para @c. Devolve numero de chaves
   reconhecidas. Suporta formato "CHAVE = VALOR" e "CHAVE VALOR". */
int leitor_carregar_configuracao(const char *caminho, Configuracao_t *c);  /* linha: Declara a funcao leitor_carregar_configuracao para uso por outros modulos. */

/* Carrega Dados.txt. Auto-detecta o formato (enunciado ou legado).
   Devolve quantidade de fichas adicionadas.
   No formato do enunciado, tambem cria os terminais. */
int leitor_carregar_dados(const char *caminho, Supermercado_t *s);  /* linha: Declara a funcao leitor_carregar_dados para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
