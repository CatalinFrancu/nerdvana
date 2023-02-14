// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one going back LSB(depth)
// bits (roughly like a Fenwick tree).
#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N];   // adjacency lists
int p[MAX_N];     // parents
int d[MAX_N];     // depths
int jump[MAX_N];  // jump pointers
int st[MAX_N];    // DFS stack for building the jump pointers

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {
  st[d[u]] = u;  // Now st[0...d[u]] holds the gray nodes.

  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (d[v] == NIL) {
      d[v] = 1 + d[u];
      p[v] = u;
      jump[v] = st[d[v] & (d[v] - 1)];
      dfs(v);
    }
  }
}

int two_ptr_lca(int u, int v) {
  if (d[v] > d[u]) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (d[u] > d[v]) {
    u = (d[jump[u]] >= d[v]) ? jump[u] : p[u];
  }

  while (u != v) {
    if (jump[u] == jump[v]) {
      u = p[u];
      v = p[v];
    } else {
      u = jump[u];
      v = jump[v];
    }
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
  jump[0] = 0;
  d[0] = 0;
  dfs(0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", two_ptr_lca(u, v));
  }

  return 0;
}
