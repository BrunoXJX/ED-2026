# Relatorio Tecnico — Compra Aqui Lda.

> Projeto da unidade curricular **Estruturas de Dados**, ESTGV
> 2025/2026. Aluno: ver `identificacao.txt`.

Este documento explica as escolhas de arquitectura, as
estruturas de dados, o ciclo de simulacao, o sistema de
metricas e o sistema de logs.

## 0. Estimativa de horas

| Fase                                       | Horas aprox. |
|--------------------------------------------|--------------|
| Analise do enunciado e planeamento         |    3         |
| Estruturas centrais (tipos, fila, terminal)|    8         |
| Motor de simulacao + calendario de eventos |   10         |
| Interface consola (menus por categorias)   |    5         |
| Leitor de dados + Configuracao             |    4         |
| Vigilante de memoria                       |    4         |
| Indice de pesquisa (hash)                  |    3         |
| Metricas + cronista (logs CSV)             |    5         |
| Testes + depuracao                         |    6         |
| Relatorio tecnico + documentacao           |    4         |
| **Total**                                  |  **~52 h**   |

## 1. Arquitectura

A solucao esta organizada como um **estado central
(Supermercado_t)** servido por um **motor de simulacao** que
opera sobre ele consultando um **calendario de eventos** e
deixando metricas/logs ao seu rasto.

```
              +----------------------+
              |   interface_consola  |     (menus por categorias)
              +----------+-----------+
                         |
                         v
  +----------+    +------+--------+    +-----------------+
  | leitor   |--->|  Supermercado |--->| painel_metricas |
  | dados    |    +------+--------+    +-----------------+
  +----------+           |
                         v
                   +-----+--------+
                   | motor_simul. |
                   +---+------+---+
                       |      |
                  +----v-+  +-v-----------+
                  | cale.|  | cronista    |
                  | even.|  | (logs CSV)  |
                  +------+  +-------------+
```

### 1.1 Diagrama de dependencias entre entidades

```
                   nucleo_tipos.h
                 (enums + forward decl.)
                         |
    +----------+---------+---------+-----------+
    |          |         |         |           |
    v          v         v         v           v
 Artigo_t  FichaCliente_t  NoFila_t  Terminal_t  Evento_t
 (catalogo)  (ficha_cl.)  (fila_at.) (term_cx.)  (calen.)
    |          |            |          |           |
    +-----+----+            +----+-----+           |
          |                      |                 |
          v                      v                 v
     FilaAtendimento_t     ElencoTerminais_t   Calendario_t
          |                      |                 |
          +----------+-----------+---------+-------+
                     |                     |
                     v                     v
              Supermercado_t          Cronista_t
                     |                     |
         +-----------+-----------+---------+
         |           |           |
         v           v           v
  motor_simul.  leitor_dados  painel_metricas
         |                         |
         +------------+------------+
                      |
                      v
               interface_consola
                      |
                      v
                  principal.c
```

A relacao de propriedade:
- O **indice** (hash) e a referencia mestre das fichas;
- As **filas** guardam apenas ponteiros — nao possuem as fichas;
- Cada **terminal** possui a sua fila e a lista de atendidos;
- O **supermercado** agrega tudo (por composicao, nao por ponteiro).

## 2. Estruturas de dados

### 2.1 Lista ligada simples — `FilaAtendimento_t`

Cada terminal tem uma fila ligada simples com cabeca (`frente`)
e cauda (`tras`). Inserir no fim e em O(1) gracas a cauda;
remover do inicio tambem em O(1). Cada `NoFila_t` guarda apenas
um ponteiro para a `FichaCliente_t` — a propriedade da ficha
nao pertence a fila, o que permite mover fichas entre filas
sem libertar dados.

### 2.2 Lista ligada simples — `ElencoTerminais_t`

Lista de todos os terminais (em qualquer estado), mantida
ordenada por `id`. Permite abrir/fechar caixas dinamicamente
sem precisar de redimensionar nada. A funcao
`elenco_melhor_para_novo` percorre a lista e devolve o terminal
ABERTO com menor tempo estimado de espera (a politica de
balanceamento — ver §4).

### 2.3 Lista ligada simples (ordenada) — `Calendario_t`

O calendario de eventos e uma lista ligada **ordenada por
instante crescente**. Inserir e O(N) (porque tem de procurar a
posicao certa); pop do primeiro e O(1). Para uma simulacao
academica deste tamanho e perfeitamente adequado.

### 2.4 Tabela hash com encadeamento separado — `IndicePesquisa_t`

A pesquisa de cliente por nome e O(1) medio. A funcao de hash
e a `sdbm` (definida em `catalogo_artigos.c`). A tabela tem
`INDICE_HASH_TAM = 31` baldes (numero primo). Cada balde e uma
lista ligada simples de `NoIndice_t`. O modulo expoe uma
funcao `indice_imprimir_estatistica` que mostra o numero de
baldes ocupados e o tamanho do maior balde — util para
demonstrar que a hash esta a distribuir bem.

