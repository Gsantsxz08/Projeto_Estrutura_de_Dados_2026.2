# Dijkstra's Algorithm

Implementation in C of Dijkstra's algorithm, which finds the shortest paths from a source vertex to all other vertices of a weighted graph with positive edge weights. The graph is stored as an adjacency matrix. At each step, the algorithm picks the unvisited vertex with the smallest known distance (by a linear scan) and relaxes the edges that leave it.

## Getting the code

```
git clone https://github.com/Gsantsxz08/Projeto_Estrutura_de_Dados_2026.2.git
cd Projeto_Estrutura_de_Dados_2026.2/"1 - seminario"/dijkstra
```

Or download the repository as a ZIP from GitHub (Code > Download ZIP), extract it, and enter the folder above. Some folder names have spaces, so keep the quotes in `cd`.

## Compilation

From inside this folder:

```
gcc -Wall -o dijkstra dijkstra.c
```

## Execution

```
./dijkstra
```

The program builds an example graph with 4 vertices (edges 0-1 with weight 5, 1-2 with weight 3, and 2-3 with weight 7), prints its adjacency matrix and the shortest distances from vertex 0:

```
Vertice 0: 0
Vertice 1: 5
Vertice 2: 8
Vertice 3: 15
```

A vertex that cannot be reached from the source is printed with distance `999999` (the value used as infinity).

To use another graph, change `MAX` (number of vertices) and the calls to `adicionar_arestas` in `main`. The last argument is `0` for an undirected edge and `1` for a directed one.

## Requirements

- Linux or macOS with `gcc`.
- Not for Windows; please use WSL or MinGW.
