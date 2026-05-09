# Compra Aqui Lda. — Gestao de Caixas

Projeto da unidade curricular **Estruturas de Dados** (1.o ano,
ESTGV / Eng. Informatica), ano lectivo 2025/2026.

Simulador de gestao de caixas de um supermercado escrito em C
puro (C99). Le clientes e configuracao de ficheiros de texto,
simula o seu atendimento e produz logs CSV e relatorios de
metricas.

## Como compilar e correr

Em qualquer sistema com `gcc` e `make` (ou `mingw32-make` no
Windows):

```sh
make            # compila e cria o executavel "compraaqui"
make run        # compila e corre
make clean      # apaga objectos e binarios
make rebuild    # apaga e volta a compilar tudo
```

O executavel aceita opcionalmente caminhos para os ficheiros
de configuracao e dados:

```sh
./compraaqui                                 # usa data/Configuracao.txt e data/Dados.txt
./compraaqui outra_config.txt outros_dados.txt
```

## Estrutura

```
ED 20252026/
├── Makefile
├── README.md
├── relatorio_tecnico.md
├── DOCUMENTACAO_PROJETO.md
├── identificacao.txt
├── data/
│   ├── Configuracao.txt
│   └── Dados.txt
├── logs/                           (criado em runtime)
│   ├── acoes_utilizador.csv
│   ├── eventos_motor.csv
│   ├── historico_simulacao.csv
│   └── resumo_final.txt
└── src/
    ├── nucleo_tipos.h
    ├── vigilante_memoria.[hc]
    ├── configuracao.[hc]
    ├── catalogo_artigos.[hc]
    ├── ficha_cliente.[hc]
    ├── fila_atendimento.[hc]
    ├── terminal_caixa.[hc]
    ├── elenco_terminais.[hc]
    ├── indice_pesquisa.[hc]
    ├── calendario_eventos.[hc]
    ├── supermercado.[hc]
    ├── cronista.[hc]
    ├── motor_simulacao.[hc]
    ├── painel_metricas.[hc]
    ├── leitor_dados.[hc]        (auto-detecta formato)
    ├── interface_consola.[hc]
    └── principal.c
```

## Filosofia da solucao

Esta nao e a solucao mais obvia para o enunciado e isso e
intencional. As ideias-chave sao:

1. **Calendario de eventos**: o motor da simulacao mantem uma
   lista ligada ordenada de eventos pendentes (`fim_atendimento`,
   `oferta_devida`). Em vez de varrer todas as caixas a cada
   tick, o motor processa apenas os eventos cujo instante ja
   chegou.
2. **Lazy deletion**: cada `FichaCliente_t` tem um numero de
   versao. Quando uma ficha muda de estado (e.g. e movida de
   caixa), a versao incrementa, e quaisquer eventos antigos
   sobre ela sao ignorados em silencio. Evita ter de remover
   eventos do meio do calendario.
3. **Lista ligada de terminais (em vez de array)**: o numero
   de caixas pode crescer; abrir/fechar e dinamico.
4. **Indice hash em paralelo**: a pesquisa de cliente por nome
   e O(1) medio. O indice e mantido sincronizado com o estado
   real do supermercado.
5. **Cabecalho escondido na alocacao**: o `vigilante_memoria`
   poe um header `CabBloco_t` antes de cada ponteiro entregue
   ao programa, contendo o tamanho. Permite contabilizar
   bytes em uso sem ter de passar tamanhos a `vigia_devolver`.
6. **Memoria desperdicada calculada dinamicamente**: percorre
   o estado actual em vez de marcar/desmarcar manualmente.
7. **Logs separados por responsabilidade**: `acoes_utilizador.csv`
   para auditoria humana, `eventos_motor.csv` para rastreio
   tecnico e `historico_simulacao.csv` para snapshots por tick.
8. **Menu por categorias**: A=Operacao, B=Simulacao,
   C=Consulta, D=Metricas, E=Sistema. Cada categoria tem o seu
   submenu (e.g. `A0` mostra o submenu A; `A1` executa a primeira
   accao).