### 2.5 Cabecalho escondido de bloco — `vigilante_memoria`

Cada chamada `vigia_pedir(N)` aloca `sizeof(CabBloco_t) + N`
bytes a libc, escreve o tamanho `N` no header e devolve um
ponteiro para o byte logo apos o header. Quando se chama
`vigia_devolver(p)`, o vigilante recua `sizeof(CabBloco_t)`,
le o tamanho gravado, actualiza os contadores e chama `free`.

O `union` que define o `CabBloco_t` inclui um `long double`,
um ponteiro e um `long long` para forcar um alinhamento
conservador — assim o ponteiro entregue ao programa fica
alinhado para qualquer tipo basico de C.

## 3. Ciclo da simulacao

### 3.1 Avancar um tick (`motor_passo`)

```c
tick_actual++;

processar_eventos_vencidos(s);   // eventos com instante <= tick_actual
puxar_proximos_atendimentos(s);  // terminais livres puxam o proximo
fechar_terminais_drenados(s);    // DRENANDO + fila vazia => FECHADO

if (tick_actual % tick_verificacao == 0) verificar_balanco(s);
```

### 3.2 Lazy deletion no calendario

Quando o motor extrai um evento do calendario, primeiro
compara `evento->versao_ficha` com `evento->ficha->versao`. Se
forem diferentes, a ficha mudou de estado entretanto e o
evento esta obsoleto — e ignorado em silencio. As situacoes
que incrementam a versao da ficha sao:

- inicio de atendimento;
- fim de atendimento;
- mudanca de terminal (manual ou automatica).

Esta tecnica evita ter de remover eventos do meio do
calendario, o que seria O(N) por remocao. As remocoes
acontecem naturalmente quando os eventos chegam a frente.

### 3.3 Atendimento

Quando um terminal aberto/drenando esta sem ficha em
atendimento mas tem fila, puxa-se o primeiro elemento. Calcula-
se o tempo de espera (`inicio - chegada`), agenda-se o
`FIM_ATENDIMENTO` em `tick_actual + tempo_servico_estimado`, e
o `instante_inicio_servico` da ficha e gravado.

### 3.4 Oferta de produto (MAX_ESPERA ultrapassado)

Quando uma ficha entra no sistema, e agendada uma `OFERTA_DEVIDA`
para `chegada + MAX_ESPERA`. Se a ficha ainda estiver em
`AGUARDA` quando o evento chega, recebe oferta do artigo mais
caro do seu cesto, contabiliza-se prejuizo e regista-se. Se ja
estiver a ser atendida, o evento e descartado pelo lazy
deletion.

### 3.5 Verificacao automatica (a cada `TICK_VERIFICACAO`)

A funcao `motor_verificar_balanco` usa a media de clientes em
espera por caixa aberta, como indicado no enunciado:

1. Se a media ultrapassa `MAX_FILA`, abre o primeiro terminal
   fechado e redistribui as filas sobrelotadas.
2. Se a media fica abaixo de `MIN_FILA` e houver mais de uma
   caixa aberta, drena (`DRENANDO`) a caixa aberta com menos
   pessoas. A drenagem evita abandonar clientes a meio do
   atendimento.

## 4. Politica de balanceamento por menor tempo de espera

`elenco_melhor_para_novo` nao escolhe a fila mais curta. Em
vez disso, soma os `tempo_servico_estimado` de todas as fichas
da fila (mais o que esta em atendimento) e escolhe a menor.
Isto evita o caso classico em que uma fila com 2 clientes de
50 produtos cada e mais lenta que uma fila com 4 clientes de
1 produto cada.

## 5. Memoria

### 5.1 Em uso

`vigia_em_uso()` devolve a soma dos tamanhos uteis dos blocos
ainda nao libertados. E exacto byte a byte porque cada bloco
guarda o seu tamanho no header escondido.

### 5.2 Pico

Actualizado a cada `vigia_pedir`.

### 5.3 Desperdicada

Calculada **dinamicamente** pela funcao
`supermercado_memoria_desperdicada`:

- soma `sizeof(Terminal_t)` por cada terminal `FECHADO`;
- soma `sizeof(Evento_t)` por cada evento obsoleto no
  calendario.

A justificacao: um terminal fechado continua a ocupar um
struct na memoria mas nao esta a produzir valor; um evento
obsoleto continua na lista a espera de ser drenado. Sao bytes
"vivos" mas "improdutivos".

Esta abordagem e mais elegante (e menos enganadora) que a
alternativa "marcar manualmente": nao ha risco de marcacao
desincronizada com o estado real.

## 6. Logs

Tres ficheiros CSV sao mantidos abertos durante toda a sessao
e fechados pelo `cronista_fechar`:

