# Relatorio Geral do Projeto e do Codigo

Projeto: Compra Aqui Lda. - Gestao de Caixas
Unidade curricular: Estruturas de Dados
Data da analise: 2026-05-13
Linguagem: C, norma C99

## 1. Objetivo do projeto

Este projeto implementa um simulador de gestao de caixas de supermercado. A aplicacao carrega parametros de configuracao e dados iniciais a partir de ficheiros de texto, cria caixas e clientes, simula o atendimento ao longo do tempo e permite ao utilizador gerir manualmente o funcionamento das caixas atraves de uma interface de consola.

O programa tambem calcula metricas de desempenho, acompanha a memoria dinamica usada, regista eventos em ficheiros CSV e exporta um resumo final da simulacao.

## 2. Ambito da revisao feita

Foram analisados e comentados os ficheiros de codigo-fonte em `src/`, incluindo todos os ficheiros `.c` e `.h`. As linhas de codigo receberam comentarios explicativos inline no formato `/* linha: ... */`, mantendo o comportamento original do programa.

Os ficheiros de dados, logs, executaveis, objetos compilados e documentacao anterior nao foram alterados, exceto pela criacao deste novo relatorio.

## 3. Estrutura geral do projeto

```text
ED 20252026/
|-- Makefile
|-- README.md
|-- DOCUMENTACAO_PROJETO.md
|-- relatorio_tecnico.md
|-- RELATORIO_GERAL_PROJETO_CODIGO.md
|-- identificacao.txt
|-- data/
|   |-- Configuracao.txt
|   `-- Dados.txt
|-- logs/
|   |-- acoes_utilizador.csv
|   |-- eventos_motor.csv
|   |-- historico_simulacao.csv
|   `-- resumo_final.txt
`-- src/
    |-- principal.c
    |-- nucleo_tipos.h
    |-- configuracao.[ch]
    |-- catalogo_artigos.[ch]
    |-- ficha_cliente.[ch]
    |-- fila_atendimento.[ch]
    |-- terminal_caixa.[ch]
    |-- elenco_terminais.[ch]
    |-- indice_pesquisa.[ch]
    |-- calendario_eventos.[ch]
    |-- supermercado.[ch]
    |-- cronista.[ch]
    |-- motor_simulacao.[ch]
    |-- painel_metricas.[ch]
    |-- leitor_dados.[ch]
    `-- interface_consola.[ch]
```

## 4. Fluxo principal de execucao

O ponto de entrada esta em `src/principal.c`. A execucao segue esta ordem:

1. Define os caminhos por omissao para `data/Configuracao.txt` e `data/Dados.txt`.
2. Permite substituir esses caminhos por argumentos da linha de comandos.
3. Carrega a configuracao e valida os valores.
4. Inicializa o estado central do supermercado.
5. Abre o sistema de logs.
6. Carrega caixas e clientes a partir do ficheiro de dados.
7. Cria caixas a partir da configuracao se o ficheiro de dados nao as tiver criado.
8. Regista o arranque nos logs.
9. Entra no loop da interface de consola.
10. Ao sair, mostra metricas finais, exporta o resumo e liberta a memoria.
11. Mostra o relatorio final do vigilante de memoria.

## 5. Estado central do sistema

O `Supermercado_t`, definido em `src/supermercado.h`, e a estrutura agregadora do programa. Guarda:

- configuracao ativa;
- lista de terminais de caixa;
- indice hash de clientes;
- calendario de eventos futuros;
- cronista responsavel pelos logs;
- tick atual da simulacao;
- metricas globais acumuladas.

Esta organizacao permite que os modulos especializados trabalhem sobre um unico estado coerente, sem duplicar informacao.

## 6. Modulos do codigo

