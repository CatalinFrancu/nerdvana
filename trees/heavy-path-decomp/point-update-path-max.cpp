// Uses heavy path decomposition to handle:
//    1. Query(u, v): returns the maximum value on the u-v path.
//    2. Update(u, val): updates node u's value to val.
// Solves https://infoarena.ro/problema/heavypath
//
// Creates one segment tree per heavy path. This is bad and slow. Kept around
// for historical reasons.
#include <stdio.h>

const int MAX_NODES = 100'000;
const int NIL = -1;

struct edge {
  int v, next;
} e[2 * MAX_NODES];

struct node {
  int adj;     // adjacency list
  int val;     // value
  int d;       // depth
  int p;       // parent
  int heavy;   // heavy child
  int head;    // the node at the top of my chain

  // For chain heads only.
  int start;   // pointer to "offset 0" in the segment tree space
  int size;    // size of my chain, rounded up to a power of 2
} nd[MAX_NODES];

// Shared space for segment trees.
int st[3 * MAX_NODES]; // cautiously optimistic; worst case is 4 * MAX_NODES
int st_ptr;        // first unused position

int max(int x, int y) {
  return (x > y) ? x : y;
}

// Returns u's position in st[], relative to "offset 0".
int st_node_pos(int u) {
  int head = nd[u].head;
  return nd[head].size + nd[u].d - nd[head].d;
}

// Returns the maximum value on the closed interval [u, v].
int st_query(int u, int v) {
  // Reduce the data to a regular (size, left, right) segment tree query.
  int* s = st + nd[nd[u].head].start;
  int l = st_node_pos(u);
  int r = st_node_pos(v);
  int result = 0;

  while (l <= r)  {
    if (l & 1) {
      result = max(result, s[l++]);
    }
    l >>= 1;

    if (!(r & 1)) {
      result = max(result, s[r--]);
    }
    r >>= 1;
  }

  return result;
}

void st_update(int u, int val) {
  // Reduce the data to a regular (size, pos, val) segment tree update.
  int* s = st + nd[nd[u].head].start;
  int pos = st_node_pos(u);

  s[pos] = val;
  while (pos > 1) {
    pos >>= 1;
    s[pos] = max(s[2 * pos], s[2 * pos + 1]);
  }
}

// Makes space for a segment tree for the chain tail->head.
void st_alloc(int head, int tail) {
  // Round the size up to a power of 2.
  int real_size = nd[tail].d - nd[head].d + 1;
  int size = 1 << (32 - __builtin_clz(real_size - 1));

  nd[head].start = st_ptr - 1; // offset "0"
  nd[head].size = size;
  st_ptr += 2 * size - 1;
  int* s = st + nd[head].start;

  // Go up the chain and copy values into the segment tree.
  for (int i = size + real_size - 1; i >= size; i--) {
    s[i] = nd[tail].val;
    tail = nd[tail].p;
  }

  // Compute the higher-level values.
  for (int i = size - 1; i; i--) {
    s[i] = max(s[2 * i], s[2 * i + 1]);
  }
}

void add_edge(int u, int v) {
  static int pos = 1;
  e[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

// Returns the size of u's subtree.
int dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int pos = nd[u].adj; pos; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != nd[u].p) {
      nd[v].p = u;
      nd[v].d = 1 + nd[u].d;
      int child_size = dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }
    }
  }

  return my_size;
}

void decompose(int u, int head) {
  nd[u].head = head;

  for (int pos = nd[u].adj; pos; pos = e[pos].next) {
    int v = e[pos].v;
    if (v == nd[u].heavy) {
      decompose(v, head);
    } else if (v != nd[u].p) {
      decompose(v, v); // start a new chain
    }
  }

  // Every leaf is at the bottom of a chain. Leaves have a single neighbor.
  if ((u > 0) && !e[nd[u].adj].next) {
    st_alloc(head, u);
  }
}

int query(int u, int v) {
  int result = 0;
  while (nd[u].head != nd[v].head) {
    // Run a prefix query on the lower chain.
    if (nd[nd[v].head].d > nd[nd[u].head].d) {
      int tmp = u; u = v; v = tmp;
    }
    result = max(result, st_query(nd[u].head, u));
    // Jumping to the head's *parent* puts us on a new chain.
    u = nd[nd[u].head].p;
  }

  // The last query happens on the common chain. Pass the nodes in the proper
  // order.
  if (nd[u].d > nd[v].d) {
    int tmp = u; u = v; v = tmp;
  }
  result = max(result, st_query(u, v));

  return result;
}

int main() {
  int n, num_queries;

  FILE* fin = fopen("heavypath.in", "r");
  FILE* fout = fopen("heavypath.out", "w");

  // Read the graph
  fscanf(fin, "%d %d", &n, &num_queries);

  for (int u = 0; u < n; u++) {
    fscanf(fin, "%d", &nd[u].val);
    nd[u].heavy = NIL;
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    u--; v--;
    add_edge(u, v);
    add_edge(v, u);
  }

  // Run the heavy path decomposition.
  nd[0].p = NIL;
  dfs(0);
  decompose(0, 0);

  // Answer queries.
  while (num_queries--) {
    int t, x, y;
    fscanf(fin, "%d %d %d", &t, &x, &y);
    if (t) {
      fprintf(fout, "%d\n", query(x - 1, y - 1));
    } else {
      st_update(x - 1, y);
    }
  }

  fclose(fin);
  fclose(fout);

  return 0;
}
