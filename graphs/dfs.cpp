// Depth-first search on a directed graph.
// We don't store colors explicityly. Instead,
// * white nodes have d[u] = NIL;
// * gray nodes have a value for d[u], but not for f[u];
// * black nodes have values for both d[u] and f[u].
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

// consider encapsulating these in a node structure
int adj[MAX_NODES];              // start of adjacency list
int d[MAX_NODES], f[MAX_NODES];  // discovery and finish time
int p[MAX_NODES];                // parent in DFS tree
edge e[2 * MAX_EDGES];
int nodes, edges, time;

void add_edge(int u, int v) {
  e[edges] = { v, adj[u] };
  adj[u] = edges++;
}

void dfs(int u) {
  d[u] = time++;
  printf("Time %2d: discovered %d\n", d[u], u);
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (d[v] == NIL) {
      p[v] = u;
      dfs(v);
    }
  }
  f[u] = time++;
  printf("Time %2d: left %d\n", f[u], u);
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    adj[u] = d[u] = NIL;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
  }

  time = 0;
  for (u = 0; u < nodes; u++) {
    if (d[u] == NIL) {
      dfs(u);
    }
  }

  return 0;
}