| Modulo | Responsabilidade principal |
|---|---|
| `principal.c` | Arranque, carregamento inicial, loop principal e encerramento. |
| `nucleo_tipos.h` | Enumeracoes, constantes e forward declarations partilhadas. |
| `configuracao.[ch]` | Valores por defeito, validacao e apresentacao da configuracao. |
| `catalogo_artigos.[ch]` | Geracao deterministica de artigos, precos e tempos de servico. |
| `ficha_cliente.[ch]` | Criacao, impressao, estado e destruicao de fichas de cliente. |
| `fila_atendimento.[ch]` | Fila ligada de clientes por caixa. |
| `terminal_caixa.[ch]` | Estado e metricas individuais de cada caixa. |
| `elenco_terminais.[ch]` | Lista ligada ordenada de terminais. |
| `indice_pesquisa.[ch]` | Tabela hash para pesquisa rapida de clientes por nome. |
| `calendario_eventos.[ch]` | Lista ordenada de eventos futuros da simulacao. |
| `supermercado.[ch]` | Estado global, inicializacao, libertacao e helpers. |
| `cronista.[ch]` | Escrita dos logs CSV. |
| `motor_simulacao.[ch]` | Avanco temporal, atendimento, ofertas e balanceamento. |
| `painel_metricas.[ch]` | Relatorios, diagnostico e exportacao de resumo. |
| `leitor_dados.[ch]` | Leitura da configuracao e dos dados iniciais. |
| `interface_consola.[ch]` | Menus e comandos interativos do utilizador. |
| `vigilante_memoria.[ch]` | Envoltorio de alocacao dinamica com contadores de memoria. |

## 7. Estruturas de dados utilizadas

### 7.1 Filas ligadas

Cada terminal contem uma `FilaAtendimento_t`, implementada como lista ligada com ponteiro para a frente e para a traseira. Isto permite inserir clientes no fim e remover clientes do inicio de forma eficiente.

As filas nao sao proprietarias das fichas de cliente; guardam apenas ponteiros. A propriedade principal das fichas fica centralizada no indice de pesquisa, evitando libertacoes duplicadas.

### 7.2 Lista ligada de terminais

O conjunto de caixas e mantido em `ElencoTerminais_t`, uma lista ligada ordenada por identificador. Esta escolha facilita a abertura, drenagem, fecho e percursos sobre todas as caixas sem depender de um array fixo.

### 7.3 Tabela hash

O `IndicePesquisa_t` usa uma tabela hash com encadeamento separado. A chave e o nome do cliente, e a funcao de hash usada e a `sdbm`, implementada no modulo do catalogo.

Esta estrutura torna a pesquisa por nome eficiente em media, adequada para as consultas feitas no menu.

### 7.4 Calendario de eventos

O `Calendario_t` e uma lista ligada ordenada por instante. O primeiro elemento e sempre o evento mais proximo. A insercao e ordenada, e a extracao do proximo evento e direta.

Este desenho evita percorrer todas as caixas a cada instante para descobrir o que termina primeiro.

### 7.5 Vigilante de memoria

O modulo `vigilante_memoria` acrescenta um cabecalho escondido antes de cada bloco alocado. Esse cabecalho guarda o tamanho util pedido, permitindo calcular memoria viva, pico de memoria, total pedido, total devolvido e numero de blocos vivos.

## 8. Motor de simulacao

O motor, em `motor_simulacao.[ch]`, e responsavel por avancar o tempo e manter a dinamica do supermercado. Em cada tick, o motor:

1. Incrementa o tempo atual.
2. Processa eventos vencidos no calendario.
3. Puxa clientes das filas para atendimento em caixas livres.
4. Fecha definitivamente caixas que estavam em drenagem e ja ficaram vazias.
5. Verifica periodicamente se deve abrir, drenar ou redistribuir caixas.
6. Regista snapshots no historico da simulacao.

Os principais eventos sao:

- fim de atendimento;
- oferta devida por espera excessiva;
- verificacao de balanceamento.

## 9. Lazy deletion

Cada ficha de cliente tem um campo `versao`. Quando a ficha muda de estado ou de caixa, essa versao e incrementada. Os eventos guardam a versao da ficha no momento em que foram criados.

Quando um evento chega ao calendario, o motor compara a versao do evento com a versao atual da ficha. Se forem diferentes, o evento esta obsoleto e e descartado.

Esta tecnica simplifica a remocao de eventos antigos, porque evita procurar e apagar eventos no meio da lista ligada.

## 10. Politica de atendimento e balanceamento

O sistema nao escolhe necessariamente a fila com menos pessoas. A escolha privilegia o menor tempo estimado total de espera, calculado a partir do tempo de servico dos clientes.

