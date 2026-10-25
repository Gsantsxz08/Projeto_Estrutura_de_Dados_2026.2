# B-Tree

Implementation in C of a B-tree, a self-balancing search tree in which each node holds several keys and can have several children. It is defined by a minimum degree `t`: every node except the root has at least `t-1` keys, and every node has at most `2t-1`. All leaves stay at the same depth, which keeps search, insertion and removal efficient. Full nodes are split during insertion, and removal uses borrowing or merging of nodes to keep the minimum number of keys.

Supported operations: insert, remove, search and print the tree level by level. Duplicate keys are not inserted.

## Getting the code

```
git clone https://github.com/Gsantsxz08/Projeto_Estrutura_de_Dados_2026.2.git
cd Projeto_Estrutura_de_Dados_2026.2/"1 - seminario"/"Árvore B"
```

Or download the repository as a ZIP from GitHub (Code > Download ZIP), extract it, and enter the folder above. Some folder names have spaces, so keep the quotes in `cd`.

## Compilation

From inside this folder:

```
gcc -Wall -o arvore_b arvore_b.c
```

## Execution

```
./arvore_b
```

The program first asks for the minimum degree `t` (at least 2) and then shows a menu (in Portuguese):

```
1 - Inserir
2 - Remover
3 - Buscar
4 - Imprimir
5 - Demonstracao
0 - Sair
```

Option 5 inserts a fixed list of values and then removes some of them, printing the tree after each step. Each line of the output is one level of the tree, with the keys of each node between brackets. With `t = 2`, the demonstration ends with:

```
Nivel 0: [3 10 17]
Nivel 1: [1] [5] [12] [25 30 40]
```

## Requirements

- Linux or macOS with `gcc`.
- Not for Windows; please use WSL or MinGW.
