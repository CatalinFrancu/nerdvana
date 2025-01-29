// Same as v2, but uses STL vectors for adjacency lists.
#include <stdio.h>
#include <vector>

const int MAX_NODES = 1 << 17;

struct node {
  std::vector<int> adj;
  int val;
  int depth;
  int parent;
  int heavy;  // the child with the largest subtree
  int head;   // the top of our path
  int tin;    // DFS time
};

node nd[MAX_NODES + 1];

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct segment_tree {
  int v[2 * MAX_NODES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int u = 1; u <= n; u++) {
      v[nd[u].tin + this->n] = nd[u].val;
    }
    for (int pos = this->n - 1; pos; pos--) {
      v[pos] = std::max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  void update(int pos, int val) {
    pos += n;
    v[pos] = val;
    for (pos /= 2; pos; pos /= 2) {
      v[pos] = std::max(v[2 * pos], v[2 * pos + 1]);
    }
  }

  int rmq(int l, int r) {
    int result = -1;

    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = std::max(result, v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = std::max(result, v[r--]);
      }
      r >>= 1;
    }

    return result;
  }
};

segment_tree segtree;
int n, num_queries;
FILE *fin, *fout;

void read_data() {
  fscanf(fin, "%d %d", &n, &num_queries);
  for (int u = 1; u <= n; u++) {
    fscanf(fin, "%d", &nd[u].val);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

// Returns the size of u's subtree.
int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {

      nd[v].parent = u;
      nd[v].depth = 1 + nd[u].depth;
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
  nd[u].tin = time++;

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int v: nd[u].adj) {
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // start a new path
    }
  }
}

int query(int u, int v) {
  int result = 0;
  while (nd[u].head != nd[v].head) {
    // Run a prefix query on the lower chain.
    if (nd[nd[v].head].depth > nd[nd[u].head].depth) {
      std::swap(u, v);
    }
    result = std::max(result, segtree.rmq(nd[nd[u].head].tin, nd[u].tin));
    // Jumping to the head's *parent* puts us on a new path.
    u = nd[nd[u].head].parent;
  }

  // The last query happens on the common path.
  if (nd[u].depth > nd[v].depth) {
    std::swap(u, v);
  }
  result = std::max(result, segtree.rmq(nd[u].tin, nd[v].tin));

  return result;
}

void process_queries() {
  while (num_queries--) {
    int t, x, y;
    fscanf(fin, "%d %d %d", &t, &x, &y);
    if (t) {
      fprintf(fout, "%d\n", query(x, y));
    } else {
      segtree.update(nd[x].tin, y);
    }
  }
}

int main() {
  fin = fopen("heavypath.in", "r");
  fout = fopen("heavypath.out", "w");

  read_data();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  segtree.init(n);
  process_queries();

  fclose(fin);
  fclose(fout);

  return 0;
}
