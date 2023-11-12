#include <stdio.h>

const int MAX_NODES = 200'000;
const int NIL = -1;

struct list {
  int val, next;
};

struct node {
  int val;
  int time_in, time_out;
  int ptr; // pointer into the adjacency list
};

struct fenwick_tree {
  long long v[2 * MAX_NODES + 1];
  int n;

  void build(int n) {
    this->n = n;
    for (int i = 1; i <= n; i++) {
      int j = i + (i & -i);
      if (j <= n) {
        v[j] += v[i];
      }
    }
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  long long sum(int pos) {
    long long s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

list adj[2 * MAX_NODES];
node n[MAX_NODES + 1];
fenwick_tree fen;
int num_nodes, num_queries;

void add_neighbor(int u, int v) {
  static int ptr = 0;
  adj[ptr] = { v, n[u].ptr };
  n[u].ptr = ptr++;
}

void read_input_data() {
  scanf("%d %d", &num_nodes, &num_queries);
  for (int u = 1; u <= num_nodes; u++) {
    scanf("%d", &n[u].val);
    n[u].ptr = NIL;
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void compute_ranges(int u) {
  static int time = 0;

  n[u].time_in = ++time;

  for (int ptr = n[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!n[v].time_in) {
      compute_ranges(v);
    }
  }

  n[u].time_out = ++time;
}

void build_fenwick_tree() {
  for (int u = 1; u <= num_nodes; u++) {
    fen.v[n[u].time_in] = n[u].val;
    fen.v[n[u].time_out] = -n[u].val;
  }

  fen.build(2 * num_nodes);
}

void process_queries() {
  while (num_queries--) {
    int type, u;
    scanf("%d %d", &type, &u);
    if (type == 1) {
      int val;
      scanf("%d", &val);
      int delta = val - n[u].val;
      fen.add(n[u].time_in, delta);
      fen.add(n[u].time_out, -delta);
      n[u].val = val;
    } else {
      printf("%lld\n", fen.sum(n[u].time_in));
    }
  }
}

int main() {
  read_input_data();
  compute_ranges(1);
  build_fenwick_tree();
  process_queries();

  return 0;
}
