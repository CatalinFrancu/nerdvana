// Strongly connected components - Kosaraju's algorithm.
#include <stdio.h>

#define MAX_NODES 300'000
#define MAX_EDGES 1'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

int adj[MAX_NODES], adjt[MAX_NODES]; // adjacency lists in G and G_T
int st[MAX_NODES], ss;  // stack of nodes in order of finish time
bool seen[MAX_NODES];   // visited markers
edge e[2 * MAX_EDGES];
int nodes, edges;

// Common code for both DFS's in G.
void dfs(bool first, int u) {
  seen[u] = true;

  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    if (!seen[e[pos].v]) {
      dfs(first, e[pos].v);
    }
  }

  if (first) {
    st[ss++] = u; // first pass pushes nodes on the stack
  } else {
    printf("%d ", u); // second pass prints nodes
  }
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
    if (!seen[u]) {
      dfs(true, u);
    }
  }

  transpose();

  // DFS #2 on transpose graph in stack order
  for (u = 0; u < nodes; u++) {
    seen[u] = false;
    adj[u] = adjt[u];
  }

  while (ss) {
    u = st[--ss];
    if (!seen[u]) {
      dfs(false, u);
      printf("\n");
    }
  }

  return 0;
}
