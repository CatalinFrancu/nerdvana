#include <algorithm>
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 1'000'000
#define NIL -1
#define INFINITY (MAX_NODES + 1)

typedef struct {
  int adj;   // head of adjacency list
  int d;     // clock during BCC computation; then reused as BFS distance to sink
  int low;   // lowest reachable d via any edge
  int c, nc; // distance to sink assuming u's edge to its parent is / is not cut
} node;

typedef struct {
  int v, comp, next;
} edge;

node r[MAX_NODES];
edge e[2 * MAX_EDGES];  // statically allocated edge array
int a[MAX_EDGES];   // various purposes; see comments
int nodes, edges, source, sink, num_bcc;
int ss, clock;      // stack size, DFS clock

int max(int x, int y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void add_edge(int u, int v) {
  e[edges] = { v, NIL, r[u].adj };
  r[u].adj = edges++;
}

// Compute shortest paths to sink using DFS. a[] is the edge stack.
// from = edge index from parent
void compute_bcc(int u, int from) {
  r[u].d = r[u].low = clock++;
  printf("Marking %d as seen at time %d\n", u, r[u].d);
  for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;

    if (r[v].d == NIL) {
      // new node
      a[ss++] = pos;
      compute_bcc(v, pos);
      r[u].low = min(r[u].low, r[v].low);
      if (r[v].low >= r[u].d) {
        int p;
        do {
          p = a[--ss];
          e[p].comp = e[p ^ 1].comp = num_bcc;
        } while (p != pos);
        num_bcc++;
      }
    } else if ((from != NIL) && ((from ^ pos) == 1)) {
      // Do nothing: this is the counterpart of the edge that brought us here.
      // Note: because G is a multigraph, we cannot simply pass the parent as
      // an argument. We need the specific edge.
    } else {
      // edge to an already visited node
      r[u].low = min(r[u].low, r[v].d);
      if (r[v].d < r[u].d) {
        a[ss++] = pos;
      }
    }
  }
}

// Compute shortest paths to sink using BFS. a[] is the BFS queue.
void compute_d() {
  for (int i = 0; i < nodes; i++) {
    r[i].d = INFINITY;
  }

  int head = 0, tail = 1;
  r[sink].d = 0;
  a[0] = sink;

  while (head != tail) {
    int u = a[head++];
    printf("d[%d] = %d\n", u, r[u].d);
    for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (r[v].d == INFINITY) {
        r[v].d = 1 + r[u].d;
        a[tail++] = v;
      }
    }
  }
}

// Compute distance to sink from every node u assuming one edge from u is cut.
void compute_c() {
  r[sink].c = 0;
  for (int i = nodes - 1; i; i--) {
    int u = a[i];

    // determine how many neighbors of each kind there are and the component
    // of the cut edge
    int num_closer = 0, num_equal = 0, comp;
    for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (r[v].d < r[u].d) {
        num_closer++;
        comp = e[pos].comp;
      } else if (r[v].d == r[u].d) {
        num_equal++;
      }
    }

    if (num_closer >= 2) {
      r[u].c = r[u].d; // have several optimal routes
    } else if (num_equal) {
      r[u].c = r[u].d + 1; // can go through the equal neighbor
    } else {
      r[u].c = INFINITY;
      // c[u] = 1 + min(c[v]) for any and all edges in the same component
      for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
        int v = e[pos].v;
        if (r[v].d > r[u].d && e[pos].comp == comp) {
          r[u].c = min(r[u].c, 1 + r[v].c);
        }
      }
    }
    printf("c[%d] = %d\n", u, r[u].c);
  }
}

void compute_nc() {
  r[sink].nc = 0;
  for (int i = 1; i < nodes; i++) {
    int u = a[i];

    for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      r[u].nc = min(r[u].nc, 1 + max(r[v].c, r[v].nc));
    }
    printf("nc[%d] = %d\n", u, r[u].nc);
  }
}

int main() {
  int m, u, v;
  scanf("%d %d %d %d", &nodes, &m, &source, &sink);
  for (int i = 0; i < nodes; i++) {
    r[i].adj = NIL;
    r[i].d = NIL;
    r[i].nc = INFINITY;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    if (u != v) { // self loops are useless
      add_edge(u, v);
      add_edge(v, u);
    }
  }

  compute_bcc(sink, NIL);
  for (int u = 0; u < nodes; u++) {
    for (int pos = r[u].adj; pos != NIL; pos = e[pos].next) {
      printf("Edge %d-%d comp %d\n", u, e[pos].v, e[pos].comp);
    }
  }

  compute_d();

  // from this point onwards, a[] gives the nodes in increasing d order
  for (int i = 0; i < nodes; i++) {
    a[i] = i;
  }
  std::sort(a, a + nodes, [](int u, int v) {
    return r[u].d < r[v].d;
  });
  compute_c();
  compute_nc();

  return 0;
}

// TODO
// * do we need both c and nc? Can we compute nc on top of c?
