// Complexity: O(m log^2 n).
//
// Method: Heavy-light decomposition.
//
// Consider an extra pathway (u,v). We can only use it to replace an edge on
// the u-v path in the tree. Therefore, we proceed as follows:
//
// 1. Read the tree and decompose it.
//
// 2. Initialize the replacement costs of all edges to +∞.
//
// 3. For every extra pathway (u,v) of cost c, notify every edge on the path
//    u-v that a replacement of cost c is available. Use c to minimize the
//    replacement costs of those edges.
//
// 4. Print the minimum replacement cost encountered for every edge.
//
// To implement (3), we use a segment tree described below. Its mechanics are
// rather simple because all the calls to set() come before all the calls to
// get().
#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_SEGTREE_NODES = 1 << 17;
const int INFINITY = 2'000'000'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;  // the child with the largest subtree
  int head;   // the top of our path
  int pos;    // DFS time
};

struct edge {
  int u, v;
};

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

// A segment tree supporting two operations:
//
// 1. set(l, r, m): for all positions l ≤ i ≤ r, set v[i] = min(v[i], m).
// 2. get(i): return v[i].
//
// Furthermore, all set operations come before all get operations.
struct segment_tree {
  int v[MAX_SEGTREE_NODES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = 1; i < 2 * this->n; i++) {
      v[i] = INFINITY;
    }
  }

  void set(int l, int r, int val) {
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        v[l] = min(v[l], val);
        l++;
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r] = min(v[r], val);
        r--;
      }
      r >>= 1;
    }
  }

  void push_all() {
    for (int i = 1; i < n; i++) {
      v[2 * i] = min(v[2 * i], v[i]);
      v[2 * i + 1] = min(v[2 * i + 1], v[i]);
    }
  }

  int get(int pos) {
    return v[pos + n];
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
edge e[MAX_NODES];
segment_tree segtree;
FILE* fin;
int n, num_extra_paths;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_extra_paths);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    e[i] = { u, v };
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
      decompose_dfs(v, v); // start a new path
    }
  }
}

void set_cost_on_path(int u, int v, int cost) {
  // Lift u or v while they are on different paths.
  while (nd[u].head != nd[v].head) {
    if (nd[v].pos > nd[u].pos) {
      int tmp = u; u = v; v = tmp;
    }
    segtree.set(nd[nd[u].head].pos, nd[u].pos, cost);
    u = nd[nd[u].head].parent;
  }

  // One last operation when u and v are on a common path.
  if (nd[u].pos > nd[v].pos) {
    int tmp = u; u = v; v = tmp;
  }

  // Do not write anything on the LCA node itself.
  segtree.set(nd[u].pos + 1, nd[v].pos, cost);
}

void read_extra_paths() {
  while (num_extra_paths--) {
    int u, v, cost;
    fscanf(fin, "%d %d %d\n", &u, &v, &cost);
    set_cost_on_path(u, v, cost);
  }
}

void compute_answers() {
  FILE* fout = fopen("disrupt.out", "w");

  segtree.push_all();
  for (int i = 0; i < n - 1; i++) {
    int child = (nd[e[i].u].parent == e[i].v) ? e[i].u : e[i].v;
    int cost = segtree.get(nd[child].pos);
    cost = (cost == INFINITY) ? -1 : cost;
    fprintf(fout, "%d\n", cost);
  }

  fclose(fout);
}

int main() {
  fin = fopen("disrupt.in", "r");
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  segtree.init(n);
  read_extra_paths();
  compute_answers();
  fclose(fin);

  return 0;
}
