// Strongly connected components in O(V * E).
// Run a DFS from every node and build a reachability matrix.
#include <stdio.h>

#define MAX_NODES 1'000

bool adj[MAX_NODES][MAX_NODES]; // adjacency matrix
bool r[MAX_NODES][MAX_NODES];   // reachability matrix
bool marked[MAX_NODES];
int nodes;

// Runs a DFS from root. Current node is u. Fills r[root] with the nodes
// reachable from root.
void dfs(int root, int u) {
  r[root][u] = true;
  for (int v = 0; v < nodes; v++) {
    if (adj[u][v] && !r[root][v]) {
      dfs(root, v);
    }
  }
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  while (m--) {
    scanf("%d %d", &u, &v);
    adj[u][v] = true;
  }

  for (u = 0; u < nodes; u++) {
    dfs(u, u);
  }

  for (u = 0; u < nodes; u++) {
    if (!marked[u]) {
      // begin a new SCC
      for (int v = u; v < nodes; v++) {
        if (r[u][v] && r[v][u]) {
          printf("%d ", v);
          marked[v] = true;
        }
      }
      printf("\n");
    }
  }

  return 0;
}
