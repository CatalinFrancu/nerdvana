// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one based on this article:
// https://codeforces.com/blog/entry/74847
// Uses the ancestor test.
#include <stdio.h>

const int MAX_NODES = 500'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int time_in, time_out;
  int parent;
  int jump;
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

// Traverse the tree and compute parents, depths and jump pointers.
void dfs(int u) {
  static int time = 1;
  n[u].time_in = time++;

  int u2 = n[u].jump, u3 = n[u2].jump;
  bool equal = (n[u2].depth - n[u].depth == n[u3].depth - n[u2].depth);

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!n[v].depth) {
      n[v].depth = 1 + n[u].depth;
      n[v].parent = u;
      n[v].jump = equal ? u3 : u;
      dfs(v);
    }
  }

  n[u].time_out = time++;
}

bool is_ancestor(int u, int v) {
  return
    (n[u].time_in <= n[v].time_in) &&
    (n[u].time_out >= n[v].time_out);
}

int two_ptr_lca(int u, int v) {
  // Find the lowest ancestor of u that is also an ancestor of v.
  while (!is_ancestor(u, v)) {
    if (n[u].jump && !is_ancestor(n[u].jump, v)) {
      u = n[u].jump;
    } else {
      u = n[u].parent;
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
  n[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
