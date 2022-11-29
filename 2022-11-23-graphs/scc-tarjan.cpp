// Strongly connected components - Tarjan's algorithm.
#include <stdio.h>

#define MAX_NODES 300'000
#define MAX_EDGES 1'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

typedef struct {
  int adj; // head of adjacency list
  int d;   // discovery time
  int low; // lowest time reachable from subtree
  bool onStack;
} node;

node n[MAX_NODES];
int st[MAX_NODES], ss;  // DFS stack
edge e[MAX_EDGES];
int nodes, edges, time;

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
      // Traverse the white descendant and make a note of how high it can
      // climb.
      dfs(v);
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
      n[v].onStack = false;
      printf("%d ", v);
    } while (v != u);
    printf("\n");
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

  return 0;
}
