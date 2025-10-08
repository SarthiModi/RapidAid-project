#include <stdio.h>
#include "graph.h"

int graph[N][N];

void initGraph() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            graph[i][j] = 0;
}

void addRoad(int u, int v, int w) {
    graph[u][v] = w;
    graph[v][u] = w;
}

void buildCity() {
    addRoad(0, 1, 2);
    addRoad(0, 2, 4);
    addRoad(1, 2, 1);
    addRoad(1, 3, 7);
    addRoad(2, 4, 3);
    addRoad(3, 4, 2);
}

void printGraph() {
    printf("\nCity Map (Adjacency Matrix):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%2d ", graph[i][j]);
        printf("\n");
    }
}
