// Breadth-first search on an undirected graph.
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int v, next;
} edge;

// consider encapsulating these in a node structure
int adj[MAX_NODES]; // start of adjacency list
int d[MAX_NODES];   // distance from source
int p[MAX_NODES];   // parent (closest node on path to source)

edge e[2 * MAX_EDGES];
int q[MAX_NODES]; // queue
int nodes, edges;

void add_edge(int u, int v) {
  e[edges] = { v, adj[u] };
  adj[u] = edges++;
}

void bfs(int source) {
  // All the vertices are white: we know nothing about them.
  // Starting point: make the source gray.
  int head = 0, tail = 1;
  d[source] = 0;   // initialize queue to just the source
  p[source] = NIL; // the distance from source to source is 0
  q[0] = source;   // the path from source to source is empty

  while (head != tail) {
    // remove the front of the queue and explore it
    int u = q[head++];
    printf("d[%d] = %d, parent = %d\n", u, d[u], p[u]);
    for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      // we are only interested in the white successors, which we make gray
      if (d[v] == INFINITY) {
        d[v] = 1 + d[u];
        p[v] = u;        // the shortest path from source to v goes through u
        q[tail++] = v;
      }
    }
    // vertex u is fully explored and becomes black
  }
}

int main() {
  int m, source, u, v;
  scanf("%d %d %d", &nodes, &m, &source);
  for (u = 0; u < nodes; u++) {
    adj[u] = NIL;
    d[u] = INFINITY;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  bfs(source);

  return 0;
}
