// Strongly connected components - Kosaraju's algorithm.
//
// The second step is more complicated than it needs to be. We only store and
// sort the components in order to conform with the output format of the naive
// algorithm.
#include <algorithm>
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

int adj[MAX_NODES], adjt[MAX_NODES]; // adjacency lists in G and G_T
int st[MAX_NODES], ss;    // stack of nodes in order of finish time
int comp[MAX_NODES], nc;  // each node's component
edge e[2 * MAX_EDGES];
int nodes, edges;

void add_edge(int u, int v) {
  e[edges] = { v, adj[u] };
  adj[u] = edges++;
}

// DFS in G. comp[u] is a visited marker for u. Pushes nodes on the st in
// order of their finish time.
void dfs(int u) {
  comp[u] = true;
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (!comp[v]) {
      dfs(v);
    }
  }
  st[ss++] = u;
}

// Transposes the graph. Uses a separate head array, but reuses the edge array.
void transpose() {
  for (int u = 0; u < nodes; u++) {
    adjt[u] = NIL;
  }
  for (int u = 0; u < nodes; u++) {
    int pos = adj[u];
    while (pos != NIL) {
      // move e[pos] from u to v's adjacency list
      int v = e[pos].v;
      int next = e[pos].next;
      e[pos] = { u, adjt[v] };
      adjt[v] = pos;
      pos = next;
    }
  }
}

// DFS in G_T. comp[u] stores u's connected component.
void dfs2(int u, int comp_num) {
  comp[u] = comp_num;
  for (int pos = adjt[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (comp[v] == NIL) {
      dfs2(v, comp_num);
    }
  }
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    adj[u] = NIL;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
  }

  for (u = 0; u < nodes; u++) {
    if (!comp[u]) {
      dfs(u);
    }
  }

  transpose();  

  for (u = 0; u < nodes; u++) {
    comp[u] = NIL;
  }

  while (ss) {
    u = st[--ss];
    if (comp[u] == NIL) {
      dfs2(u, nc++);
    }
  }

  // We no longer need adj[] and edges[], so we repurpose them. adj[x] stores
  // a sorted list of nodes u having comp[u] = x.
  for (int x = 0; x < nc; x++) {
    adj[x] = NIL;
  }
  for (u = nodes - 1; u >= 0; u--) {
    e[u] = { u, adj[comp[u]] };
    adj[comp[u]] = u;
  }

  for (u = 0; u < nodes; u++) {
    if (comp[u] != NIL)  {
      // traverse and print this component
      for (int pos = adj[comp[u]]; pos != NIL; pos = e[pos].next) {
        printf("%d ", e[pos].v);
        comp[e[pos].v] = NIL;
      }
      printf("\n");
    }
  }

  return 0;
}