9. **Politica de balanceamento por menor tempo estimado de
   espera**, nao apenas pelo numero de pessoas na fila.
10. **Encerramento drenante**: ao fechar uma caixa, ela passa a
    `DRENANDO` — deixa de aceitar novos mas atende os que ja
    la estavam, e so depois fica `FECHADO`.
11. **Fecho imediato com redistribuicao**: a opcao `A5`
    redistribui a fila de uma caixa pelas restantes abertas e
    deixa terminar apenas o cliente que ja esteja em atendimento.
12. **Tempo de servico aleatorizado**: cada produto demora entre
    2 e `TEMPO_ATENDIMENTO_PRODUTO` (intervalo conforme enunciado),
    calculado deterministicamente a partir do nome do cliente.
13. **Historico de atendidos**: cada terminal guarda uma lista
    ligada dos clientes que ja atendeu, consultavel via `C5`.

## Resolucao da ambiguidade dos artigos

O enunciado refere que cada artigo tem nome e preco, mas o
formato do `Dados.txt` so traz a quantidade de artigos por
cliente. Resolvemos assim:

- A `FichaCliente_t` tem um campo `cesto[]` de `Artigo_t`
  (nome e preco).
- O modulo `catalogo_artigos` gera o cesto **deterministicamente**
  a partir do nome do cliente:
  1. Calcula uma semente como `sdbm(nome)`.
  2. Usa `xorshift32` para produzir designacoes do tipo `art_NNNN`.
  3. Cada preco e um inteiro de centimos entre 1 e
     `MAX_PRECO * 100`, dividido por 100.
- Reexecutar o programa com os mesmos dados produz exactamente
  os mesmos cestos. Isto facilita testes e a defesa oral.

## Formato dos ficheiros

### Configuracao.txt

Pares `CHAVE = VALOR`, uma linha por chave. Comentarios comecam
por `#`.

```
MAX_ESPERA       = 120
N_CAIXAS         = 6
TEMPO_PRODUTO    = 6
MAX_PRECO        = 40
MAX_FILA         = 7
MIN_FILA         = 3
TICK_VERIFICACAO = 5
```

### Dados.txt (formato do enunciado)

O programa le o formato especificado no enunciado. Cada caixa
define o seu estado (activa/inactiva) e os clientes na fila.
Comentarios com `//` ou `#`.

```
6                          // Numero de caixas
Caixa1 : 1                 // nome : estado (1=activa)
6                          // Numero de clientes
Alice : 12                 // nome : artigos
Beatriz : 5
Carlos : 18
...
Caixa4 : 0                 // Caixa inactiva
0                          // Zero clientes
```

Formato alternativo (legado, auto-detectado): `nome;quantidade`
numa linha por cliente.

## Logs

Sao gravados em `logs/`. O programa cria a pasta automaticamente
se nao existir.

- `acoes_utilizador.csv` — uma linha por accao do utilizador no
  menu (`tick;categoria;descricao`). Categorias: ARRANQUE, ABRIR,
  DRENAR, MOVER, REDISTRIB, SIM, BALANCO, PESQUISA, EXPORT, SAIR.
- `eventos_motor.csv` — uma linha por evento automatico
  (`tick;tipo;descricao`). Tipos: ATEND_INICIO, ATEND_FIM, OFERTA,
  ABERTURA_AUTO, FECHO_AUTO, DRENAR_AUTO, REDISTRIB.
- `historico_simulacao.csv` — uma fotografia numerica por tick:
  caixas abertas, clientes em espera/no sistema, atendidos,
  produtos processados, medias, ofertas, prejuizo e memoria.
- `resumo_final.txt` — um resumo das metricas finais, em formato
  `chave;valor`, escrito automaticamente no final.

## Validacao rapida

```sh
./compraaqui
> B3        (correr ate estabilizar)
> C5 0      (listar todos os atendidos por terminal)
> D3        (relatorio global)
> E3        (sair)
```

Output esperado: 30 clientes atendidos, 0 no sistema, memoria
final de `0 bytes em uso` e `total_pedido == total_devolvido`.
