# Documentacao do Projeto - Compra Aqui Lda.

## Objetivo geral

O projeto implementa, em linguagem C, um simulador de gestao de caixas de supermercado para a empresa ficticia "Compra Aqui Lda.". A aplicacao carrega caixas, clientes e parametros a partir de ficheiros de texto, simula o atendimento ao longo do tempo e permite ao gerente abrir, fechar, redistribuir e consultar caixas. No fim, apresenta metricas de desempenho, memoria utilizada e historicos em ficheiros CSV.

## Autores

- Bruno Jesus — 25007
- A definir
- A definir
- A definir

## Tecnologias utilizadas

- Linguagem C, norma C99.
- `gcc`/MinGW e `mingw32-make` para compilacao em Windows.
- Ficheiros `.txt` para configuracao e dados iniciais.
- Ficheiros `.csv` para historico de utilizador, eventos do motor e evolucao da simulacao.
- Estruturas de dados trabalhadas em ED: listas ligadas, filas, tabela hash e calendario ordenado de eventos.

## Estrutura principal

- `src/principal.c`: ponto de entrada, carregamento inicial, arranque dos logs, ciclo da interface e libertacao final.
- `src/supermercado.[ch]`: estado global do sistema, incluindo configuracao, terminais, indice, calendario, cronista e metricas.
- `src/motor_simulacao.[ch]`: avanco do tempo, atendimento, ofertas, abertura/fecho automatico e redistribuicao.
- `src/interface_consola.[ch]`: menus e comandos do utilizador.
- `src/leitor_dados.[ch]`: leitura de `Configuracao.txt` e `Dados.txt`.
- `src/fila_atendimento.[ch]`, `src/elenco_terminais.[ch]`, `src/indice_pesquisa.[ch]` e `src/calendario_eventos.[ch]`: estruturas de dados centrais.
- `src/cronista.[ch]`: escrita dos CSV de auditoria, eventos e historico.
- `src/painel_metricas.[ch]`: relatorios, diagnostico e exportacao do resumo final.
- `data/Configuracao.txt`: parametros do enunciado.
- `data/Dados.txt`: caixas e clientes iniciais.

## Funcionalidades implementadas

- Carregamento automatico de configuracao e dados no inicio do programa.
- Gestao de caixas abertas, fechadas e em drenagem.
- Filas de atendimento por caixa com clientes e numero de artigos.
- Tempo de atendimento deterministico por produto no intervalo `[2, TEMPO_ATENDIMENTO_PRODUTO]`.
- Geracao deterministica de artigos e precos para cumprir o requisito de cada produto ter nome e preco.
- Oferta automatica do artigo mais caro quando o cliente ultrapassa `MAX_ESPERA` em espera.
- Pesquisa de cliente por nome atraves de tabela hash.
- Abertura manual e automatica de caixas.
- Fecho gentil de caixas, deixando atender quem ja esta em espera.
- Fecho imediato com redistribuicao da fila para outras caixas abertas.
- Movimento manual de clientes entre caixas.
- Redistribuicao manual e automatica de filas.
- Listagem de clientes atendidos por caixa.
- Relatorios globais e por terminal.
- Contabilizacao de memoria usada, pico de memoria, blocos vivos e memoria desperdicada.
- Logs CSV das acoes do utilizador, eventos internos e historico da simulacao.

## Funcionalidades adicionadas nesta revisao

- A decisao automatica de abrir/fechar caixas passou a usar a media de clientes em espera por caixa aberta, como indicado no enunciado.
- Quando a media fica abaixo de `MIN_FILA`, a caixa aberta com menos pessoas passa para drenagem.
- Foi adicionada a opcao `A5`, que fecha uma caixa e redistribui a sua fila pelas restantes caixas abertas.
- Ao mover ou redistribuir clientes, o prazo de oferta respeita sempre o instante original de chegada; mudar de fila nao reinicia `MAX_ESPERA`.
- O formato alternativo `nome;quantidade` cria terminais antes de inserir clientes, evitando falhas por falta de caixas.
- Foram reforcadas validacoes para evitar fichas parcialmente inseridas no indice ou nas filas quando uma operacao falha.
- Foi criado `logs/historico_simulacao.csv`, com uma linha por tick e metricas de evolucao.
- A identificacao do grupo foi atualizada com Bruno Jesus e os restantes elementos ficaram como "A definir".

## Partes principais do codigo

