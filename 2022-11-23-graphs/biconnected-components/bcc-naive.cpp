// Biconnected components, bridges and articulation points.
// Dumbest, safest approach; see comments in main().
#include <stdio.h>

#define MAX_NODES 1'000
#define NONE -1

bool adj[MAX_NODES][MAX_NODES]; // adjacency matrix
bool seen[MAX_NODES];           // visited markers
int dsf[MAX_NODES * MAX_NODES]; // disjoint set forest for edges
int p[MAX_NODES];   // parents on the DFS stack during BCC
int nodes;

int get_edge_index(int u, int v) {
  return (u < v)
    ? u * nodes + v
    : v * nodes + u;
}

int set_find(int x) {
  int r = x;
  while (dsf[r] != r) {
    r = dsf[r];
  }

  // path compression
  while (dsf[x] != r) {
    int p = dsf[x];
    dsf[x] = r;
    x = p;
  }

  return r;
}

void set_union(int x, int y) {
  x = set_find(x);
  y = set_find(y);

  if (x != y) {
    dsf[y] = x;
  }
}

void dfs(int u, int bad_node) {
  seen[u] = true;
  for (int v = 0; v < nodes; v++) {
    if (adj[u][v] && !seen[v] && (v != bad_node)) {
      dfs(v, bad_node);
    }
  }
}

// Returns the number of connected components in the current graph, assuming
// bad_node is removed.
int count_cc(int bad_node) {
  int result = 0;

  for (int u = 0; u < nodes; u++) {
    if (!seen[u] && (u != bad_node)) {
      result++;
      dfs(u, bad_node);
    }
  }
  for (int u = 0; u < nodes; u++) {
    seen[u] = false;
  }

  return result;
}

void dfs_bcc(int u, int parent) {
  seen[u] = true;
  p[u] = parent;

  for (int v = 0; v < nodes; v++) {
    if (adj[u][v]) {
      if (!seen[v]) {
        // white node
        dfs_bcc(v, u);
      } else if (v != parent && p[v] != NONE) {
        // gray node closing a cycle
        int a = v, b = u, c = p[u];
        do {
          set_union(get_edge_index(a, b), get_edge_index(b, c));
          a = b; b = c; c = p[c];
        } while (b != v);
      }
    }
  }

  p[u] = NONE;
}

int main() {
  int m, u, v;
  scanf("%d %d", &nodes, &m);
  while (m--) {
    scanf("%d %d", &u, &v);
    adj[u][v] = adj[v][u] = true;
    int idx = get_edge_index(u, v);
    dsf[idx] = idx;
  }

  int num_cc = count_cc(NONE);

  // To find articulation points, pretend to remove each node in turn and see
  // if the number of connected components increases.
  for (int u = 0; u < nodes; u++) {
    if (count_cc(u) > num_cc) {
      printf("AP: %d\n", u);
    }
  }

  // To find bridges, remove each edge in turn and see if the number of
  // connected components increases.
  for (int u = 0; u < nodes - 1; u++) {
    for (int v = u + 1; v < nodes; v++) {
      if (adj[u][v]) {
        adj[u][v] = adj[v][u] = false;
        if (count_cc(NONE) > num_cc) {
          printf("B: %d-%d\n", u, v);
        }
        adj[u][v] = adj[v][u] = true;
      }
    }
  }

  // To find biconnected components, use a DSF to partition edges. Run a DFS
  // and, whenever we encounter a cycle, unite the edges' sets.
  for (int u = 0; u < nodes; u++) {
    if (!seen[u]) {
      dfs_bcc(u, u);
    }
  }
  for (int u = 0; u < nodes - 1; u++) {
    for (int v = u + 1; v < nodes; v++) {
      if (adj[u][v]) {
        printf("BCC:");
        int set = set_find(get_edge_index(u, v));
        for (int u2 = u; u2 < nodes - 1; u2++) {
          for (int v2 = u2 + 1; v2 < nodes; v2++) {
            if (adj[u2][v2] && (set_find(get_edge_index(u2, v2)) == set)) {
              printf(" %d-%d", u2, v2);
              adj[u2][v2] = false;
            }
          }
        }
        printf("\n");
      }
    }
  }

  return 0;
}
