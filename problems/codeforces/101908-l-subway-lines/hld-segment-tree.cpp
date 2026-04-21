#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_SEGTREE_NODES = 1 << 18;

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

// A segment tree that supports range add and range sum operations.
// Contract: the node's value does not include the lazy value. That needs to
// be applied before push() and range_sum() operations/
struct segment_tree_node {
  int s;
  int lazy; // quantity to add to every element spanned
};

struct segment_tree {
  segment_tree_node v[MAX_SEGTREE_NODES];
  int n;

  void init(int _n) {
    n = next_power_of_2(_n);
  }

  void push(int node, int size) {
    v[node].s += v[node].lazy * size;
    v[2 * node].lazy += v[node].lazy;
    v[2 * node + 1].lazy += v[node].lazy;
    v[node].lazy = 0;
  }

  void push_path(int node, int size) {
    if (node) {
      push_path(node / 2, size * 2);
      push(node, size);
    }
  }

  void pull(int node, int size) {
    v[node].s =
      v[2 * node].s + v[2 * node].lazy * size / 2 +
      v[2 * node + 1].s + v[2 * node + 1].lazy * size / 2;
  }

  void pull_path(int node) {
    for (int x = node / 2, size = 2; x; x /= 2, size <<= 1) {
      pull(x, size);
    }
  }

  int range_sum(int l, int r) {
    int sum = 0;
    int size = 1;

    l += n;
    r += n;
    push_path(l / 2, 2);
    push_path(r / 2, 2);

    while (l <= r)  {
      if (l & 1) {
        sum += v[l].s + v[l].lazy * size;
        l++;
      }
      l >>= 1;

      if (!(r & 1)) {
        sum += v[r].s + v[r].lazy * size;
        r--;
      }
      r >>= 1;
      size <<= 1;
    }

    return sum;
  }

  void range_add(int l, int r, int delta) {
    l += n;
    r += n;
    int lc = l, rc = r;
    push_path(l / 2, 2);
    push_path(r / 2, 2);

    while (l <= r)  {
      if (l & 1) {
        v[l++].lazy += delta;
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r--].lazy += delta;
      }
      r >>= 1;
    }

    pull_path(lc);
    pull_path(rc);
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
segment_tree segtree;
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
  static int time = 0;

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
    segtree.range_add(nd[nd[u].head].pos, nd[u].pos, delta);
    u = nd[nd[u].head].parent;
  }
  if (nd[u].pos > nd[v].pos) {
    int tmp = u; u = v; v = tmp;
  }
  segtree.range_add(nd[u].pos, nd[v].pos, delta);
}

int path_sum(int u, int v) {
  int result = 0;

  while (nd[u].head != nd[v].head) {
    if (nd[v].pos > nd[u].pos) {
      int tmp = u; u = v; v = tmp;
    }
    result += segtree.range_sum(nd[nd[u].head].pos, nd[u].pos);
    u = nd[nd[u].head].parent;
  }
  if (nd[u].pos > nd[v].pos) {
    int tmp = u; u = v; v = tmp;
  }
  result += segtree.range_sum(nd[u].pos, nd[v].pos);

  return result;
}

void process_queries() {
  segtree.init(n);

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
