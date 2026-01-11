// Answer LCA queries via square root decomposition. Each node stores two
// ancestor pointers: one to the parent, one sqrt(n) levels above.
#include <math.h>
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
int n, jump_distance;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);
  jump_distance = sqrt(n);

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
      nd[v].jump = (nd[v].depth > jump_distance)
        ? st[nd[v].depth - jump_distance]
        : 0;
      dfs(v);
    }
  }
}

int sqrt_lca(int u, int v) {
  if (nd[v].depth > nd[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (nd[u].depth - jump_distance >= nd[v].depth) {
    u = nd[u].jump;
  }
  while (nd[u].depth > nd[v].depth) {
    u = nd[u].parent;
  }

  // Climb entire blocks.
  while ((nd[u].depth >= jump_distance) && (nd[u].jump != nd[v].jump)) {
    u = nd[u].jump;
    v = nd[v].jump;
  }

  // Climb single levels.
  while (u != v) {
    u = nd[u].parent;
    v = nd[v].parent;
  }
  return u;
}

void answer_queries() {
  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", sqrt_lca(u, v));
  }
}

int main() {
  read_input_data();
  nd[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
