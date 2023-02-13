// Answer LCA queries via square root decomposition. Each node stores two
// ancestor pointers: one to the parent, one sqrt(n) levels above.
#include <math.h>
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
int jump_distance;

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
      jump[v] = (d[v] >= jump_distance) ? st[d[v] - jump_distance] : NIL;
      dfs(v);
    }
  }
}

int sqrt_lca(int u, int v) {
  if (d[v] > d[u]) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (d[u] - jump_distance >= d[v]) {
    u = jump[u];
  }
  while (d[u] > d[v]) {
    u = p[u];
  }

  // Climb entire blocks.
  while ((d[u] >= jump_distance) && (jump[u] != jump[v])) {
    u = jump[u];
    v = jump[v];
  }

  // Climb single levels.
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

  jump_distance = sqrt(n);
  p[0] = jump[0] = NIL;
  d[0] = 0;
  dfs(0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", sqrt_lca(u, v));
  }
}
