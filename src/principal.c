/*
 * principal.c
 * -----------
 * Ponto de entrada do programa "Compra Aqui Lda. — Gestao de
 * Caixas". Faz exclusivamente:
 *
 *   1. arranca o supermercado com a configuracao por defeito
 *   2. tenta carregar Configuracao.txt se existir
 *   3. cria os terminais e abre os primeiros tres
 *   4. carrega Dados.txt
 *   5. abre o cronista (logs CSV)
 *   6. corre o loop interactivo
 *   7. mostra estatisticas finais
 *   8. liberta tudo
 */
#include <stdio.h>  /* linha: Importa a biblioteca padrao stdio.h necessaria neste modulo. */

#include "configuracao.h"  /* linha: Importa as declaracoes locais de configuracao.h usadas neste modulo. */
#include "supermercado.h"  /* linha: Importa as declaracoes locais de supermercado.h usadas neste modulo. */
#include "leitor_dados.h"  /* linha: Importa as declaracoes locais de leitor_dados.h usadas neste modulo. */
#include "cronista.h"  /* linha: Importa as declaracoes locais de cronista.h usadas neste modulo. */
#include "interface_consola.h"  /* linha: Importa as declaracoes locais de interface_consola.h usadas neste modulo. */
#include "painel_metricas.h"  /* linha: Importa as declaracoes locais de painel_metricas.h usadas neste modulo. */
#include "vigilante_memoria.h"  /* linha: Importa as declaracoes locais de vigilante_memoria.h usadas neste modulo. */

int main(int argc, char **argv)  /* linha: Inicia ou continua a definicao da funcao main. */
{  /* linha: Abre o bloco de instrucoes associado a declaracao anterior. */
    Supermercado_t super;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    Configuracao_t cfg;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    int            n_carregadas;  /* linha: Declara ou inicializa uma variavel local usada neste bloco. */
    const char    *cam_cfg = "data/Configuracao.txt";  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    const char    *cam_dad = "data/Dados.txt";  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */

    if (argc >= 2) cam_cfg = argv[1];  /* linha: Testa a condicao antes de executar o bloco associado. */
    if (argc >= 3) cam_dad = argv[2];  /* linha: Testa a condicao antes de executar o bloco associado. */

    printf("=========================================================\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("  Compra Aqui Lda. — Gestao de Caixas (ED 2025/2026)    \n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    printf("=========================================================\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */

    leitor_carregar_configuracao(cam_cfg, &cfg);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    if (!configuracao_valida(&cfg)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("[!] configuracao invalida, a usar valores por defeito\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
        configuracao_defeitos(&cfg);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    }  /* linha: Fecha o bloco de instrucoes atual. */
    configuracao_mostrar(&cfg);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    supermercado_iniciar(&super, &cfg);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    if (!cronista_abrir(&super.cronista)) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        printf("[!] aviso: nao foi possivel abrir os ficheiros de log\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    n_carregadas = leitor_carregar_dados(cam_dad, &super);  /* linha: Atribui ou atualiza um valor necessario ao estado do programa. */
    printf("\n>> %d fichas carregadas a partir de %s\n",  /* linha: Mostra texto formatado no ecra para o utilizador. */
           n_carregadas, cam_dad);  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */

    /* Se o ficheiro de dados criou os terminais (formato do
       enunciado), ja estao no elenco. Caso contrario (formato
       legado ou ficheiro vazio), criar a partir da config. */
    if (elenco_total(&super.elenco) == 0) {  /* linha: Testa a condicao antes de executar o bloco associado. */
        supermercado_inicializar_terminais(&super, cfg.n_caixas);  /* linha: Acede a um campo de uma estrutura local. */
    }  /* linha: Fecha o bloco de instrucoes atual. */

    cronista_acao(&super.cronista, 0, "ARRANQUE",  /* linha: Acede a um campo de uma estrutura local. */
                  "sistema iniciado");  /* linha: Executa uma instrucao simples necessaria a logica do modulo. */
    cronista_snapshot(&super.cronista, &super);  /* linha: Acede a um campo de uma estrutura local. */

    interface_loop(&super);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    printf("\n>> a finalizar...\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    painel_relatorio_global(&super);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */
    painel_exportar_resumo(&super, "logs/resumo_final.txt");  /* linha: Acede a um campo de uma estrutura local. */

    supermercado_libertar(&super);  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    printf("\n>> verificacao final do vigilante de memoria:\n");  /* linha: Mostra texto formatado no ecra para o utilizador. */
    vigia_relatorio();  /* linha: Chama uma funcao auxiliar para executar esta parte da operacao. */

    return 0;  /* linha: Termina a funcao e devolve o resultado indicado ao chamador. */
}  /* linha: Fecha o bloco de instrucoes atual. */
