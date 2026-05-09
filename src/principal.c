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
#include <stdio.h>

#include "configuracao.h"
#include "supermercado.h"
#include "leitor_dados.h"
#include "cronista.h"
#include "interface_consola.h"
#include "painel_metricas.h"
#include "vigilante_memoria.h"

int main(int argc, char **argv)
{
    Supermercado_t super;
    Configuracao_t cfg;
    int            n_carregadas;
    const char    *cam_cfg = "data/Configuracao.txt";
    const char    *cam_dad = "data/Dados.txt";

    if (argc >= 2) cam_cfg = argv[1];
    if (argc >= 3) cam_dad = argv[2];

    printf("=========================================================\n");
    printf("  Compra Aqui Lda. — Gestao de Caixas (ED 2025/2026)    \n");
    printf("=========================================================\n");

    leitor_carregar_configuracao(cam_cfg, &cfg);
    if (!configuracao_valida(&cfg)) {
        printf("[!] configuracao invalida, a usar valores por defeito\n");
        configuracao_defeitos(&cfg);
    }
    configuracao_mostrar(&cfg);

    supermercado_iniciar(&super, &cfg);
    if (!cronista_abrir(&super.cronista)) {
        printf("[!] aviso: nao foi possivel abrir os ficheiros de log\n");
    }

    n_carregadas = leitor_carregar_dados(cam_dad, &super);
    printf("\n>> %d fichas carregadas a partir de %s\n",
           n_carregadas, cam_dad);

    /* Se o ficheiro de dados criou os terminais (formato do
       enunciado), ja estao no elenco. Caso contrario (formato
       legado ou ficheiro vazio), criar a partir da config. */
    if (elenco_total(&super.elenco) == 0) {
        supermercado_inicializar_terminais(&super, cfg.n_caixas);
    }

    cronista_acao(&super.cronista, 0, "ARRANQUE",
                  "sistema iniciado");
    cronista_snapshot(&super.cronista, &super);

    interface_loop(&super);

    printf("\n>> a finalizar...\n");
    painel_relatorio_global(&super);
    painel_exportar_resumo(&super, "logs/resumo_final.txt");

    supermercado_libertar(&super);

    printf("\n>> verificacao final do vigilante de memoria:\n");
    vigia_relatorio();

    return 0;
}
