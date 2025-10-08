#ifndef AMBULANCE_H
#define AMBULANCE_H

#define MAX 10        // Maximum nodes in graph
#define INF 1000000    // A very large number (infinity)

// ---------------- Graph Structures ----------------
typedef struct Edge {
    int to;             // destination node
    int weight;         // edge weight (distance)
    struct Edge* next;  // pointer to next edge
} Edge;

typedef struct {
    int n;              // number of nodes
    Edge* list[MAX];    // adjacency list
} Graph;

// ---------------- Function Prototypes ----------------

// Graph functions
void initGraph(Graph* g, int n);
void addEdge(Graph* g, int u, int v, int w);

// Shortest path (Dijkstra)
void dijkstra(Graph* g, int src, int dist[], int parent[]);

// Build actual path (from parent array)
void buildPath(int parent[], int src, int dest, int path[], int* pathLen);

#endif
