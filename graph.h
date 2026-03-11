#ifndef GRAPH_H
#define GRAPH_H

#include <limits.h>

#define MAXN 300
#define MAX_NAME 64
#define MAX_PATH_LEN 1024

typedef struct Edge {
    int to, weight;
    struct Edge *next;
} Edge;

typedef struct {
    char name[MAX_NAME];
    Edge *adj;
} Node;

extern Node graph[MAXN];
extern int nodeCount;

/* node helpers */
int getNodeIndex(const char *name);
int findNodeIndex(const char *name);

/* graph load */
void addEdge(const char *a, const char *b, int w);
void loadGraph(const char *filename);


int dijkstra_with_path(const char *src, const char *dest, char *route_buf, int buf_len);

#endif /* GRAPH_H */
