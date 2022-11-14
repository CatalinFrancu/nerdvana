// Breadth-first search on an undirected graph.
#include <stdio.h>

#define MAX_NODES 100'000
#define MAX_EDGES 500'000
#define INFINITY 1'000'000'000
#define NIL -1

typedef struct {
  int adj; // start of adjacency list
  int d;   // distance from source
  int p;   // parent (closest node on path to source)
} node;

typedef struct {
  int v, next;
} edge;

node n[MAX_NODES];
edge e[2 * MAX_EDGES];
int q[MAX_NODES]; // queue
int nodes, edges;

void add_edge(int u, int v) {
  e[edges] = { v, n[u].adj };
  n[u].adj = edges++;
}

void bfs(int source) {
  int head = 0, tail = 1;
  n[source].d = 0;
  n[source].p = NIL;
  q[0] = source;

  while (head != tail) {
    int u = q[head++];
    printf("d[%d] = %d, parent = %d\n", u, n[u].d, n[u].p);
    for (int pos = n[u].adj; pos != NIL; pos = e[pos].next) {
      int v = e[pos].v;
      if (n[v].d == INFINITY) {
        n[v].d = 1 + n[u].d;
        n[v].p = u;
        q[tail++] = v;
      }
    }
  }
}

int main() {
  int m, source, u, v;
  scanf("%d %d %d", &nodes, &m, &source);
  for (int i = 0; i < nodes; i++) {
    n[i].adj = NIL;
    n[i].d = INFINITY;
  }
  while (m--) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  bfs(source);

  return 0;
}
