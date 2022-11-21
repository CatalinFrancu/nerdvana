// Topological sort of a directed graph (DFS method).
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

int adj[MAX_NODES];   // start of adjacency list
int seen[MAX_NODES];  // mark visited nodes
int ord[MAX_NODES];   // sorted nodes go here
edge e[2 * MAX_EDGES];
int nodes, edges, pos;

void add_edge(int u, int v) {
  e[edges] = { v, adj[u] };
  adj[u] = edges++;
}

void dfs(int u) {
  seen[u] = true;
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (!seen[v]) {
      dfs(v);
    }
  }
  ord[pos--] = u;
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    adj[u] = NIL;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
  }

  pos = nodes - 1;
  for (u = 0; u < nodes; u++) {
    if (!seen[u]) {
      dfs(u);
    }
  }

  for (int i = 0; i < nodes; i++) {
    printf("%d ", ord[i]);
  }
  printf("\n");

  return 0;
}
