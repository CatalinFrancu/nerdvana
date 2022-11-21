// Topological sort of a directed graph (BFS method).
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

int adj[MAX_NODES]; // start of adjacency list
int d[MAX_NODES];   // in-degree

edge e[2 * MAX_EDGES];
int q[MAX_NODES]; // queue
int nodes, edges;

void add_edge(int u, int v) {
  e[edges] = { v, adj[u] };
  adj[u] = edges++;
  d[v]++;
}

void top_sort() {
  int head = 0, tail = 0;

  for (int u = 0; u < nodes; u++) {
    if (!d[u]) {
      q[tail++] = u;
    }
  }

  while (head != tail) {
    int u = q[head++];
    printf("%d\n", u);
    for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      d[v]--;
      if (!d[v]) {
        q[tail++] = v;
      }
    }
  }
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    adj[u] = NIL;
    d[u] = 0;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
  }

  top_sort();

  return 0;
}
