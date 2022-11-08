#include <stdio.h>

#define MAX_N 500'000
#define NIL 0

typedef struct {
  int v, next;
} edge;

edge e[2 * MAX_N];   // space for linked lists of edges
int adj[MAX_N + 1];  // entry points into lists of edges, initialized to NIL
bool vc[MAX_N + 1];  // vc[u] == true iff u is part of the MVC
int name[MAX_N + 1]; // name of each node, changed when folding
int n, next_name, vc_size;

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

void find_mvc(int u, int parent) {
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v != parent) {
      find_mvc(v, u);
      if (!vc[v] && !vc[u]) {
        vc[u] = true;
        vc_size++;
      }
    }
  }
}

void find_folds(int u, int parent, int dest) {
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    if (e[pos].v != parent) {
      find_folds(e[pos].v, u, vc[u] ? u : parent);
    }
  }

  if (vc[u] && (parent != NIL)) {
    // fold u onto its parent or grandparent
    printf("%d %d\n", name[u], name[dest]);
    name[dest] = next_name++;
  }
}

int main() {
  scanf("%d", &n);
  for (int u = 1; u <= n; u++) {
    name[u] = u;
  }
  next_name = n + 1;
  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  find_mvc(1, NIL);

  printf("%d\n", vc_size - 1);

  // Choose a VC root to ensure that every node has a VC parent or grandparent.
  // By definition, either node 1 or its first neighbor are part of the VC.
  int root = vc[1] ? 1 : e[adj[1]].v;
  find_folds(root, NIL, NIL);
}
