#include <stdio.h>
#include <stdlib.h>

#define MAX 4
#define INF 999999

typedef struct 
{
    int matriz[MAX][MAX];
}Grafo;

void inicializar(Grafo *g)
{
    int i,j;
    for(i = 0 ; i < MAX ; i++)
    {
        for(j = 0 ; j < MAX ; j++)
        {
            g->matriz[i][j] = 0;
        }
    }
}

void adicionar_arestas(Grafo *g, int u, int v , int peso, int direcionado)
{
    if((g == NULL) || (u < 0 || u >= MAX) || (v < 0 || v >= MAX) || (peso <= 0))
    {
        return;
    }
    if(direcionado != 0 && direcionado != 1)
    {
        return;
    }
    g->matriz[u][v] = peso;
    if(direcionado == 0)
    {
        g->matriz[v][u] = peso;
    }
}

void imprimir_grafo(Grafo *g)
{
    int i,j;
    for(i = 0 ; i < MAX ; i++)
    {
        for(j = 0 ; j < MAX ; j++)
        {
            printf("%d ", g->matriz[i][j]);
        }
        printf("\n");
    }
}

void inicializar_visitado(int visitado[])
{
    int i;
    for(i = 0 ; i < MAX ; i++)
    {
        visitado[i] = 0;
    }
}

void inicializar_distancia(int distancia[], int origem)
{
    int i;
    for(i = 0 ; i < MAX  ; i++)
    {
        distancia[i] = INF;
    }
    distancia[origem] = 0;
}

int encontrar_menor(int distancia[], int visitado[])
{
    int i;
    int menor = INF;
    int indice_menor = -1;
    for(i = 0 ; i < MAX ; i++)
    {
        if(visitado[i] != 1 &&  distancia[i] < menor)
        {
            menor = distancia[i];
            indice_menor = i;
        }
    }
    return indice_menor;
}

void imprimir_distancia(int distancia[])
{
    int i;
    for(i = 0 ; i < MAX ; i++)
    {
        printf("Vertice %d: %d\n", i, distancia[i]);
    }
}

void dijkstra(Grafo *g, int origem)
{
    int i, u, v;
    int distancia[MAX];
    int visitado[MAX];
    int nova_distancia;
    if((g == NULL) || (origem < 0) || (origem >= 4))
    {
        return;
    }
    inicializar_distancia(distancia, origem);
    inicializar_visitado(visitado);
    for(i = 0 ; i < MAX ; i++)
    {
        u = encontrar_menor(distancia, visitado);
        if(u == -1)
        {
            break;
        }
        visitado[u] = 1;
        for(v = 0 ; v < MAX ; v++)
        {
            if(g->matriz[u][v] != 0 && visitado[v] != 1)
            {
                nova_distancia = distancia[u] + g->matriz[u][v];
                if(nova_distancia < distancia[v])
                {
                    distancia[v] = nova_distancia;
                }
            }
        }
    }
    imprimir_distancia(distancia);
}

int main()
{
    Grafo g;
    inicializar(&g);
    adicionar_arestas(&g, 0, 1, 5, 0);
    adicionar_arestas(&g, 1, 2, 3, 0);
    adicionar_arestas(&g,2 ,3 , 7, 0);
    imprimir_grafo(&g);
    dijkstra(&g, 0);
    return 0;
}