Isto e mais justo do que contar apenas pessoas, porque uma fila pequena pode estar mais lenta se tiver clientes com muitos artigos.

A verificacao automatica usa os limites configurados:

- se a media de clientes em espera por caixa aberta ultrapassa `MAX_FILA`, abre uma nova caixa quando possivel;
- se a media fica abaixo de `MIN_FILA`, uma caixa pode entrar em drenagem;
- a redistribuicao move clientes para equilibrar melhor as filas.

## 11. Gestao de clientes e artigos

Cada cliente e representado por uma `FichaCliente_t`. A ficha guarda nome, quantidade de artigos, cesto, total a pagar, tempo estimado de servico, estado, caixa atual, informacao de oferta e versao para lazy deletion.

Como o ficheiro de dados apenas indica a quantidade de artigos, o modulo `catalogo_artigos` gera os artigos de forma deterministica a partir do nome do cliente. Assim, a mesma entrada gera sempre os mesmos artigos, precos e tempos.

## 12. Interface de consola

A interface esta organizada por categorias:

- `A`: operacoes sobre caixas e clientes;
- `B`: simulacao e avanco temporal;
- `C`: consultas;
- `D`: metricas e diagnostico;
- `E`: sistema e saida.

Esta separacao facilita a utilizacao e reduz a complexidade do menu principal.

## 13. Ficheiros de entrada

`data/Configuracao.txt` contem pares `CHAVE = VALOR`, como:

- `MAX_ESPERA`;
- `N_CAIXAS`;
- `TEMPO_PRODUTO`;
- `MAX_PRECO`;
- `MAX_FILA`;
- `MIN_FILA`;
- `TICK_VERIFICACAO`.

`data/Dados.txt` contem o numero de caixas, o estado inicial de cada caixa e os clientes associados a cada uma.

O leitor tambem suporta um formato legado `nome;quantidade`.

## 14. Ficheiros de saida

O programa escreve ficheiros em `logs/`:

- `acoes_utilizador.csv`: comandos feitos pelo utilizador;
- `eventos_motor.csv`: eventos automaticos do motor;
- `historico_simulacao.csv`: snapshots numericos por tick;
- `resumo_final.txt`: resumo final das metricas.

## 15. Pontos fortes do codigo

- Boa separacao por modulos.
- Uso adequado de estruturas de dados da unidade curricular.
- Estado global bem centralizado.
- Sistema de eventos mais eficiente do que uma simulacao que varre tudo a cada tick.
- Pesquisa por nome com tabela hash.
- Memoria monitorizada por um modulo dedicado.
- Logs separados por responsabilidade.
- Comentarios linha a linha adicionados ao codigo-fonte para apoiar leitura e defesa.

## 16. Limitacoes e melhorias possiveis

- Os autores em `identificacao.txt` ainda devem ser confirmados antes da entrega final.
- Nao existe uma suite automatica de testes; a validacao e principalmente manual.
- O `Makefile` usa `make`, mas no Windows pode ser necessario executar `mingw32-make`.
- Os comentarios linha a linha aumentam bastante o tamanho visual do codigo; sao uteis para estudo, mas em codigo profissional normalmente seriam substituidos por comentarios de bloco mais seletivos.
- Poderia existir um alvo `test` para correr fluxos automaticos de simulacao.

## 17. Verificacao tecnica

A compilacao foi verificada com:

```sh
mingw32-make
```

Resultado: compilacao concluida com sucesso.

O comando `make` simples nao estava disponivel neste PowerShell, mas `mingw32-make` estava instalado e compilou o projeto com as flags definidas no `Makefile`.

## 18. Conclusao

O projeto esta bem modularizado para um trabalho de Estruturas de Dados. As principais estruturas sao listas ligadas, filas, tabela hash e calendario ordenado de eventos. O motor de simulacao articula essas estruturas para gerir tempos de espera, atendimentos, ofertas, abertura e fecho de caixas.

Nesta revisao, o codigo-fonte em `src/` ficou comentado linha a linha e este relatorio resume a arquitetura, as responsabilidades dos modulos, o funcionamento interno e os pontos de validacao do projeto.
