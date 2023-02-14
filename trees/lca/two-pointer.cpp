// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one based on this article:
// https://codeforces.com/blog/entry/74847
#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];

struct node {
  int adj;   // adjacency list
  int p;     // parent
  int d;     // depth
  int jump;  // jump pointer
} n[MAX_N];

void add_edge(int u, int v, int pos) {
  e[pos] = { v, n[u].adj };
  n[u].adj = pos;
}

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {

  int u2 = n[u].jump, u3 = n[u2].jump;

  for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (n[v].d == NIL) {
      n[v].d = 1 + n[u].d;
      n[v].p = u;
      n[v].jump = (n[u2].d - n[u].d == n[u3].d - n[u2].d) ? u3 : u;
      dfs(v);
    }
  }
}

int two_ptr_lca(int u, int v) {
  if (n[v].d > n[u].d) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (n[u].d > n[v].d) {
    u = (n[n[u].jump].d >= n[v].d) ? n[u].jump : n[u].p;
  }

  while (u != v) {
    if (n[u].jump == n[v].jump) {
      u = n[u].p;
      v = n[v].p;
    } else {
      u = n[u].jump;
      v = n[v].jump;
    }
  }
  return u;
}

int main() {
  int nodes;

  scanf("%d", &nodes);
  for (int u = 0; u < nodes; u++) {
    n[u].adj = n[u].d = NIL;
  }
  for (int i = 0; i < nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  n[0].p = NIL;
  n[0].jump = 0;
  n[0].d = 0;
  dfs(0);

  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", two_ptr_lca(u, v));
  }

  return 0;
}
