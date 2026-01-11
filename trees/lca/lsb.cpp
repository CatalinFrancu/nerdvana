// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one going back LSB(depth)
// bits (roughly like a Fenwick tree).
#include <stdio.h>

const int MAX_NODES = 500'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int parent;
  int jump;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int st[MAX_NODES + 1];    // DFS stack for building the jump pointers
int n;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {
  st[nd[u].depth] = u;  // The stack holds the gray nodes.

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].depth) {
      nd[v].depth = 1 + nd[u].depth;
      nd[v].parent = u;
      nd[v].jump = st[nd[v].depth & (nd[v].depth - 1)];
      dfs(v);
    }
  }
}

int two_ptr_lca(int u, int v) {
  if (nd[v].depth > nd[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (nd[u].depth > nd[v].depth) {
    u = (nd[nd[u].jump].depth >= nd[v].depth) ? nd[u].jump : nd[u].parent;
  }

  while (u != v) {
    if (nd[u].jump != nd[v].jump) {
      u = nd[u].jump;
      v = nd[v].jump;
    } else {
      u = nd[u].parent;
      v = nd[v].parent;
    }
  }
  return u;
}

void answer_queries() {
  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", two_ptr_lca(u, v));
  }
}

int main() {
  read_input_data();
  nd[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
