// Biconnected components, bridges and articulation points.
// Tarjan's algorithm.
#include <stdio.h>

#define MAX_NODES 300'000
#define MAX_EDGES 1'000'000
#define NIL -1

typedef struct {
  int v, next;
} cell;

typedef struct {
  int u, v;
} edge;

typedef struct {
  int adj; // head of adjacency list
  int d;   // discovery time
  int low; // lowest time reachable from subtree
} node;

node n[MAX_NODES];
edge st[MAX_NODES];  // DFS stack
cell e[2 * MAX_EDGES];
int nodes, edges, time;
int ss; // stack size

int min(int x, int y) {
  return (x < y) ? x : y;
}

void add_edge(int u, int v) {
  e[edges] = { v, n[u].adj };
  n[u].adj = edges++;
}

// The stack from (u, v) to its top forms a BCC.
void print_bcc(int u, int v) {
  printf("BCC:");
  do {
    --ss;
    printf(" %d-%d", st[ss].u, st[ss].v);
  } while (st[ss].u != u || st[ss].v != v);
  printf("\n");
}

void dfs(int u, int parent) {
  bool is_ap = false;
  int num_children = 0;
  n[u].d = n[u].low = ++time;

  for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (!n[v].d) {

      // Traverse the white descendant and make a note of how high it can
      // climb.
      num_children++;
      st[ss++] = { u, v };
      dfs(v, u);
      n[u].low = min(n[u].low, n[v].low);

      // If v cannot reach u (or above), then u-v is a bridge. Depending on
      // the definition, bridges may or may not be BCC's by themselves.
      if (n[v].low > n[u].d) {
        printf("B: %d-%d\n", u, v);
      }

      // If v cannot climb above u, then (1) a BCC starts at v and (2) either
      // u is the root or it is an AP.
      if (n[v].low >= n[u].d) {
        is_ap = true;
        print_bcc(u, v);
      }

    } else if (v != parent) {

      // back edge -- make sure not to traverse it going forward
      if (n[v].d < n[u].d) {
        n[u].low = min(n[u].low, n[v].d);
        st[ss++] = { u, v };
      }

    }
  }

  // The root is an AP if it has multiple children.
  // A non-root vertex is an AP if it was marked while visiting its children.
  if (((parent == NIL) && (num_children > 1)) ||
      ((parent != NIL) && is_ap)) {
    printf("AP: %d\n", u);
  }
}

int main() {
  // read the graph and create adjacency lists
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    n[u].adj = NIL;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (u = 0; u < nodes; u++) {
    if (!n[u].d) {
      dfs(u, NIL);
    }
  }

  return 0;
}
