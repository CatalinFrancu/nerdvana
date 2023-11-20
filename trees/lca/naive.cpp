#include <stdio.h>

const int MAX_NODES = 500'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int parent;
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

// Traverse the tree and compute parents and depths.
void dfs(int u) {
  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].depth) {
      n[v].depth = 1 + n[u].depth;
      n[v].parent = u;
      dfs(v);
    }
  }
}

int naive_lca(int u, int v) {
  while (n[u].depth > n[v].depth) {
    u = n[u].parent;
  }
  while (n[v].depth > n[u].depth) {
    v = n[v].parent;
  }
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
    printf("%d\n", naive_lca(u, v));
  }
}

int main() {
  read_input_data();
  n[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
