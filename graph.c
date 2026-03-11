#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

Node graph[MAXN];
int nodeCount = 0;

int getNodeIndex(const char *name) {
    for (int i = 0; i < nodeCount; i++)
        if (strcmp(graph[i].name, name) == 0)
            return i;

    if (nodeCount >= MAXN)
        return -1;

    strncpy(graph[nodeCount].name, name, MAX_NAME - 1);
    graph[nodeCount].name[MAX_NAME - 1] = '\0';
    graph[nodeCount].adj = NULL;
    return nodeCount++;
}

int findNodeIndex(const char *name) {
    for (int i = 0; i < nodeCount; i++)
        if (strcmp(graph[i].name, name) == 0)
            return i;

    return -1;
}

void addEdge(const char *a, const char *b, int w) {
    int u = getNodeIndex(a);
    int v = getNodeIndex(b);

    if (u < 0 || v < 0)
        return;

    Edge *e = malloc(sizeof(Edge));
    e->to = v;
    e->weight = w;
    e->next = graph[u].adj;
    graph[u].adj = e;

    Edge *r = malloc(sizeof(Edge));
    r->to = u;
    r->weight = w;
    r->next = graph[v].adj;
    graph[v].adj = r;
}

void loadGraph(const char *filename) {
    for (int i = 0; i < nodeCount; i++) {
        Edge *e = graph[i].adj;
        while (e) {
            Edge *t = e;
            e = e->next;
            free(t);
        }
        graph[i].adj = NULL;
    }
    nodeCount = 0;

    FILE *f = fopen(filename, "r");
    if (!f)
        return;

    char a[MAX_NAME], b[MAX_NAME];
    int w;

    while (fscanf(f, "%63s %63s %d", a, b, &w) == 3)
        addEdge(a, b, w);

    fclose(f);
}

/* ---------- MIN HEAP FOR DIJKSTRA ---------- */
typedef struct {
    int v;
    int dist;
} HN;

typedef struct {
    HN *data;
    int size, cap;
} MinH;

static MinH* mh_create(int cap) {
    MinH *h = malloc(sizeof(MinH));
    h->data = malloc(sizeof(HN) * cap);
    h->size = 0;
    h->cap = cap;
    return h;
}

static void mh_swap(HN *a, HN *b) {
    HN t = *a;
    *a = *b;
    *b = t;
}

static void mh_push(MinH *h, int v, int dist) {
    if (h->size >= h->cap) {
        h->cap *= 2;
        h->data = realloc(h->data, sizeof(HN) * h->cap);
    }

    int i = h->size++;
    h->data[i].v = v;
    h->data[i].dist = dist;

    while (i > 0) {
        int p = (i - 1) >> 1;
        if (h->data[p].dist <= h->data[i].dist)
            break;

        mh_swap(&h->data[p], &h->data[i]);
        i = p;
    }
}

static HN mh_pop(MinH *h) {
    HN r = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int l = i * 2 + 1;
        int rch = i * 2 + 2;
        int s = i;

        if (l < h->size && h->data[l].dist < h->data[s].dist) s = l;
        if (rch < h->size && h->data[rch].dist < h->data[s].dist) s = rch;

        if (s == i)
            break;

        mh_swap(&h->data[i], &h->data[s]);
        i = s;
    }

    return r;
}

static void mh_free(MinH *h) {
    free(h->data);
    free(h);
}

/* ---------- DIJKSTRA WITH PATH ---------- */
int dijkstra_with_path(const char *src, const char *dest, char *route_buf, int buf_len) {
    int s = findNodeIndex(src);
    int d = findNodeIndex(dest);

    if (s < 0 || d < 0)
        return INT_MAX;

    int n = nodeCount;
    int *dist = malloc(sizeof(int) * n);
    int *vis = calloc(n, sizeof(int));
    int *parent = malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        parent[i] = -1;
    }

    dist[s] = 0;

    MinH *h = mh_create(n > 16 ? n : 16);
    mh_push(h, s, 0);

    while (h->size) {
        HN cur = mh_pop(h);
        int v = cur.v;
        int cd = cur.dist;
        if (vis[v])
            continue;

        vis[v] = 1;

        if (v == d)
            break;

        for (Edge *e = graph[v].adj; e; e = e->next) {
            int to = e->to;
            int nd = cd + e->weight;

            if (nd < dist[to]) {
                dist[to] = nd;
                parent[to] = v;
                mh_push(h, to, nd);
            }
        }
    }

    int result = dist[d];

    if (result == INT_MAX) {
        free(dist); free(vis); free(parent);
        mh_free(h);
        return INT_MAX;
    }
    const char *names[256];
    int cnt = 0;

    int cur = d;
    while (cur != -1 && cnt < 256) {
        names[cnt++] = graph[cur].name;
        cur = parent[cur];
    }

    route_buf[0] = '\0';

    for (int i = cnt - 1; i >= 0; i--) {
        strncat(route_buf, names[i], buf_len - strlen(route_buf) - 1);
        if (i != 0)
            strncat(route_buf, " -> ", buf_len - strlen(route_buf) - 1);
    }

    free(dist);
    free(vis);
    free(parent);
    mh_free(h);

    return result;
}