O `Supermercado_t` agrega o estado da aplicacao. Os terminais sao mantidos numa lista ligada ordenada por id, cada terminal tem uma fila ligada de fichas e o indice hash permite encontrar clientes rapidamente pelo nome.

O motor avanca por ticks. Em cada tick processa eventos vencidos, inicia atendimentos em caixas livres, fecha caixas que terminaram a drenagem e verifica se deve abrir ou drenar caixas. O calendario de eventos fica ordenado por instante, o que evita percorrer todo o sistema para descobrir o proximo acontecimento.

A tecnica de "lazy deletion" usa uma versao em cada ficha. Quando a ficha muda de estado ou de caixa, a versao aumenta. Eventos antigos ficam obsoletos e sao ignorados quando chegam ao calendario.

O cronista separa tres responsabilidades: `acoes_utilizador.csv` guarda comandos do utilizador, `eventos_motor.csv` guarda eventos automaticos e `historico_simulacao.csv` guarda snapshots numericos para analise.

O vigilante de memoria envolve as alocacoes dinamicas e guarda o tamanho de cada bloco num pequeno cabecalho escondido. Assim consegue calcular memoria viva, pico, total pedido e total devolvido.

## Como executar

Compilar:

```sh
mingw32-make
```

Executar com os ficheiros por omissao:

```sh
./compraaqui
```

Executar com ficheiros especificos:

```sh
./compraaqui caminho/Configuracao.txt caminho/Dados.txt
```

No Windows, tambem pode ser necessario executar:

```sh
.\compraaqui.exe
```

## Como testar e verificar

Fluxo principal recomendado:

```text
B3
C5
0
D3
E3
```

Este fluxo corre a simulacao ate estabilizar, lista os atendidos de todas as caixas, mostra o relatorio global e sai libertando memoria.

Outros testes importantes:

- `A1`: abrir uma caixa fechada.
- `A2`: drenar uma caixa aberta.
- `A3`: mover uma ficha em espera para outra caixa.
- `A4`: redistribuir clientes de uma caixa.
- `A5`: fechar uma caixa e redistribuir a fila.
- `B4`: forcar a verificacao de balanco.
- `C1`: pesquisar um cliente por nome.
- `D4`: ver estatisticas da tabela hash.
- `D5`: confirmar memoria usada pelo vigilante.
- Confirmar que existem os ficheiros `logs/acoes_utilizador.csv`, `logs/eventos_motor.csv`, `logs/historico_simulacao.csv` e `logs/resumo_final.txt`.

## Decisoes tecnicas importantes

- Foi usada uma lista ligada de terminais em vez de um array fixo para facilitar abertura, fecho e manutencao do estado das caixas.
- A distribuicao de clientes escolhe a caixa com menor tempo estimado de espera, nao apenas a fila mais curta.
- Os artigos sao gerados deterministicamente a partir do nome do cliente porque o ficheiro de dados do enunciado so fornece a quantidade de produtos.
- As ofertas sao agendadas pelo prazo absoluto `instante_chegada + MAX_ESPERA`, garantindo que mudar de caixa nao beneficia nem penaliza indevidamente o cliente.
- O fecho automatico e drenante para nao perder clientes; o fecho imediato redistribui a fila, respeitando o requisito de uma caixa que tenha de fechar num dado instante.

## Limitacoes conhecidas

- Os restantes autores ainda estao como "A definir" e devem ser preenchidos antes da entrega.
- A compilacao depende de uma instalacao funcional do MinGW/GCC. Se surgir `Permission denied` ao ler ficheiros em pastas Dropbox/OneDrive ou headers do MinGW, o problema e de permissoes/instalacao local; copiar o projeto para uma pasta simples, como `C:\Projetos\ED`, ou corrigir as permissoes costuma resolver.
- Os ficheiros de log sao gerados em runtime e podem ser substituidos em cada execucao.
- Antes de criar o ZIP final, os colegas devem excluir `build/`, ficheiros `.o`, `compraaqui`, `compraaqui.exe`, logs gerados e ficheiros de IDE. O enunciado pede apenas codigo fonte, dados necessarios, relatorio/documentacao e identificacao.

## Proximos passos possiveis

- Preencher os tres autores em falta.
- Acrescentar um pequeno conjunto de dados de teste para casos extremos.
- Criar um alvo `make test` com execucoes automaticas simples.
- Melhorar a exportacao final com mais campos estatisticos, caso seja pedido na defesa.
