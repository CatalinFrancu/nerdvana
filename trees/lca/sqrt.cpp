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
node n[MAX_NODES + 1];
int st[MAX_NODES + 1];    // DFS stack for building the jump pointers
int num_nodes, jump_distance;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &num_nodes);
  jump_distance = sqrt(num_nodes);

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {
  st[n[u].depth] = u;  // The stack holds the gray nodes.
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].depth) {
      n[v].depth = 1 + n[u].depth;
      n[v].parent = u;
      n[v].jump = (n[v].depth > jump_distance)
        ? st[n[v].depth - jump_distance]
        : 0;
      dfs(v);
    }
  }
}

int sqrt_lca(int u, int v) {
  if (n[v].depth > n[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (n[u].depth - jump_distance >= n[v].depth) {
    u = n[u].jump;
  }
  while (n[u].depth > n[v].depth) {
    u = n[u].parent;
  }

  // Climb entire blocks.
  while ((n[u].depth >= jump_distance) && (n[u].jump != n[v].jump)) {
    u = n[u].jump;
    v = n[v].jump;
  }

  // Climb single levels.
  while (u != v) {
    u = n[u].parent;
    v = n[v].parent;
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
  n[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
