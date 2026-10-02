# Pindorama

Jogo de simulação de fazenda em C++ com interface gráfica em raylib, desenvolvido na disciplina **Programação e Desenvolvimento de Software II (PDS II)**, DCC/UFMG, 2026/2.

> **Etapa atual: C6** — definição dos contratos (`.hpp`) e documentação inicial com Doxygen.

## Integrantes

| # | Nome | Bloco |
|---|------|-------|
| 1 | _(Enzo)_ | Tempo e orquestração |
| 2 | _(Lucas Lemos)_ | Mapa e interface |
| 3 | _(Paola)_ | Cultivo |
| 4 | _(Aline)_ | Itens |
| 5 | _(Jonathan)_ | Economia |
| 6 | _(Lucas Lemos)_ | Dados e persistência |

## Descrição do problema

O Pindorama é um jogo de fazenda em que o jogador cuida de uma propriedade dividida em uma grade de tiles. Ele anda pelo mapa, ara a terra, rega, semeia, aduba e colhe plantas que crescem aos poucos conforme o tempo do jogo passa. O que é colhido vai para o inventário e pode ser depositado na caixa de venda, que converte os itens em dinheiro no começo do dia seguinte. Com o dinheiro, o jogador compra sementes, adubo e ferramentas para expandir a produção.

O tempo do jogo corre em passos (ticks) de 10 minutos de jogo. O solo seca depois de um tempo, a planta só cresce enquanto o solo está úmido, e a planta que fica dias seguidos sem água morre e permanece no tile até ser removida com a enxada. O dia vira às 06:00, com ou sem o jogador ter dormido. O progresso pode ser salvo, e falhas no salvamento são registradas em um log de erros.

## Objetivos

- Construir um jogo funcional que cubra o ciclo completo: plantar, cuidar, colher, vender e reinvestir.
- Separar o **núcleo do jogo** (regras, estado, tempo) da **apresentação**, de modo que o núcleo possa ser testado sem janela e **só a `GameView` use a raylib**.
- Aplicar na prática os conceitos da disciplina: orientação a objetos, herança e polimorfismo (itens), padrão Observador (relógio do jogo), padrão Type Object (espécies de cultivo), tratamento de exceções, leitura e escrita de arquivos, STL e documentação de contratos.
- Entregar um código documentado, com contratos claros (`@pre`, `@post`, `@throws`) entre os blocos de cada integrante.

## Motivação

Jogos de fazenda reúnem, em um escopo pequeno, quase todos os problemas de projeto de software que a disciplina cobra: muitos objetos com estados que evoluem no tempo, coleções de itens de tipos diferentes, regras que se influenciam (tempo, água, crescimento, dinheiro), persistência em arquivo e tratamento de erros. Escolher um jogo permite ver o efeito de cada decisão de projeto de forma visível na tela, e o tema, em português e com nome indígena, aproxima o projeto do nosso contexto.

## Arquitetura em resumo

- **Núcleo sem interface:** `Game` cria e possui todos os componentes (`GameClock`, `TileMap`, `Player`, `ShippingBin`, `DeliveryService`, `SaveManager`, `ErrorLogEntry`/log) e recebe comandos já traduzidos, devolvendo um `ActionResult` com sucesso ou o motivo da recusa.
- **Tempo por observadores:** o `GameClock` avisa quem implementa `TimeObserver` (`onTick` a cada 10 minutos de jogo e `onNewDay` na virada do dia). `TileMap`, `ShippingBin` e `DeliveryService` são observadores. O `TileMap` repassa os eventos a cada `Tile`.
- **Cultivo:** `Tile` guarda o estado do solo e, opcionalmente, um `Crop`. `CropType` guarda só os dados da espécie (Type Object) e o estado de cada planta fica no `Crop`.
- **Itens:** `Item` é a base polimórfica de `Seed`, `Fertilizer`, `Tool` e `Produce`, guardados pelo `Inventory`.
- **Apresentação:** `GameView` é a única classe que inclui `raylib.h`. Ela só lê o mundo por referências `const` e todas as ações passam por comandos do `Game`.
- **Convenção do mapa:** grade ortogonal vista de cima, origem `(0, 0)` no canto superior esquerdo, coluna crescendo à direita e linha para baixo (mesma orientação da tela da raylib). Os tiles ficam em um único `std::vector` em ordem de linhas, com `índice = linha * largura + coluna`.

## Cartão CRC → classe

Liga o cartão CRC (etapa C5, em português) à classe do código (C6, em inglês).

| Cartão CRC | Classe | Integrante |
|------------|--------|------------|
| CicloDiario | `GameClock` | 1 |
| _(interface nova)_ | `TimeObserver` (e tipo `GameMinutes`) | 1 |
| Jogo | `Game` | 1 |
| _(novo)_ / Aviso | `ActionResult` | 1 |
| Apresentacao | `GameView` | 1 e 2 |
| Fazenda | `TileMap` | 2 |
| Tile | `Tile` | 3 |
| Planta | `Crop` | 3 |
| EspecieDeCultivo | `CropType` | 3 |
| Objeto | `Item` (abstrata) | 4 |
| Semente | `Seed` | 4 |
| Adubo | `Fertilizer` | 4 |
| Ferramenta | `Tool` | 4 |
| ProdutoColhido | `Produce` | 4 |
| Inventario | `Inventory` | 4 |
| Jogador | `Player` | 5 |
| CaixaDeVenda | `ShippingBin` | 5 |
| CentralDeEncomendas | `DeliveryService` | 5 |
| Loja | `Shop` (se mantida) | 5 |
| TabelaDeCultivos | `CropRegistry` | 6 |
| GerenciadorDeSave | `SaveManager` | 6 |
| LogDeErro | `ErrorLogEntry` | 6 |

## Convenções do código

- Todo código e todos os nomes em **inglês**; classes em `PascalCase`, métodos e variáveis em `camelCase`, callbacks com prefixo `on`, consultas booleanas com `is`/`has`/`can`.
- Um header por classe em `include/`, com o nome da classe (tipos pequenos sempre usados juntos podem dividir arquivo).
- Headers com Doxygen: `@brief`, `@param`, `@return`, `@pre`, `@post`, `@throws`.
- Dinheiro sempre em inteiros, nunca `float`.
- Padrão de linguagem: **C++11**.

## Documentação

A documentação dos contratos é gerada com [Doxygen](https://www.doxygen.nl/) a partir dos comentários dos headers em `include/`.

```
doxygen Doxyfile
```

_(Se o `Doxyfile` ainda não existir no repositório, gerar um com `doxygen -g` e apontar `INPUT` para `include/`.)_

## Compilação

_(A preencher quando houver código nos `.cpp` e o `Makefile` estiver definido.)_

## Pendências do grupo

- Manter a `Shop` (compra imediata), a `DeliveryService` (entrega no dia seguinte) ou as duas.
- Dormir: a partir de que horas é permitido, o que acontece às 02:00 e se o autosave ocorre às 06:00, pelo menu ou nos dois.
- Include guard: `#pragma once` ou `#ifndef`; usar ou não um namespace do projeto (os headers atuais usam `pindorama`).
- Valores de `N` (dias sem água, dias sem semente, duração da umidade) e o bônus de rendimento da planta adubada.
- O que o jogo faz quando o salvamento falha.
