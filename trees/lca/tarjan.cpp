// Answer LCA queries using Tarjan's offline algorithm.
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_QUERIES = 500'000;

struct disjoint_set_forest {
  int p[MAX_NODES + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  // Always merges v into u. No union by rank.
  void unite(int u, int v) {
    p[find(v)] = find(u);
  }
};

struct cell {
  int val, next;
};

struct node {
  int ptr, qptr; // adjacency list and query list
  bool visited;
};

struct query {
  int u, v, lca;
};

cell list[2 * MAX_NODES];
cell qlist[2 * MAX_QUERIES];
node nd[MAX_NODES + 1];
query q[MAX_QUERIES];
disjoint_set_forest dsf;
int n, num_queries;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].ptr };
  nd[u].ptr = pos++;
}

void add_query(int ind, int u) {
  static int pos = 1;
  qlist[pos] = { ind, nd[u].qptr };
  nd[u].qptr = pos++;
}

void read_input_data() {
  scanf("%d", &n);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].u, &q[i].v);
    add_query(i, q[i].u);
    add_query(i, q[i].v);
  }
}

void offline_lca(int u) {
  nd[u].visited = true;

  for (int ptr = nd[u].ptr; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (!nd[v].visited) {
      offline_lca(v);
      dsf.unite(u, v);
    }
  }

  for (int ptr = nd[u].qptr; ptr; ptr = qlist[ptr].next) {
    int i = qlist[ptr].val;
    int v = (q[i].u == u) ? q[i].v : q[i].u;
    if (nd[v].visited) {
      q[i].lca = dsf.find(v);
    }
  }
}

void answer_queries() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", q[i].lca);
  }
}

int main() {
  read_input_data();
  dsf.init(n);
  offline_lca(1);
  answer_queries();

  return 0;
}
