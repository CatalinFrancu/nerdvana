#include <stdio.h>

#define MAX_N 500'000
#define NIL -1

struct edge {
  int v, next;
} e[2 * MAX_N];
int adj[MAX_N]; // adjacency lists
int v[MAX_N];   // node values
int p[MAX_N];   // parents

void add_edge(int u, int v, int pos) {
  e[pos] = { v, adj[u] };
  adj[u] = pos;
}

// Traverse the tree and compute parents.
void dfs(int u) {
  for (int pos = adj[u]; pos != NIL; pos = e[pos].next) {
    int v = e[pos].v;
    if (v && p[v] == NIL) {
      p[v] = u;
      dfs(v);
    }
  }
}

int main() {
  int n;

  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    adj[i] = p[i] = NIL;
  }
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v, 2 * i);
    add_edge(v, u, 2 * i + 1);
  }

  p[0] = NIL;
  dfs(0);

  for (int i = 0; i < n; i++) {
    scanf("%d", &v[i]);
  }

  int num_queries;
  scanf("%d", &num_queries);
  while (num_queries--) {
    int query_type, u, val;
    scanf("%d %d", &query_type, &u);
    if (query_type == 1) {
      // Update node value.
      scanf("%d", &val);
      v[u] += val;
    } else {
      // Print sum of nodes from u to root.
      int sum = 0;
      do {
        sum += v[u];
        u = p[u];
      } while (u != NIL);
      printf("%d\n", sum);
    }
  }
}
