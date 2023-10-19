# Dicionário SkipList

<br>

#### Repositório: [https://github.com/bcmaia/skiplist](https://github.com/bcmaia/skiplist)

#### Estudantes: 
- Bernardo Maia Coelho - 12542481
- Rodrigo de Freitas Lima - 12547510

<br>

Este programa em C fornece uma interface de linha de comando para executar várias operações em um dicionário. Este projeto faz parte da disciplina Estruturas de Dados Abstratas, com o objetivo de implementar a estrutura de dados SkipList em C e demonstrar seu uso.


## Visão Geral

Uma SkipList é uma estrutura de dados que permite a inserção, exclusão e busca eficientes de itens. Este programa aceita comandos do usuário para realizar essas operações.

## Documentação

Documentamos o projeto seguindo os padrões do Doxygen. Você pode encontrar a saída do Doxygen na pasta `./doc`.

## Estrutura do Código

O programa é composto pelos seguintes componentes principais:

1. **Laço de Execução**: O núcleo do programa é um loop infinito que aguarda a entrada do usuário e executa várias operações com base no comando fornecido.

2. **Operações**:

    - **Inserção**: O usuário pode inserir um item na SkipList usando o comando "insercao". O programa lê um item, insere-o na SkipList e lida com quaisquer erros.

    - **Atualização**: O comando "alteracao" permite que o usuário atualize um item existente na SkipList. Ele lê um item, realiza a atualização e lida com erros.

    - **Remoção**: O comando "remocao" é usado para remover um item da SkipList. Ele lê um item, remove-o da SkipList e gerencia erros.

    - **Busca**: O comando "busca" permite que o usuário procure um item na SkipList. Ele lê um item, realiza a busca e imprime o resultado ou lida com erros.

    - **Impressão**: O comando "impressao" é usado para imprimir a SkipList com diferentes opções de formatação. O programa lê o especificador de formato e imprime a SkipList de acordo com ele, tratando erros.

    - **Depuração**: O comando "debug" imprime o estado atual da SkipList para fins de depuração.

    - **Operação Não Identificada**: Se o programa encontrar um comando não reconhecido, ele exibe "OPERACAO INVALIDA".

3. **Limpeza**: Após o usuário sair do programa, ele desaloca a memória e faz a limpeza da SkipList para evitar vazamentos de memória.

## Uso

Para usar o programa, compile-o e execute o executável. Você pode então inserir comandos e dados para realizar operações na SkipList. Comandos inválidos resultarão na mensagem "OPERACAO INVALIDA".

Este programa oferece uma maneira versátil e interativa de trabalhar com SkipLists, permitindo a inserção, atualização, remoção, busca e visualização do conteúdo da estrutura de dados.

## Compilação

### Sistema de Compilação, Linguagem e Ferramentas

Utilizamos um Makefile robusto para simplificar o processo de desenvolvimento. Você pode encontrar mais informações sobre este Makefile em [github.com/bcmaia/MasterMaker](https://github.com/bcmaia/MasterMaker). O código foi compilado e testado usando `gcc` com os padrões C99, C11 e C17, funcionando conforme o esperado para essas especificações.

### Ambiente

Testamos o programa em um sistema Linux x86 `Pop!_OS 22.04 LTS`. Portanto, deve funcionar na maioria das distribuições Linux baseadas no Debian mais modernas.

### Configuração

Para baixar as ferramentas necessárias no Pop!_OS ou na maioria das distribuições Linux baseadas no Debian:

Para baixar as ferramentas de compilação:

```bash
sudo apt update
sudo apt install -y build-essential
```

Certifique-se de ter o Git instalado:

```bash
sudo apt install -y git
```

Para clonar o repositório:

```bash
git clone https://github.com/bcmaia/skiplist
```

### Compilação

Para compilar o projeto, navegue até o diretório do projeto e execute:

```bash
make all
```

Para executar:

```bash
make run
```