// Answer LCA queries via binary lifting. Uses the ancestor test.
#include <stdio.h>

#define MAX_N 500'000
#define MAX_LOG 19
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N];            // adjacency lists
int jump[MAX_N][MAX_LOG];  // jump pointers
int in[MAX_N], out[MAX_N]; // discovery / return times
int l2;                    // log_2(n)
int timer;

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree and compute node timers and jump pointers.
void dfs(int u, int parent) {
  in[u] = timer++;
  jump[u][0] = parent;
  for (int i = 0; i < l2; i++) {
    jump[u][i + 1] = jump[jump[u][i]][i];
  }

  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != parent) {
      dfs(v, u);
    }
  }
  out[u] = timer++;
}

// Returns true iff u is an ancestor of v *or* v itself.
bool is_ancestor(int u, int v) {
  return (in[u] <= in[v]) && (out[u] >= out[v]);
}

int lca(int u, int v) {
  if (is_ancestor(u, v)) {
    return u;
  }

  // Find the highest ancestor of u that is *not* an ancestor of v.
  for (int i = l2; i >= 0; i--) {
    if (!is_ancestor(jump[u][i], v)) {
      u = jump[u][i];
    }
  }

  // Now u's parent *is* an ancestor of v.
  return jump[u][0];
}

int main() {
  int n;

  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    adj[i] = NIL;
  }
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  // Compute floor(log2(n)) (the most we can ever climb at once).
  for (int nc = n; nc > 1; nc >>= 1) {
    l2++;
  }

  dfs(0, 0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", lca(u, v));
  }

  return 0;
}