| Ficheiro                       | Conteudo                                |
|--------------------------------|-----------------------------------------|
| `logs/acoes_utilizador.csv`    | accoes feitas pelo utilizador no menu   |
| `logs/eventos_motor.csv`       | eventos automaticos do motor            |
| `logs/historico_simulacao.csv` | snapshots numericos por tick            |

A separacao permite filtrar facilmente o que vem do utilizador,
o que vem do sistema e a evolucao numerica da simulacao. O
`cronista` faz **flush** depois de cada escrita para que os
ficheiros estejam sempre actualizados ate em caso de paragem
inesperada.

## 7. Metricas finais

`painel_relatorio_global` apresenta:

- tick final, terminais totais/abertos, clientes atendidos,
  clientes ainda no sistema;
- tempo medio de espera e servico (calculados a partir das
  somas incrementais — nao recalculam nada);
- ofertas concedidas e prejuizo total;
- memoria em uso, pico, desperdicada, blocos vivos;
- distincoes: terminal com mais clientes, terminal com mais
  artigos, operador menos activo (excluindo terminais nunca
  abertos).

`painel_exportar_resumo` escreve uma versao chave/valor do
relatorio em `logs/resumo_final.txt`.

## 8. Resolucao da ambiguidade dos artigos

Como o `Dados.txt` so traz a quantidade, geramos o cesto
deterministicamente a partir do nome do cliente:

```
semente = sdbm(nome)                  -> unsigned long
sequencia = xorshift32(semente)
para cada artigo i:
    n = sequencia % 9999
    designacao = "art_NNNN"
    p_centimos = sequencia % (MAX_PRECO * 100) + 1
    preco = p_centimos / 100
```

O tempo de servico tambem e deterministico mas aleatorizado:

```
semente = sdbm(nome) XOR 0xDEADBEEF
para cada artigo i:
    tempo_i = 2 + sequencia % (TEMPO_PRODUTO - 2 + 1)
tempo_total = soma de todos os tempo_i
```

Cada produto demora entre 2 e TEMPO_ATENDIMENTO_PRODUTO
segundos, conforme especificado no enunciado. O uso de uma
semente diferente (XOR com constante) evita correlacao com
os nomes dos artigos.

Estas decisoes sao documentadas no codigo (`catalogo_artigos.h`)
e neste relatorio. As vantagens:

- e reprodutivel: a mesma execucao produz os mesmos cestos e tempos;
- nao depende de `rand()` nem do estado da libc;
- so precisa do nome (sem ficheiro de catalogo separado).

## 9. Formato dos ficheiros de dados

O programa suporta dois formatos de `Dados.txt`:

### 9.1 Formato do enunciado (recomendado)

```
6                          // Numero de caixas
Caixa1 : 1                 // nome : estado (1=activa, 0=inactiva)
6                          // Numero de clientes
Alice : 12                 // nome : quantidade de artigos
Beatriz : 5
...
Caixa4 : 0                 // Caixa inactiva
0                          // Zero clientes
```

Os terminais sao criados a partir do ficheiro, com o estado
inicial (activo/inactivo) definido pela segunda coluna.

### 9.2 Formato legado (auto-detectado)

```
Alice;12
Beatriz;5
```

Formato simplificado com `nome;quantidade`. Se detectado
(presença de `;` na primeira linha), os terminais sao criados
a partir da configuracao (`N_CAIXAS`).

A auto-deteccao permite retrocompatibilidade sem configuracao.

## 10. Historico de atendidos por terminal

Cada terminal mantem uma lista ligada (`NoAtendido_t`) com
copia dos dados dos clientes ja atendidos: id, nome, quantidade
de artigos, total pago e se recebeu oferta. Esta lista e
consultavel pelo menu `C5` (listar atendidos por terminal).

## 11. Validacao manual

Os testes seguintes foram feitos a mao no terminal:

| # | Accao                                       | Esperado                                          |
|---|---------------------------------------------|---------------------------------------------------|
| 1 | `B3` (correr ate estabilizar)               | 30 atendidos, 0 no sistema                        |
| 2 | `D3` (relatorio global)                     | metricas coerentes, distincoes mostradas          |
| 3 | `D5` (vigilante memoria)                    | em uso > 0 antes de sair                          |
| 4 | `E3` (sair) + relatorio final               | em uso = 0, blocos = 0                            |
| 5 | `C1` Helena                                  | mostra detalhes da ficha                          |
| 6 | `A3 8 3` (move ficha 8 para terminal 3)     | move silenciosamente, log MOVE_FICHA              |
| 7 | `C5 0` (listar atendidos de todos)          | lista completa de 30 clientes atendidos           |
| 8 | `A5 1` (fecho imediato com redistribuicao)  | fila movida para outras caixas abertas            |

Em todos os casos: `total_pedido == total_devolvido` e nao ha
warnings de compilacao com `-Wall -Wextra -Wpedantic`.
