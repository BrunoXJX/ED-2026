/*
 * interface_consola.h
 * -------------------
 * Menu da consola. Em vez de uma lista linear de opcoes 1..N,
 * o menu esta organizado por CATEGORIAS, cada uma com a sua
 * tecla de atalho:
 *
 *   [A] OPERACAO   — abrir/drenar terminais, mover clientes
 *   [B] SIMULACAO  — avancar 1, N ou ate estabilizar
 *   [C] CONSULTA   — pesquisar cliente, ver terminal, ver fila
 *   [D] METRICAS   — relatorios, snapshot, exportacoes
 *   [E] SISTEMA    — configuracao, diagnostico, sair
 *
 * O loop principal aceita o codigo da categoria seguido do
 * codigo da accao, e.g. "A1", "B3", "C2".
 */
#ifndef INTERFACE_CONSOLA_H  /* linha: Inicia a guarda de inclusao para evitar repetir o cabecalho INTERFACE_CONSOLA_H. */
#define INTERFACE_CONSOLA_H  /* linha: Define a macro ou constante simbolica INTERFACE_CONSOLA_H. */

#include "nucleo_tipos.h"  /* linha: Importa as declaracoes locais de nucleo_tipos.h usadas neste modulo. */

/* Loop principal. Devolve quando o utilizador escolhe sair. */
void interface_loop(Supermercado_t *s);  /* linha: Declara a funcao interface_loop para uso por outros modulos. */

#endif  /* linha: Fecha o bloco de compilacao condicional ou a guarda do cabecalho. */
