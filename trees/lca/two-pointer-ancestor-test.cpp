// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one based on this article:
// https://codeforces.com/blog/entry/74847
// Uses the ancestor test.
#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];

struct node {
  int adj;     // adjacency list
  int in, out; // discovery / return times
  int d;       // depth
  int p;       // parent
  int jump;    // jump pointer
} n[MAX_N];

int timer;

void add_edge(int u, int v, int pos) {
  e[pos] = { v, n[u].adj };
  n[u].adj = pos;
}

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {
  n[u].in = timer++;
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
  n[u].out = timer++;
}

// Returns true iff u is an ancestor of v *or* v itself.
bool is_ancestor(int u, int v) {
  return (n[u].in <= n[v].in) && (n[u].out >= n[v].out);
}

int two_ptr_lca(int u, int v) {
  // Find the lowest ancestor of u that is also an ancestor of v.
  while (!is_ancestor(u, v)) {
    if (is_ancestor(n[u].jump, v)) {
      u = n[u].p;
    } else {
      u = n[u].jump;
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
