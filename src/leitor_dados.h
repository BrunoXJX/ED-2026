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
#ifndef LEITOR_DADOS_H
#define LEITOR_DADOS_H

#include "nucleo_tipos.h"

/* Carrega Configuracao.txt para @c. Devolve numero de chaves
   reconhecidas. Suporta formato "CHAVE = VALOR" e "CHAVE VALOR". */
int leitor_carregar_configuracao(const char *caminho, Configuracao_t *c);

/* Carrega Dados.txt. Auto-detecta o formato (enunciado ou legado).
   Devolve quantidade de fichas adicionadas.
   No formato do enunciado, tambem cria os terminais. */
int leitor_carregar_dados(const char *caminho, Supermercado_t *s);

#endif
