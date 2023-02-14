// Answer LCA queries via binary lifting.
#include <stdio.h>

#define MAX_N 500'000
#define MAX_LOG 19
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N];            // adjacency lists
int d[MAX_N];              // depths
int jump[MAX_N][MAX_LOG];  // jump pointers
int l2;                    // log_2(n)

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree and compute jump pointers.
void dfs(int u, int parent) {
  jump[u][0] = parent;
  for (int i = 0; i < l2; i++) {
    jump[u][i + 1] = jump[jump[u][i]][i];
  }

  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != parent) {
      d[v] = 1 + d[u];
      dfs(v, u);
    }
  }
}

int lca(int u, int v) {
  if (d[v] > d[u]) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  // Compare d[u] and d[v] bit by bit.
  int pow = 0;
  while (d[u] > d[v]) {
    if ((d[u] & (1 << pow)) != (d[v] & (1 << pow))) {
      u = jump[u][pow];
    }
    pow++;
  }

  if (u == v) {
    return u;
  } else {
    for (int i = l2; i >= 0; i--) {
      if (jump[u][i] != jump[v][i]) {
        u = jump[u][i];
        v = jump[v][i];
      }
    }

    return jump[u][0];
  }
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

  for (int nc = n; nc > 1; nc >>= 1) {
    l2++;
  }

  d[0] = 0;
  dfs(0, 0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", lca(u, v));
  }

  return 0;
}
