// Strongly connected components - Tarjan's algorithm.
//
// The code is more complicated than it needs to be. We only store and sort
// the components to ensure output consistency with other algorithms.
#include <algorithm>
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

typedef struct {
  int adj; // head of adjacency list
  int d;   // discovery time
  int low; // lowest time reachable from subtree
  int c;   // component number
  bool onStack;
} node;

node n[MAX_NODES];
int st[MAX_NODES], ss;  // DFS stack
int list[MAX_NODES];    // list[x] = sorted list of nodes in component x
edge e[2 * MAX_EDGES];
int nodes, edges, time, num_comps;

int min(int x, int y) {
  return (x < y) ? x : y;
}

void add_edge(int u, int v) {
  e[edges] = { v, n[u].adj };
  n[u].adj = edges++;
}

void dfs(int u) {
  n[u].d = n[u].low = ++time;
  n[u].onStack = true;
  st[ss++] = u;

  for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (!n[v].d) {
      dfs(v);
      // Traverse the white descendant and make a note of how high it can
      // climb.
      n[u].low = min(n[u].low, n[v].low);
    } else if (n[v].onStack) {
      // We can climb to v's level.
      n[u].low = min(n[u].low, n[v].d);
    }
  }

  // If no vertex from u's subtree can climb higher than u, then the stack
  // from u to the top is a SCC.
  if (n[u].low == n[u].d) {
    int v;
    do {
      v = st[--ss];
      n[v].onStack = 0;
      n[v].c = num_comps;
    } while (v != u);
    num_comps++;
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
  }

  for (u = 0; u < nodes; u++) {
    if (!n[u].d) {
      dfs(u);
    }
  }

  // The rest is fluff needed to achieve a consistent output order.

  // Repurpose edges[]. Collect each component's nodes in sorted order.
  for (int x = 0; x < num_comps; x++) {
    list[x] = NIL;
  }
  for (u = nodes - 1; u >= 0; u--) {
    e[u] = { u, list[n[u].c] };
    list[n[u].c] = u;
  }

  // Traverse and print components in *node* order.
  for (u = 0; u < nodes; u++) {
    if (list[n[u].c] != NIL)  {
      for (int pos = list[n[u].c]; pos != NIL; pos = e[pos].next) {
        printf("%d ", e[pos].v);
      }
      printf("\n");
      list[n[u].c] = NIL; // wipe it so we don't print it twice
    }
  }

  return 0;
}
