// Answer LCA queries via binary lifting. Uses the ancestor test.
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_LOG = 19;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int time_in, time_out;
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

// Traverse the tree and compute node timers and jump pointers.
void dfs(int u, int parent) {
  static int time = 0;
  n[u].time_in = time++;

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

  n[u].time_out = time++;
}

bool is_ancestor(int u, int v) {
  return
    (n[u].time_in <= n[v].time_in) &&
    (n[u].time_out >= n[v].time_out);
}

int lca(int u, int v) {
  if (is_ancestor(u, v)) {
    return u;
  }

  // Find the highest ancestor of u that is *not* an ancestor of v.
  for (int i = log_2; i >= 0; i--) {
    if (n[u].jump[i] && !is_ancestor(n[u].jump[i], v)) {
      u = n[u].jump[i];
    }
  }

  // Now u's parent *is* an ancestor of v.
  return n[u].jump[0];
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
