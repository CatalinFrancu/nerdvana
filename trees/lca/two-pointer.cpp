// Answer LCA queries via binary lifting with only two pointers. Each node
// stores two ancestor pointers: one to the parent, one based on this article:
// https://codeforces.com/blog/entry/74847
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_LOG = 19;

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
}

int two_ptr_lca(int u, int v) {
  if (n[v].depth > n[u].depth) {
    int tmp = u; u = v; v = tmp;
  }

  // First bring them on the same level.
  while (n[u].depth > n[v].depth) {
    u = (n[n[u].jump].depth >= n[v].depth) ? n[u].jump : n[u].parent;
  }

  while (u != v) {
    if (n[u].jump != n[v].jump) {
      u = n[u].jump;
      v = n[v].jump;
    } else {
      u = n[u].parent;
      v = n[v].parent;
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
