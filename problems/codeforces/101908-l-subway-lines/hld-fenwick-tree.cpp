// v2: Use a Fenwick tree with range add support instead of the segment tree.
#include <stdio.h>

const int MAX_NODES = 100'000;

struct fenwick_tree {
  int v[MAX_NODES + 2];
  int n;

  void init(int _n) {
    n = _n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int prefix_sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

struct fenwick_tree_2 {
  fenwick_tree v, w;

  void init(int n) {
    v.init(n);
    w.init(n);
  }

  void range_add(int l, int r, int val) {
    v.add(l, val);
    v.add(r + 1, -val);
    w.add(l, -val * (l - 1));
    w.add(r + 1, val * r);
  }

  int prefix_sum(int pos) {
    return v.prefix_sum(pos) * pos + w.prefix_sum(pos);
  }

  int range_sum(int l, int r) {
    return prefix_sum(r) - prefix_sum(l - 1);
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;  // heavy child
  int head;   // beginning of our chain
  int pos;    // DFS discovery time
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree_2 fen;
int n, num_queries;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_queries);
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

void decompose_dfs(int u, int head) {
  static int time = 1;

  nd[u].head = head;
  nd[u].pos = time++;

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // v starts a new chain
    }
  }
}

void path_add(int u, int v, int delta) {
  while (nd[u].head != nd[v].head) {
    if (nd[v].pos > nd[u].pos) {
      int tmp = u; u = v; v = tmp;
    }
    fen.range_add(nd[nd[u].head].pos, nd[u].pos, delta);
    u = nd[nd[u].head].parent;
  }
  if (nd[u].pos > nd[v].pos) {
    int tmp = u; u = v; v = tmp;
  }
  fen.range_add(nd[u].pos, nd[v].pos, delta);
}

int path_sum(int u, int v) {
  int result = 0;

  while (nd[u].head != nd[v].head) {
    if (nd[v].pos > nd[u].pos) {
      int tmp = u; u = v; v = tmp;
    }
    result += fen.range_sum(nd[nd[u].head].pos, nd[u].pos);
    u = nd[nd[u].head].parent;
  }
  if (nd[u].pos > nd[v].pos) {
    int tmp = u; u = v; v = tmp;
  }
  result += fen.range_sum(nd[u].pos, nd[v].pos);

  return result;
}

void process_queries() {
  fen.init(n);

  while (num_queries--) {
    int a, b, c, d;
    scanf("%d %d %d %d", &a, &b, &c, &d);
    path_add(a, b, +1);
    printf("%d\n", path_sum(c, d));
    path_add(a, b, -1);
  }
}

int main() {
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  process_queries();

  return 0;
}
