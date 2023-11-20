// Same as v2, but does away with node depths. Uses node DFS pos for
// comparisons. Simplifies the decision of which path to climb, as follows.
//
// The original code compares the depths of n[u].head and n[v].head. But
// comparing the DFS times is enough. What we want is to ensure that, once u
// is on the common path, it will wait for v there. Note that, if u is on the
// common path and v is not, then necessarily n[v].pos > n[u].pos, because u
// is on a heavy path and v is not. And heavy children are always called
// before light ones.
#include <stdio.h>

const int MAX_NODES = 1 << 17;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int val;
  int parent;
  int heavy;  // the child with the largest subtree
  int head;   // the top of our path
  int pos;    // DFS time
};

int next_power_of_2(int n) {
  while (n & (n - 1)) {
    n += n & -n;
  }
  return n;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

struct segment_tree {
  int v[2 * MAX_NODES];
  int n;

  void init(node* n, int num_nodes) {
    this->n = next_power_of_2(num_nodes);
    for (int u = 1; u <= num_nodes; u++) {
      v[n[u].pos + this->n] = n[u].val;
    }
    for (int pos = this->n - 1; pos; pos--) {
      v[pos] = max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  void update(int pos, int val) {
    pos += n;
    v[pos] = val;
    for (pos /= 2; pos; pos /= 2) {
      v[pos] = max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  int rmq(int l, int r) {
    int result = -1;

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = max(result, v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = max(result, v[r--]);
      }
      r >>= 1;
    }

    return result;
  }
};

cell list[2 * MAX_NODES];
node n[MAX_NODES + 1];
segment_tree segtree;
int num_nodes, num_queries;
FILE *fin, *fout;

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, n[u].adj };
  n[u].adj = pos++;
}

void read_input_data() {
  fin = fopen("heavypath.in", "r");
  fout = fopen("heavypath.out", "w");

  fscanf(fin, "%d %d", &num_nodes, &num_queries);
  for (int u = 1; u <= num_nodes; u++) {
    fscanf(fin, "%d", &n[u].val);
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != n[u].parent) {

      n[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        n[u].heavy = v;
      }

    }
  }

  return my_size;
}

void decompose_dfs(int u, int head) {
  static int time = 0;

  n[u].head = head;
  n[u].pos = time++;

  if (n[u].heavy) {
    decompose_dfs(n[u].heavy, head);
  }

  for (int ptr = n[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != n[u].parent && v != n[u].heavy) {
      decompose_dfs(v, v); // start a new path
    }
  }
}

int query(int u, int v) {
  int result = 0;
  while (n[u].head != n[v].head) {
    if (n[v].pos > n[u].pos) {
      int tmp = u; u = v; v = tmp;
    }
    result = max(result, segtree.rmq(n[n[u].head].pos, n[u].pos));
    // Jumping to the head's *parent* puts us on a new path.
    u = n[n[u].head].parent;
  }

  // The last query happens on the common path.
  if (n[u].pos > n[v].pos) {
    int tmp = u; u = v; v = tmp;
  }
  result = max(result, segtree.rmq(n[u].pos, n[v].pos));

  return result;
}

void process_queries() {
  while (num_queries--) {
    int t, x, y;
    fscanf(fin, "%d %d %d", &t, &x, &y);
    if (t) {
      fprintf(fout, "%d\n", query(x, y));
    } else {
      segtree.update(n[x].pos, y);
    }
  }

  fclose(fin);
  fclose(fout);
}

int main() {
  read_input_data();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  segtree.init(n, num_nodes);
  process_queries();

  return 0;
}
