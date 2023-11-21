// Answer LCA queries using Tarjan's offline algorithm.
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_QUERIES = 500'000;

struct cell {
  int val, next;
};

struct node {
  int ptr, qptr; // adjacency list and query list
  int parent;
};

struct query {
  int u, v, lca;
};

cell list[2 * MAX_NODES];
cell qlist[2 * MAX_QUERIES];
node n[MAX_NODES + 1];
query q[MAX_QUERIES];
int ds_parent[MAX_NODES + 1];
int num_nodes, num_queries;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].ptr };
  n[u].ptr = pos++;
}

void add_query(int ind, int u) {
  static int pos = 1;
  qlist[pos] = { ind, n[u].qptr };
  n[u].qptr = pos++;
}

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int i = 0; i < num_nodes - 1; i++) {
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

void ds_init() {
  for (int u = 1; u <= num_nodes; u++) {
    ds_parent[u] = u;
  }
}

int ds_find(int u) {
  return (ds_parent[u] == u)
    ? u
    : (ds_parent[u] = ds_find(ds_parent[u]));
}

// Always merges v into u. No union by rank.
void ds_union(int u, int v) {
  ds_parent[ds_find(v)] = ds_find(u);
}


void offline_lca(int u, int parent) {
  n[u].parent = parent;

  for (int ptr = n[u].ptr; ptr; ptr = list[ptr].next) {
    int v = list[ptr].val;
    if (v != parent) {
      offline_lca(v, u);
      ds_union(u, v);
    }
  }

  for (int ptr = n[u].qptr; ptr; ptr = qlist[ptr].next) {
    int i = qlist[ptr].val;
    int v = (q[i].u == u) ? q[i].v : q[i].u;
    if (n[v].parent) {
      q[i].lca = ds_find(v);
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
  ds_init();
  offline_lca(1, 0);
  answer_queries();

  return 0;
}
