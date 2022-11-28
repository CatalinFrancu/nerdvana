// Strongly connected components - Kosaraju's algorithm.
//
// The second step is more complicated than it needs to be. We only store and
// sort the components to ensure output consistency with other algorithms.
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

int adj[MAX_NODES], adjt[MAX_NODES]; // adjacency lists in G and G_T
int st[MAX_NODES], ss;    // stack of nodes in order of finish time
int d[MAX_NODES];         // multipurpose node info
edge e[2 * MAX_EDGES];
int nodes, edges;

// DFS in G. d[u] is a visited marker for u. Pushes nodes on the stack in
// order of their finish times.
void dfs(int u) {
  d[u] = true;
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (!d[v]) {
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

// DFS in G_T. d[u] stores u's connected component.
void dfs2(int u, int comp_num) {
  d[u] = comp_num;
  for (int pos = adjt[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (d[v] == NIL) {
      dfs2(v, comp_num);
    }
  }
}

int main() {
  // read the graph and create adjacency lists
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  for (u = 0; u < nodes; u++) {
    adj[u] = NIL;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    e[edges] = { v, adj[u] };
    adj[u] = edges++;
  }

  // DFS #1: create a stack of nodes in reverse finish order
  for (u = 0; u < nodes; u++) {
    if (!d[u]) {
      dfs(u);
    }
  }

  transpose();

  // DFS #2 on transpose graph in stack order
  for (u = 0; u < nodes; u++) {
    d[u] = NIL;
  }

  int nc = 0;
  while (ss) {
    u = st[--ss];
    if (d[u] == NIL) {
      dfs2(u, nc++);
    }
  }

  // The rest is fluff needed to achieve a consistent output order.

  // Repurpose adj[] and edges[]. adj[x] stores a sorted list of nodes u
  // having d[u] = x.
  for (int x = 0; x < nc; x++) {
    adj[x] = NIL;
  }
  for (u = nodes - 1; u >= 0; u--) {
    e[u] = { u, adj[d[u]] };
    adj[d[u]] = u;
  }

  // Traverse and print components in *node* order.
  for (u = 0; u < nodes; u++) {
    if (adj[d[u]] != NIL)  {
      for (int pos = adj[d[u]]; pos != NIL; pos = e[pos].next) {
        printf("%d ", e[pos].v);
      }
      printf("\n");
      adj[d[u]] = NIL; // wipe it so we don't print it twice
    }
  }

  return 0;
}
