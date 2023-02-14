#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N]; // adjacency lists
int p[MAX_N];   // parents
int d[MAX_N];   // depths

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree and compute parents.
void dfs(int u) {
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (d[v] == NIL) {
      d[v] = 1 + d[u];
      p[v] = u;
      dfs(v);
    }
  }
}

int naive_lca(int u, int v) {
  while (d[u] > d[v]) {
    u = p[u];
  }
  while (d[v] > d[u]) {
    v = p[v];
  }
  while (u != v) {
    u = p[u];
    v = p[v];
  }
  return u;
}

int main() {
  int n;

  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    adj[i] = d[i] = NIL;
  }
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  p[0] = NIL;
  d[0] = 0;
  dfs(0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", naive_lca(u, v));
  }

  return 0;
}
