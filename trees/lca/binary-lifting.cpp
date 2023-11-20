// Answer LCA queries via binary lifting.
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_LOG = 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int jump[MAX_LOG];
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
int num_nodes, log_2;

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

void compute_log_2() {
  for (int n = num_nodes; n > 1; n >>= 1) {
    log_2++;
  }
}

// Traverse the tree and compute depths and jump pointers.
void dfs(int u, int parent) {
  n[u].depth = 1 + n[parent].depth;
  n[u].jump[0] = parent;
  for (int i = 0; i < log_2; i++) {
    n[u].jump[i + 1] = n[n[u].jump[i]].jump[i];
  }

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      dfs(v, u);
    }
  }
}

int lca(int u, int v) {
  if (n[v].depth > n[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  // Compare d[u] and d[v] bit by bit.
  int pow = 0;
  while (n[u].depth > n[v].depth) {
    if ((n[u].depth & (1 << pow)) != (n[v].depth & (1 << pow))) {
      u = n[u].jump[pow];
    }
    pow++;
  }

  if (u == v) {
    return u;
  } else {
    for (int i = log_2; i >= 0; i--) {
      if (n[u].jump[i] != n[v].jump[i]) {
        u = n[u].jump[i];
        v = n[v].jump[i];
      }
    }

    return n[u].jump[0];
  }
}

void answer_queries() {
  int num_queries, u, v;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &u, &v);
    printf("%d\n", lca(u, v));
  }
}

int main() {
  read_input_data();
  compute_log_2();
  dfs(1, 0);
  answer_queries();

  return 0;
}
